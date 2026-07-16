#include <mesh/Grid.hpp>

Grid::Grid(int width, int height, double dx, double dy)
    : m_width(width), m_height(height), m_dx(dx), m_dy(dy) {
}

int Grid::width() const {
    return m_width;
}
int Grid::height() const {
    return m_height;
}
double Grid::dx() const {
    return m_dx;
}
double Grid::dy() const {
    return m_dy;
}

Vec2 Grid::cellCenter(int x, int y) const {
    return {(x + 0.5) * dx(), (y + 0.5) * dy()};
}

Vec2 Grid::cellHorizontalCenter(int x, int y) const {
    return {x * dx(), (y + 0.5) * dy()};
}

Vec2 Grid::cellVerticalCenter(int x, int y) const {
    return {(x + 0.5) * dx(), y * dy()};
}