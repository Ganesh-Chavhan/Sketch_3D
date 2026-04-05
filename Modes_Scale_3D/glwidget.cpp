#include "glwidget.h"
#include <cmath>

static const char* vert2D = R"(
attribute vec2 aPos; attribute vec3 aColor; varying vec3 vColor; uniform mat4 uProj;
void main() { vColor=aColor; gl_Position=uProj*vec4(aPos,0,1); })";

static const char* frag2D = R"(
varying vec3 vColor; void main() { gl_FragColor=vec4(vColor,1); })";

static const char* vert3D = R"(
attribute vec3 aPos; attribute vec3 aColor; varying vec3 vColor; uniform mat4 uMVP;
void main() { vColor=aColor; gl_Position=uMVP*vec4(aPos,1); })";

static const char* frag3D = R"(
varying vec3 vColor; void main() { gl_FragColor=vec4(vColor,1); })";

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent), m_vbo(QOpenGLBuffer::VertexBuffer)
{
    m_shape.exists = false;
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_vbo.destroy();
    delete m_prog2D;
    delete m_prog3D;
    doneCurrent();
}

void GLWidget::setMode(int mode) { m_mode = mode; update(); }

void GLWidget::createShape(int type, float w, float h, float r)
{
    m_shape = { type, QPointF(width() / 2.0, height() / 2.0), w, h, r, true };
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    m_prog2D = new QOpenGLShaderProgram(this);
    m_prog2D->addShaderFromSourceCode(QOpenGLShader::Vertex, vert2D);
    m_prog2D->addShaderFromSourceCode(QOpenGLShader::Fragment, frag2D);
    m_prog2D->link();

    m_prog3D = new QOpenGLShaderProgram(this);
    m_prog3D->addShaderFromSourceCode(QOpenGLShader::Vertex, vert3D);
    m_prog3D->addShaderFromSourceCode(QOpenGLShader::Fragment, frag3D);
    m_prog3D->link();

    m_vbo.create();
}

void GLWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_shape.exists) return;
    if (m_mode == MODE_3D) draw3DShape(); else draw2DShape();
}

void GLWidget::draw2DShape()
{
    build2D(m_shape, m_verts2D);

    QMatrix4x4 proj;
    proj.ortho(0, width(), height(), 0, -1, 1);

    m_prog2D->bind();
    m_prog2D->setUniformValue("uProj", proj);
    m_vbo.bind();
    m_vbo.allocate(m_verts2D.constData(), m_verts2D.size() * sizeof(float));

    int stride = 5 * sizeof(float);
    m_prog2D->enableAttributeArray("aPos");
    m_prog2D->setAttributeBuffer("aPos", GL_FLOAT, 0, 2, stride);
    m_prog2D->enableAttributeArray("aColor");
    m_prog2D->setAttributeBuffer("aColor", GL_FLOAT, 2 * sizeof(float), 3, stride);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_verts2D.size() / 5);
    glEnable(GL_DEPTH_TEST);

    m_prog2D->disableAttributeArray("aPos");
    m_prog2D->disableAttributeArray("aColor");
    m_vbo.release();
    m_prog2D->release();
}

void GLWidget::draw3DShape()
{
    build3D(m_shape, m_verts3D);

    QMatrix4x4 proj, view, model;
    proj.perspective(45.0f, (float)width() / height(), 1.0f, 2000.0f);
    view.translate(0, 0, -350);
    model.rotate(m_rotX, 1, 0, 0);
    model.rotate(m_rotY, 0, 1, 0);

    m_prog3D->bind();
    m_prog3D->setUniformValue("uMVP", proj * view * model);
    m_vbo.bind();
    m_vbo.allocate(m_verts3D.constData(), m_verts3D.size() * sizeof(float));

    int stride = 6 * sizeof(float);
    m_prog3D->enableAttributeArray("aPos");
    m_prog3D->setAttributeBuffer("aPos", GL_FLOAT, 0, 3, stride);
    m_prog3D->enableAttributeArray("aColor");
    m_prog3D->setAttributeBuffer("aColor", GL_FLOAT, 3 * sizeof(float), 3, stride);

    glDrawArrays(GL_TRIANGLES, 0, m_verts3D.size() / 6);

    m_prog3D->disableAttributeArray("aPos");
    m_prog3D->disableAttributeArray("aColor");
    m_vbo.release();
    m_prog3D->release();
}

bool GLWidget::nearCorner(QPointF mouse)
{
    float cx = m_shape.center.x(), cy = m_shape.center.y(), thresh = 30;

    if (m_shape.type == SHAPE_CIRCLE) {
        float dx = mouse.x() - cx, dy = mouse.y() - cy;
        return fabsf(sqrtf(dx * dx + dy * dy) - m_shape.radius) < thresh;
    }

    float hw = m_shape.width / 2, hh = m_shape.height / 2;
    QPointF corners[4] = { {cx - hw,cy - hh},{cx + hw,cy - hh},{cx + hw,cy + hh},{cx - hw,cy + hh} };
    for (auto& c : corners) {
        float dx = mouse.x() - c.x(), dy = mouse.y() - c.y();
        if (sqrtf(dx * dx + dy * dy) < thresh) return true;
    }
    return false;
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
    m_lastMouse = e->pos();
    if (m_mode == MODE_EDIT && m_shape.exists && nearCorner(e->pos()))
        m_dragging = true;
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (m_mode == MODE_3D) {
        QPointF d = e->pos() - m_lastMouse;
        m_rotY += d.x() * 0.5f;
        m_rotX += d.y() * 0.5f;
        m_lastMouse = e->pos();
        update();
        return;
    }

    if (m_mode != MODE_EDIT || !m_dragging) return;

    float dx = e->pos().x() - m_shape.center.x();
    float dy = e->pos().y() - m_shape.center.y();

    if (m_shape.type == SHAPE_CIRCLE)
        m_shape.radius = qMax(10.0f, sqrtf(dx * dx + dy * dy));
    else if (m_shape.type == SHAPE_SQUARE) {
        float s = qMax(20.0f, fmaxf(fabsf(dx), fabsf(dy)) * 2);
        m_shape.width = m_shape.height = s;
    }
    else {
        m_shape.width = qMax(20.0f, fabsf(dx) * 2);
        m_shape.height = qMax(20.0f, fabsf(dy) * 2);
    }

    m_lastMouse = e->pos();
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent*) { m_dragging = false; }