#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <cmath>

#define M_PI 3.14159265358979323846

// ---- Simple 2D shader (x,y,r,g,b) ----
static const char* vert2D = R"(
attribute vec2 aPos;
attribute vec3 aColor;
varying vec3 vColor;
uniform mat4 uProj;
void main() {
    vColor = aColor;
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
}
)";

static const char* frag2D = R"(
varying vec3 vColor;
void main() {
    gl_FragColor = vec4(vColor, 1.0);
}
)";

// ---- Simple 3D shader (x,y,z,r,g,b) ----
static const char* vert3D = R"(
attribute vec3 aPos;
attribute vec3 aColor;
varying vec3 vColor;
uniform mat4 uMVP;
void main() {
    vColor = aColor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

static const char* frag3D = R"(
varying vec3 vColor;
void main() {
    gl_FragColor = vec4(vColor, 1.0);
}
)";

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent), m_vbo(QOpenGLBuffer::VertexBuffer)
{
    m_shape.exists = false;
    m_shape.center = QPointF(0, 0);
    m_shape.type = SHAPE_RECTANGLE;
    m_shape.width = 100;
    m_shape.height = 80;
    m_shape.radius = 60;
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_vbo.destroy();
    delete m_program2D;
    delete m_program3D;
    doneCurrent();
}

void GLWidget::setMode(int mode)
{
    m_mode = mode;
    update();
}

void GLWidget::createShape(int type, float w, float h, float r)
{
    m_shape.type = type;
    m_shape.center = QPointF(width() / 2.0, height() / 2.0);
    m_shape.width = w;
    m_shape.height = h;
    m_shape.radius = r;
    m_shape.exists = true;
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // 2D shader
    m_program2D = new QOpenGLShaderProgram(this);
    m_program2D->addShaderFromSourceCode(QOpenGLShader::Vertex, vert2D);
    m_program2D->addShaderFromSourceCode(QOpenGLShader::Fragment, frag2D);
    m_program2D->link();

    // 3D shader
    m_program3D = new QOpenGLShaderProgram(this);
    m_program3D->addShaderFromSourceCode(QOpenGLShader::Vertex, vert3D);
    m_program3D->addShaderFromSourceCode(QOpenGLShader::Fragment, frag3D);
    m_program3D->link();

    m_vbo.create();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_shape.exists)
        return;

    if (m_mode == MODE_3D)
        draw3DShape();
    else
        draw2DShape();
}

// Convert widget pixel coords → OpenGL NDC-like coords for 2D

void GLWidget::build2DMesh()
{
    m_vertices2D.clear();

    if (m_shape.type == SHAPE_CIRCLE)
        buildCircle(m_vertices2D);
    else
        buildRect(m_vertices2D);
}

void GLWidget::buildCircle(QVector<float>& verts)
{
    // Triangle fan: center + ring
    float cx = m_shape.center.x();
    float cy = m_shape.center.y();
    float r = m_shape.radius;
    int   slices = 48;

    // Color: soft teal
    float cr = 0.2f, cg = 0.8f, cb = 0.7f;

    for (int i = 0; i < slices; i++) {
        float a1 = (float)i / slices * 2.0f * M_PI;
        float a2 = (float)(i + 1) / slices * 2.0f * M_PI;

        // center
        verts << cx << cy << cr << cg << cb;
        // p1
        verts << cx + r * cosf(a1) << cy + r * sinf(a1) << cr << cg << cb;
        // p2
        verts << cx + r * cosf(a2) << cy + r * sinf(a2) << cr << cg << cb;
    }
}

void GLWidget::buildRect(QVector<float>& verts)
{
    float cx = m_shape.center.x();
    float cy = m_shape.center.y();
    float hw = m_shape.width / 2.0f;
    float hh = m_shape.height / 2.0f;

    // Color: warm coral
    float r = 0.9f, g = 0.4f, b = 0.35f;

    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;

    // Triangle 1
    verts << x0 << y0 << r << g << b;
    verts << x1 << y0 << r << g << b;
    verts << x1 << y1 << r << g << b;
    // Triangle 2
    verts << x0 << y0 << r << g << b;
    verts << x1 << y1 << r << g << b;
    verts << x0 << y1 << r << g << b;
}

void GLWidget::draw2DShape()
{
    build2DMesh();

    if (m_vertices2D.isEmpty())
        return;

    // Ortho projection: pixel coords → [-1,1]
    float W = width();
    float H = height();
    // ortho: maps [0,W]x[0,H] to [-1,1]x[-1,1]  (flip Y so top=positive)
    QMatrix4x4 proj;
    proj.setToIdentity();
    proj.ortho(0, W, H, 0, -1, 1);

    m_program2D->bind();
    m_program2D->setUniformValue("uProj", proj);

    m_vbo.bind();
    m_vbo.allocate(m_vertices2D.constData(), m_vertices2D.size() * sizeof(float));

    // stride = 5 floats per vertex (x,y,r,g,b)
    int stride = 5 * sizeof(float);
    int posLoc = m_program2D->attributeLocation("aPos");
    int colorLoc = m_program2D->attributeLocation("aColor");

    m_program2D->enableAttributeArray(posLoc);
    m_program2D->setAttributeBuffer(posLoc, GL_FLOAT, 0, 2, stride);

    m_program2D->enableAttributeArray(colorLoc);
    m_program2D->setAttributeBuffer(colorLoc, GL_FLOAT, 2 * sizeof(float), 3, stride);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices2D.size() / 5);
    glEnable(GL_DEPTH_TEST);

    m_program2D->disableAttributeArray(posLoc);
    m_program2D->disableAttributeArray(colorLoc);
    m_vbo.release();
    m_program2D->release();
}

// 3D

void GLWidget::build3DMesh()
{
    m_vertices3D.clear();
    float depth = 100.0f;

    if (m_shape.type == SHAPE_CIRCLE)
        buildSphere(m_vertices3D, m_shape.radius);
    else
        buildCuboid(m_vertices3D, m_shape.width, m_shape.height, depth);
}

// Helper to add a quad face as 2 triangles with a given color
static void addFace(QVector<float>& v,
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float r, float g, float b)
{
    // Triangle 1: 0,1,2
    v << x0 << y0 << z0 << r << g << b;
    v << x1 << y1 << z1 << r << g << b;
    v << x2 << y2 << z2 << r << g << b;
    // Triangle 2: 0,2,3
    v << x0 << y0 << z0 << r << g << b;
    v << x2 << y2 << z2 << r << g << b;
    v << x3 << y3 << z3 << r << g << b;
}

void GLWidget::buildCuboid(QVector<float>& verts, float w, float h, float d)
{
    float hw = w / 2.0f, hh = h / 2.0f, hd = d / 2.0f;

    // Front  (blue)
    addFace(verts, -hw, -hh, hd, hw, -hh, hd, hw, hh, hd, -hw, hh, hd, 0.3f, 0.5f, 0.9f);
    // Back   (dark blue)
    addFace(verts, hw, -hh, -hd, -hw, -hh, -hd, -hw, hh, -hd, hw, hh, -hd, 0.15f, 0.25f, 0.6f);
    // Left   (green)
    addFace(verts, -hw, -hh, -hd, -hw, -hh, hd, -hw, hh, hd, -hw, hh, -hd, 0.3f, 0.75f, 0.4f);
    // Right  (dark green)
    addFace(verts, hw, -hh, hd, hw, -hh, -hd, hw, hh, -hd, hw, hh, hd, 0.15f, 0.45f, 0.2f);
    // Top    (yellow)
    addFace(verts, -hw, hh, hd, hw, hh, hd, hw, hh, -hd, -hw, hh, -hd, 0.9f, 0.8f, 0.2f);
    // Bottom (orange)
    addFace(verts, -hw, -hh, -hd, hw, -hh, -hd, hw, -hh, hd, -hw, -hh, hd, 0.85f, 0.45f, 0.1f);
}

void GLWidget::buildSphere(QVector<float>& verts, float r)
{
    int stacks = 16, slices = 24;

    for (int i = 0; i < stacks; i++) {
        float phi1 = (float)i / stacks * M_PI - M_PI / 2.0f;
        float phi2 = (float)(i + 1) / stacks * M_PI - M_PI / 2.0f;

        for (int j = 0; j < slices; j++) {
            float th1 = (float)j / slices * 2.0f * M_PI;
            float th2 = (float)(j + 1) / slices * 2.0f * M_PI;

            // 4 corners
            float x00 = r * cosf(phi1) * cosf(th1), y00 = r * sinf(phi1), z00 = r * cosf(phi1) * sinf(th1);
            float x10 = r * cosf(phi2) * cosf(th1), y10 = r * sinf(phi2), z10 = r * cosf(phi2) * sinf(th1);
            float x11 = r * cosf(phi2) * cosf(th2), y11 = r * sinf(phi2), z11 = r * cosf(phi2) * sinf(th2);
            float x01 = r * cosf(phi1) * cosf(th2), y01 = r * sinf(phi1), z01 = r * cosf(phi1) * sinf(th2);

            // Color based on latitude
            float t = (float)(i * slices + j) / (stacks * slices);
            float cr = 0.2f + 0.7f * t;
            float cg = 0.6f - 0.2f * t;
            float cb = 0.9f - 0.5f * t;

            addFace(verts, x00, y00, z00, x10, y10, z10, x11, y11, z11, x01, y01, z01, cr, cg, cb);
        }
    }
}

void GLWidget::draw3DShape()
{
    build3DMesh();

    if (m_vertices3D.isEmpty())
        return;

    // MVP: simple perspective + rotation
    QMatrix4x4 proj, view, model, mvp;
    proj.setToIdentity();
    proj.perspective(45.0f, (float)width() / height(), 1.0f, 2000.0f);

    view.setToIdentity();
    view.translate(0, 0, -350);

    model.setToIdentity();
    model.rotate(m_rotX, 1, 0, 0);
    model.rotate(m_rotY, 0, 1, 0);

    mvp = proj * view * model;

    m_program3D->bind();
    m_program3D->setUniformValue("uMVP", mvp);

    m_vbo.bind();
    m_vbo.allocate(m_vertices3D.constData(), m_vertices3D.size() * sizeof(float));

    int stride = 6 * sizeof(float);
    int posLoc = m_program3D->attributeLocation("aPos");
    int colorLoc = m_program3D->attributeLocation("aColor");

    m_program3D->enableAttributeArray(posLoc);
    m_program3D->setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, stride);

    m_program3D->enableAttributeArray(colorLoc);
    m_program3D->setAttributeBuffer(colorLoc, GL_FLOAT, 3 * sizeof(float), 3, stride);

    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices3D.size() / 6);

    m_program3D->disableAttributeArray(posLoc);
    m_program3D->disableAttributeArray(colorLoc);
    m_vbo.release();
    m_program3D->release();
}

// Mouse events

bool GLWidget::nearCorner(QPointF mouse)
{
    if (!m_shape.exists) return false;
    float cx = m_shape.center.x();
    float cy = m_shape.center.y();
    float threshold = 30.0f;

    if (m_shape.type == SHAPE_CIRCLE) {
        float dx = mouse.x() - cx;
        float dy = mouse.y() - cy;
        float dist = sqrtf(dx * dx + dy * dy);
        return fabsf(dist - m_shape.radius) < threshold;
    }
    else {
        float hw = m_shape.width / 2.0f;
        float hh = m_shape.height / 2.0f;
        // 4 corners
        QPointF corners[4] = {
            QPointF(cx - hw, cy - hh),
            QPointF(cx + hw, cy - hh),
            QPointF(cx + hw, cy + hh),
            QPointF(cx - hw, cy + hh)
        };
        for (auto& c : corners) {
            float dx = mouse.x() - c.x();
            float dy = mouse.y() - c.y();
            if (sqrtf(dx * dx + dy * dy) < threshold)
                return true;
        }
        return false;
    }
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_mode != MODE_EDIT) return;
    if (!m_shape.exists) return;

    QPointF pos = event->pos();
    if (nearCorner(pos)) {
        m_dragging = true;
        m_lastMouse = pos;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mode == MODE_3D) {
        // Rotate 3D shape
        QPointF delta = event->pos() - m_lastMouse;
        m_rotY += delta.x() * 0.5f;
        m_rotX += delta.y() * 0.5f;
        m_lastMouse = event->pos();
        update();
        return;
    }

    if (m_mode != MODE_EDIT || !m_dragging) return;

    QPointF mouse = event->pos();
    float dx = mouse.x() - m_shape.center.x();
    float dy = mouse.y() - m_shape.center.y();

    if (m_shape.type == SHAPE_CIRCLE) {
        m_shape.radius = sqrtf(dx * dx + dy * dy);
    }
    else if (m_shape.type == SHAPE_SQUARE) {
        float side = fmaxf(fabsf(dx), fabsf(dy));
        m_shape.width = side * 2.0f;
        m_shape.height = side * 2.0f;
    }
    else { // Rectangle
        m_shape.width = fabsf(dx) * 2.0f;
        m_shape.height = fabsf(dy) * 2.0f;
    }

    // Minimum size guard
    if (m_shape.type == SHAPE_CIRCLE && m_shape.radius < 10) m_shape.radius = 10;
    if (m_shape.type != SHAPE_CIRCLE) {
        if (m_shape.width < 20) m_shape.width = 20;
        if (m_shape.height < 20) m_shape.height = 20;
    }

    m_lastMouse = mouse;
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    m_dragging = false;
}