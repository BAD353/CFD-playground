#pragma once
#include "math/Vec2.hpp"

class Grid {
  public:
    Grid(int width, int height, double dx, double dy);

    int width() const;
    int height() const;

    double dx() const;
    double dy() const;

    Vec2 cellCenter(int x, int y) const;

  private:
    int m_width;
    int m_height;

    double m_dx;
    double m_dy;
};