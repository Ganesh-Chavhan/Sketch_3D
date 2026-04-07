#include "DataClass.h"
#include <fstream>
#include <cmath>
#include <QDebug>

void DataClass::clear() {
    vertices.clear();
    triangles.clear();
    vertexMap.clear(); 
}

// Convert float to grid cell for tolerance-based comparison
int DataClass::snapToGrid(float value) {
    return static_cast<int>(round(value / VERTEX_TOLERANCE));
}

int DataClass::findOrAdd(float x, float y, float z) {
    // Snap coordinates to grid cells
    int gx = snapToGrid(x);
    int gy = snapToGrid(y);
    int gz = snapToGrid(z);

    auto key = make_tuple(gx, gy, gz);

    auto it = vertexMap.find(key);
    if (it != vertexMap.end()) {
        return it->second;
    }

    // not foind add new vertex
    int newIndex = vertices.size();
    vertices.push_back(Vertex(x, y, z));
    vertexMap[key] = newIndex;

    return newIndex;
}

void DataClass::addTriangle(int a, int b, int c) {
    triangles.push_back(Triangle(a, b, c));
}

bool DataClass::exportSTL(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << "solid mesh\n";

    for (const Triangle& tri : triangles) {
        Vertex v0 = vertices[tri.getA()];
        Vertex v1 = vertices[tri.getB()];
        Vertex v2 = vertices[tri.getC()];

        // Calculate normal using cross product
        // Edge vectors: e1 = v1 - v0, e2 = v2 - v0
        float e1x = v1.getX() - v0.getX();
        float e1y = v1.getY() - v0.getY();
        float e1z = v1.getZ() - v0.getZ();
        float e2x = v2.getX() - v0.getX();
        float e2y = v2.getY() - v0.getY();
        float e2z = v2.getZ() - v0.getZ();

        // Normal = e1 x e2
        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;

        // Normalize
        float len = sqrt(nx * nx + ny * ny + nz * nz);
        if (len > 0) { nx /= len; ny /= len; nz /= len; }

        file << "  facet normal " << nx << " " << ny << " " << nz << "\n";
        file << "    outer loop\n";
        file << "      vertex " << v0.getX() << " " << v0.getY() << " " << v0.getZ() << "\n";
        file << "      vertex " << v1.getX() << " " << v1.getY() << " " << v1.getZ() << "\n";
        file << "      vertex " << v2.getX() << " " << v2.getY() << " " << v2.getZ() << "\n";
        file << "    endloop\n";
        file << "  endfacet\n";
    }

    file << "endsolid mesh\n";
    file.close();
    return true;
}

// Load from ASCII STL file
bool DataClass::loadSTL(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    clear();

    string word;
    float x, y, z;
    int id[3];
    int count = 0;

    while (file >> word) {
        if (word == "vertex")
        {
            file >> x >> y >> z;

            if (count < 3) {
                id[count] = findOrAdd(x, y, z);
                count++;
            }
        }
        if (word == "endfacet")
        {
            addTriangle(id[0], id[1], id[2]);
            count = 0;
        }
    }

    file.close();

    qDebug() << "Loaded STL:" << vertices.size() << "vertices," << triangles.size() << "triangles";
    return triangles.size() > 0;
}


void DataClass::printInfo() {
    qDebug() << "=== Mesh Info ===";
    qDebug() << "Vertices:" << vertices.size();
    qDebug() << "Triangles:" << triangles.size();
}
