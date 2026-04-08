#include "meshbuilder.h"
#include <cmath>

#define PI 3.14159265f

// 2D SHAPE
void build2D(const Shape& s, QVector<float>& out) {
    out.clear();
    float cx = s.getCenter().x();
    float cy = s.getCenter().y();

    if (s.getType() == SHAPE_CIRCLE) {
        // Circle as triangle fan
        float r = s.getRadius();
        int segments = 48;

        // Center vertex
        out << cx << cy << 0.2f << 0.8f << 0.7f;

        // Ring vertices
        for (int i = 0; i <= segments; i++) {
            float angle = i * 2 * PI / segments;
            out << cx + r * cos(angle) << cy + r * sin(angle) << 0.2f << 0.8f << 0.7f;
        }
    }
    else {
        // Rectangle OR  Square  as 2 triangles
        float hw = s.getWidth() / 2;
        float hh = s.getHeight() / 2;
        float x0 = cx - hw, x1 = cx + hw;
        float y0 = cy - hh, y1 = cy + hh;

        out << x0 << y0 << 0.9f << 0.4f << 0.35f;
        out << x1 << y0 << 0.9f << 0.4f << 0.35f;
        out << x1 << y1 << 0.9f << 0.4f << 0.35f;

        out << x0 << y0 << 0.9f << 0.4f << 0.35f;
        out << x1 << y1 << 0.9f << 0.4f << 0.35f;
        out << x0 << y1 << 0.9f << 0.4f << 0.35f;
    }
}

// CUBOID MESH 8 vert and 6 faces (12 triangles)
void buildCuboid(DataClass& mesh, float w, float h, float d) {
    mesh.clear();

    float hw = w / 2;  
    float hh = h / 2;  
    float hd = d / 2; 

    int v0 = mesh.findOrAdd(-hw, hh, hd);  
    int v1 = mesh.findOrAdd(hw, hh, hd);  
    int v2 = mesh.findOrAdd(hw, -hh, hd); 
    int v3 = mesh.findOrAdd(-hw, -hh, hd);  
    int v4 = mesh.findOrAdd(-hw, hh, -hd);  
    int v5 = mesh.findOrAdd(hw, hh, -hd); 
    int v6 = mesh.findOrAdd(hw, -hh, -hd);  
    int v7 = mesh.findOrAdd(-hw, -hh, -hd);  

    // Add 12 triangles (2 per face)
    // Front face
    mesh.addTriangle(v0, v3, v2);
    mesh.addTriangle(v0, v2, v1);

    // Back face
    mesh.addTriangle(v5, v6, v7);
    mesh.addTriangle(v5, v7, v4);

    // Left face
    mesh.addTriangle(v4, v7, v3);
    mesh.addTriangle(v4, v3, v0);

    // Right face
    mesh.addTriangle(v1, v2, v6);
    mesh.addTriangle(v1, v6, v5);

    // Top face
    mesh.addTriangle(v4, v0, v1);
    mesh.addTriangle(v4, v1, v5);

    // Bottom face
    mesh.addTriangle(v3, v7, v6);
    mesh.addTriangle(v3, v6, v2);
}

// SPHERE MESH
void buildSphere(DataClass& mesh, float radius, int segments) {
    mesh.clear();

    // Store vertex indices in a 2D array for easy triangle building
    vector<vector<int>> grid(segments + 1, vector<int>(segments + 1));

    for (int i = 0; i <= segments; i++) {
        // phi = angle from bottom to top (-90 to +90 degrees)
        float phi = -PI / 2 + PI * i / segments;

        for (int j = 0; j <= segments; j++) {
            // theta = angle around the equator (0 to 360 degrees)
            float theta = 2 * PI * j / segments;

            //  standard sphere formula:
            float x = radius * cos(phi) * cos(theta);
            float y = radius * sin(phi); 
            float z = radius * cos(phi) * sin(theta);

            grid[i][j] = mesh.findOrAdd(x, y, z);
        }
    }

    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int v00 = grid[i][j];
            int v01 = grid[i][j + 1];
            int v10 = grid[i + 1][j];
            int v11 = grid[i + 1][j + 1];

            // Two triangles per grid square
            mesh.addTriangle(v00, v10, v11);
            mesh.addTriangle(v00, v11, v01);
        }
    }
}

// BUILD 3D MESH from 2D shape
void build3DMesh(const Shape& s, DataClass& mesh) {
    if (s.getType() == SHAPE_CIRCLE) {
        buildSphere(mesh, s.getRadius(), 32);  
    }
    else {
        if (s.getType() == SHAPE_SQUARE) {
            buildCuboid(mesh, s.getWidth(), s.getHeight(), s.getHeight());
        }
        else {
            float depth = 100.0f;  
            buildCuboid(mesh, s.getWidth(), s.getHeight(), depth);
        }
    }
}

// BUILD 3D for OpenGL rendering mesh to flat vertex array with colors
void build3D(const Shape& s, QVector<float>& out) {
    out.clear();

    DataClass mesh;
    build3DMesh(s, mesh);

    // Convert triangles to colored vertices for GPU
    for (size_t t = 0; t < mesh.triangles.size(); t++) {
        Triangle& tri = mesh.triangles[t];
        Vertex& v0 = mesh.vertices[tri.getA()];
        Vertex& v1 = mesh.vertices[tri.getB()];
        Vertex& v2 = mesh.vertices[tri.getC()];

        // Simple coloring: different color per triangle
        float shade = 0.3f + 0.7f * (float)t / mesh.triangles.size();
        float r, g, b;

        if (s.getType() == SHAPE_CIRCLE) {
            // Blue-ish sphere
            r = 0.2f * shade;
            g = 0.5f * shade;
            b = 0.9f * shade;
        }
        else {
            int face = t / 2;
            switch (face % 6) {
            case 0: r = 0.3f; g = 0.5f; b = 0.9f; break;  // blue
            case 1: r = 0.2f; g = 0.3f; b = 0.6f; break;  // dark blue
            case 2: r = 0.3f; g = 0.8f; b = 0.4f; break;  // green
            case 3: r = 0.2f; g = 0.5f; b = 0.3f; break;  // dark green
            case 4: r = 0.9f; g = 0.8f; b = 0.2f; break;  // yellow
            case 5: r = 0.9f; g = 0.5f; b = 0.2f; break;  // orange
            default: r = g = b = 0.5f; break;
            }
        }

        // Add 3 vertices 
        out << v0.getX() << v0.getY() << v0.getZ() << r << g << b;
        out << v1.getX() << v1.getY() << v1.getZ() << r << g << b;
        out << v2.getX() << v2.getY() << v2.getZ() << r << g << b;
    }
}
