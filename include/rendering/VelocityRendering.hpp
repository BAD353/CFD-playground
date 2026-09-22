#pragma once

#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include <SFML/Graphics.hpp>

class VelocityRenderer {
  public:
    explicit VelocityRenderer(double cellSize);

    void draw(sf::RenderWindow &window, const Field2D<double> &horizontalField, const Field2D<double> &verticalField);

  private:
    double m_cellSize;
    const double m_maxArrowVelocity = 15;
};

void drawArrow(sf::RenderWindow &window, sf::Vector2f center, sf::Vector2f direction, double lengthFraction);