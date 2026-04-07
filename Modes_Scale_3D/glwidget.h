#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include "meshbuilder.h"
#include "DataClass.h"

// Modes
#define MODE_SKETCH 0
#define MODE_EDIT   1
#define MODE_3D     2

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    // Mode switching
    void setMode(int mode);
    int getMode() const { return m_mode; }

    // Shape creation
    void createShape(int type, float w, float h, float r);
    bool hasShape() const { return m_shape.exists(); }

    // Mouse-based shape creation
    void setPendingShape(int type);
    void clearAll();

    // STL export/import
    bool exportSTL(const QString& path);
    bool loadSTL(const QString& path);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    // Current state
    int m_mode = MODE_SKETCH;
    Shape m_shape;
    DataClass m_mesh;  // Triangulated mesh for 3D

    // Mouse interaction
    bool m_dragging = false;
    QPointF m_lastMouse;
    float m_rotX = 25, m_rotY = 30;

    // OpenGL stuff
    QOpenGLShaderProgram* m_prog2D = nullptr;
    QOpenGLShaderProgram* m_prog3D = nullptr;
    QOpenGLBuffer m_vbo;
    QVector<float> m_verts2D, m_verts3D;

    // Drawing state
    bool m_isDrawing = false;
    int m_pendingShapeType = -1;
    QPointF m_drawStart;

    // Helper functions
    void draw2DShape();
    void draw3DShape();
    bool nearCorner(QPointF mouse);
};
