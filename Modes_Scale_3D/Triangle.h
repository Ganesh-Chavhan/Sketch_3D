#pragma once

class Triangle {
private:
    int m_a;
    int m_b;
    int m_c;

public:
    Triangle(int a = 0, int b = 0, int c = 0);

    int getA() const;
    int getB() const;
    int getC() const;

    void setA(int a);
    void setB(int b);
    void setC(int c);
};