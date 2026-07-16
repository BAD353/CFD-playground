#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

constexpr Vec2 gravity(0.0, -9.81);

void Simulation::updateCellType(Vec2 position, CellType cellType, double radius) {
    for (int y = 0; y < m_cellType.height(); y++) {
        for (int x = 0; x < m_cellType.width(); x++) {
            double distance2 = (y + 0.5 - position.y) * (y + 0.5 - position.y) +
                               (x + 0.5 - position.x) * (x + 0.5 - position.x);

            if (distance2 < radius * radius) {
                m_cellType(x, y) = cellType;
            }
        }
    }
}