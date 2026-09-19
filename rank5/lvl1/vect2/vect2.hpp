#pragma once
#include <iostream>

class vect2 {
    int _x, _y;
public:
    vect2(int x = 0, int y = 0) : _x(x), _y(y) {}
    vect2(const vect2& o) : _x(o._x), _y(o._y) {}
    vect2& operator=(const vect2& o) { _x = o._x; _y = o._y; return *this; }
    ~vect2() {}

    int getX() const { return _x; }
    int getY() const { return _y; }
    int& operator[](int i) { return i ? _y : _x; }
    const int& operator[](int i) const { return i ? _y : _x; }

    vect2 operator+(const vect2& o) const { return vect2(_x + o._x, _y + o._y); }
    vect2 operator-(const vect2& o) const { return vect2(_x - o._x, _y - o._y); }
    vect2 operator*(int s) const { return vect2(_x * s, _y * s); }
    vect2 operator-() const { return vect2(-_x, -_y); }

    vect2& operator+=(const vect2& o) { _x += o._x; _y += o._y; return *this; }
    vect2& operator-=(const vect2& o) { _x -= o._x; _y -= o._y; return *this; }
    vect2& operator*=(int s) { _x *= s; _y *= s; return *this; }

    vect2& operator++() { ++_x; ++_y; return *this; }
    vect2 operator++(int) { vect2 t(*this); ++*this; return t; }
    vect2& operator--() { --_x; --_y; return *this; }
    vect2 operator--(int) { vect2 t(*this); --*this; return t; }

    bool operator==(const vect2& o) const { return _x == o._x && _y == o._y; }
    bool operator!=(const vect2& o) const { return !(*this == o); }
};

inline vect2 operator*(int s, const vect2& v) { return v * s; }
inline std::ostream& operator<<(std::ostream& os, const vect2& v) {
    return os << "{" << v.getX() << ", " << v.getY() << "}";
}
