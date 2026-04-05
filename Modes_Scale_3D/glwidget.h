#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QVector>

// Shape types
#define SHAPE_CIRCLE    0
#define SHAPE_RECTANGLE 1
#define SHAPE_SQUARE    2

// App modes
#define MODE_SKETCH 0
#define MODE_EDIT   1
#define MODE_3D     2

struct Shape {
    int type;       // 0=circle, 1=rectangle, 2=square
    QPointF center;
    float width;
    float height;
    float radius;
    bool exists;    // has a shape been created?
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void setMode(int mode);
    void createShape(int type, float w, float h, float r);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // Mode
    int m_mode = MODE_SKETCH;

    // The one shape
    Shape m_shape;

    // Edit mode state
    bool m_dragging = false;
    QPointF m_lastMouse;

    // OpenGL
    QOpenGLShaderProgram* m_program2D = nullptr;
    QOpenGLShaderProgram* m_program3D = nullptr;
    QOpenGLBuffer m_vbo;

    // Vertex data
    QVector<float> m_vertices2D;   // x,y,r,g,b
    QVector<float> m_vertices3D;   // x,y,z,r,g,b

    // Build mesh helpers
    void build2DMesh();
    void build3DMesh();

    void draw2DShape();
    void draw3DShape();

    void buildCircle(QVector<float>& verts);
    void buildRect(QVector<float>& verts);
    void buildCuboid(QVector<float>& verts, float w, float h, float d);
    void buildSphere(QVector<float>& verts, float r);

    // Rotation for 3D
    float m_rotX = 25.0f;
    float m_rotY = 30.0f;

    // Check if point is near a corner (for edit mode)
    bool nearCorner(QPointF mouse);

    // Convert widget coords to OpenGL coords
    QPointF toGL(QPointF p);
};