#pragma once

class Vertex {
private:
    float m_x;
    float m_y;
    float m_z;

public:
    Vertex(float x = 0, float y = 0, float z = 0);

    float getX() const;
    float getY() const;
    float getZ() const;

    void setX(float x);
    void setY(float y);
    void setZ(float z);
};