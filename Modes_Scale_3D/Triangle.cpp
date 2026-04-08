#include "Triangle.h"

Triangle::Triangle(int a, int b, int c)
    : m_a(a), m_b(b), m_c(c){}

int Triangle::getA() const {
    return m_a;
}

int Triangle::getB() const {
    return m_b;
}

int Triangle::getC() const {
    return m_c;
}

void Triangle::setA(int a) {
    m_a = a;
}

void Triangle::setB(int b) {
    m_b = b;
}

void Triangle::setC(int c) {
    m_c = c;
}