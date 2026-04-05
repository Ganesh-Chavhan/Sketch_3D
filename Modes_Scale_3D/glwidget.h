#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include "meshbuilder.h"

#define MODE_SKETCH 0
#define MODE_EDIT   1
#define MODE_3D     2

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void setMode(int mode);
    void createShape(int type, float w, float h, float r);

protected:
    void initializeGL()         override;
    void resizeGL(int w, int h) override;
    void paintGL()              override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    int    m_mode = MODE_SKETCH;
    Shape  m_shape;
    bool   m_dragging = false;
    QPointF m_lastMouse;
    float  m_rotX = 25, m_rotY = 30;

    QOpenGLShaderProgram* m_prog2D = nullptr;
    QOpenGLShaderProgram* m_prog3D = nullptr;
    QOpenGLBuffer m_vbo;
    QVector<float> m_verts2D, m_verts3D;

    void draw2DShape();
    void draw3DShape();
    bool nearCorner(QPointF p);
};