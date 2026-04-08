#include "Vertex.h"

Vertex::Vertex(float x, float y, float z)
    : m_x(x), m_y(y), m_z(z) {}

float Vertex::getX() const {
    return m_x;
}

float Vertex::getY() const {
    return m_y;
}

float Vertex::getZ() const {
    return m_z;
}

void Vertex::setX(float x) {
    m_x = x;
}

void Vertex::setY(float y) {
    m_y = y;
}

void Vertex::setZ(float z) {
    m_z = z;
}