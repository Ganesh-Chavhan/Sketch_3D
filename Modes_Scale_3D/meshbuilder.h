#pragma once
#include <QVector>
#include <QPointF>

// All geometry lives here so glwidget.cpp stays small.

struct Shape {
    int     type;
    QPointF center;
    float   width, height, radius;
    bool    exists;
};

#define SHAPE_CIRCLE    0
#define SHAPE_RECTANGLE 1
#define SHAPE_SQUARE    2

// 2D: x,y,r,g,b per vertex
void build2D(const Shape& s, QVector<float>& out);

// 3D: x,y,z,r,g,b per vertex
void build3D(const Shape& s, QVector<float>& out);