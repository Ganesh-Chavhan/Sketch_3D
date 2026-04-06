#pragma once
#include <QVector>
#include "Shape.h"
#include "DataClass.h"

// 2D drawing
void build2D(const Shape& s, QVector<float>& out);

// 3D mesh building 
void buildCuboid(DataClass& mesh, float w, float h, float d);
void buildSphere(DataClass& mesh, float radius, int segments);
void build3DMesh(const Shape& s, DataClass& mesh);

// 3D drawing (for 3D mode - converts mesh to GPU format)
void build3D(const Shape& s, QVector<float>& out);
