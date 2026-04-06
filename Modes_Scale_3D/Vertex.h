#pragma once

class Vertex {
private:
    float m_x, m_y, m_z;

public:
    Vertex(float x = 0, float y = 0, float z = 0)
        : m_x(x), m_y(y), m_z(z) {
    }

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getZ() const { return m_z; }

    void setX(float x) { m_x = x; }
    void setY(float y) { m_y = y; }
    void setZ(float z) { m_z = z; }
};
