#pragma once
#include <QPointF>

// Shape Types
#define SHAPE_SQUARE    0
#define SHAPE_RECTANGLE 1
#define SHAPE_CIRCLE    2

class Shape {
private:
    int m_type;
    QPointF m_center;
    float m_width, m_height, m_radius;
    bool m_exists;

public:
    Shape()
        : m_type(0), m_width(0), m_height(0), m_radius(0), m_exists(false) {
    }

    int getType() const {
        return m_type; 
    }
    QPointF getCenter() const { 
        return m_center; 
    }
    float getWidth() const {
        return m_width; 
    }
    float getHeight() const {
        return m_height; 
    }
    float getRadius() const {
        return m_radius; 
    }
    bool exists() const { 
        return m_exists;
    }

    void setType(int type) {
        m_type = type; 
    }
    void setCenter(const QPointF& center) { 
        m_center = center; 
    }
    void setWidth(float w) {
        m_width = w;
    }
    void setHeight(float h) { 
        m_height = h; 
    }
    void setRadius(float r) {
        m_radius = r; 
    }
    void setExists(bool e) {
        m_exists = e; 
    }
};
