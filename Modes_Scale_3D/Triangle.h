#pragma once

class Triangle {
private:
    int m_a, m_b, m_c;

public:
    Triangle(int a = 0, int b = 0, int c = 0)
        : m_a(a), m_b(b), m_c(c) {
    }

    int getA() const { return m_a; }
    int getB() const { return m_b; }
    int getC() const { return m_c; }

    void setA(int a) { m_a = a; }
    void setB(int b) { m_b = b; }
    void setC(int c) { m_c = c; }
};
