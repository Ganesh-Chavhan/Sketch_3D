#pragma once
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "Vertex.h"
#include "Triangle.h"
using namespace std;

// Tolerance for comparing floating point values
const float VERTEX_TOLERANCE = 0.0001f;

class DataClass {
public:
    vector<Vertex> vertices;
    vector<Triangle> triangles;

    void clear();

    int findOrAdd(float x, float y, float z);

    void addTriangle(int a, int b, int c);

    bool exportSTL(const string& filename);

    bool loadSTL(const string& filename);

    void printInfo();

private:
    map<string, int> vertexMap;

    int snapToGrid(float value);
};
