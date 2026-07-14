#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

struct Vec2 {
    double x;
    double y;

    Vec2 operator+(const Vec2 &other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2 &other) const {
        return {x - other.x, y - other.y};
    }

    Vec2 operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    Vec2 operator/(double scalar) const {
        return {x / scalar, y / scalar};
    }

    double operator*(const Vec2 &other) const {
        return x * other.x + y * other.y;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Vec2 &v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}