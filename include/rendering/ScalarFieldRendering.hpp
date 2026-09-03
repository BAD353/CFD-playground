#pragma once

#include "field/Field2D.hpp"
#include <SFML/Graphics.hpp>
#include <simulation/Simulation.hpp>
using namespace std;

class ScalarFieldRenderer {
  public:
    explicit ScalarFieldRenderer(int cellSize, int width, int height);

    void drawDivergence(sf::RenderWindow &window, const Field2D<double> &field);
    void drawDye(sf::RenderWindow &window, const Field2D<double> &field);
    void drawPressure(sf::RenderWindow &window, const Field2D<double> &field);
    void drawVelocityMagnitude(sf::RenderWindow &window, const Field2D<double> &horizontalVelocity,
                                                const Field2D<double> &verticalVelocity);
    void drawCellType(sf::RenderWindow &window, const Field2D<CellType> &field);

  private:
    int m_cellSize;
    
    vector<uint8_t> m_pixels;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};