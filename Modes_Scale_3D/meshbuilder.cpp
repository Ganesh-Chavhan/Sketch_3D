#include "meshbuilder.h"
#include <cmath>
#define PI 3.14159265f

// helper: push one quad as 2 triangles (3D)
static void addQuad(QVector<float>& v,
    float x0, float y0, float z0, float x1, float y1, float z1,
    float x2, float y2, float z2, float x3, float y3, float z3,
    float r, float g, float b)
{
    v << x0 << y0 << z0 << r << g << b << x1 << y1 << z1 << r << g << b << x2 << y2 << z2 << r << g << b;
    v << x0 << y0 << z0 << r << g << b << x2 << y2 << z2 << r << g << b << x3 << y3 << z3 << r << g << b;
}

void build2D(const Shape& s, QVector<float>& out)
{
    out.clear();
    float cx = s.center.x(), cy = s.center.y();

    if (s.type == SHAPE_CIRCLE) {
        float r = s.radius;
        for (int i = 0; i < 48; i++) {
            float a1 = (float)i / 48 * 2 * PI, a2 = (float)(i + 1) / 48 * 2 * PI;
            out << cx << cy << 0.2f << 0.8f << 0.7f;
            out << cx + r * cosf(a1) << cy + r * sinf(a1) << 0.2f << 0.8f << 0.7f;
            out << cx + r * cosf(a2) << cy + r * sinf(a2) << 0.2f << 0.8f << 0.7f;
        }
    }
    else {
        float hw = s.width / 2, hh = s.height / 2;
        float x0 = cx - hw, x1 = cx + hw, y0 = cy - hh, y1 = cy + hh;
        out << x0 << y0 << 0.9f << 0.4f << 0.35f << x1 << y0 << 0.9f << 0.4f << 0.35f << x1 << y1 << 0.9f << 0.4f << 0.35f;
        out << x0 << y0 << 0.9f << 0.4f << 0.35f << x1 << y1 << 0.9f << 0.4f << 0.35f << x0 << y1 << 0.9f << 0.4f << 0.35f;
    }
}

void build3D(const Shape& s, QVector<float>& out)
{
    out.clear();

    if (s.type == SHAPE_CIRCLE) {
        // sphere
        float r = s.radius;
        for (int i = 0; i < 16; i++) {
            float p1 = (float)i / 16 * PI - PI / 2, p2 = (float)(i + 1) / 16 * PI - PI / 2;
            for (int j = 0; j < 24; j++) {
                float t1 = (float)j / 24 * 2 * PI, t2 = (float)(j + 1) / 24 * 2 * PI;
                float x00 = r * cosf(p1) * cosf(t1), y00 = r * sinf(p1), z00 = r * cosf(p1) * sinf(t1);
                float x10 = r * cosf(p2) * cosf(t1), y10 = r * sinf(p2), z10 = r * cosf(p2) * sinf(t1);
                float x11 = r * cosf(p2) * cosf(t2), y11 = r * sinf(p2), z11 = r * cosf(p2) * sinf(t2);
                float x01 = r * cosf(p1) * cosf(t2), y01 = r * sinf(p1), z01 = r * cosf(p1) * sinf(t2);
                float t = (float)(i * 24 + j) / (16 * 24);
                addQuad(out, x00, y00, z00, x10, y10, z10, x11, y11, z11, x01, y01, z01,
                    0.2f + 0.7f * t, 0.6f - 0.2f * t, 0.9f - 0.5f * t);
            }
        }
    }
    else {
        // cuboid
        float w = s.width, h = s.height, d = 100;
        float hw = w / 2, hh = h / 2, hd = d / 2;
        addQuad(out, -hw, -hh, hd, hw, -hh, hd, hw, hh, hd, -hw, hh, hd, 0.3f, 0.5f, 0.9f);
        addQuad(out, hw, -hh, -hd, -hw, -hh, -hd, -hw, hh, -hd, hw, hh, -hd, 0.15f, 0.25f, 0.6f);
        addQuad(out, -hw, -hh, -hd, -hw, -hh, hd, -hw, hh, hd, -hw, hh, -hd, 0.3f, 0.75f, 0.4f);
        addQuad(out, hw, -hh, hd, hw, -hh, -hd, hw, hh, -hd, hw, hh, hd, 0.15f, 0.45f, 0.2f);
        addQuad(out, -hw, hh, hd, hw, hh, hd, hw, hh, -hd, -hw, hh, -hd, 0.9f, 0.8f, 0.2f);
        addQuad(out, -hw, -hh, -hd, hw, -hh, -hd, hw, -hh, hd, -hw, -hh, hd, 0.85f, 0.45f, 0.1f);
    }
}