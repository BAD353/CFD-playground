#pragma once

#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include <SFML/Graphics.hpp>

class VelocityRenderer {
  public:
    explicit VelocityRenderer(double cellSize);

    void draw(sf::RenderWindow &window, const Field2D<Vec2> &field);

  private:
    double m_cellSize;
    const double m_maxArrowVelocity = 5;
};