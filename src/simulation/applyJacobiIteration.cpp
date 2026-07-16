#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

void Simulation::applyPressureJacobiIteration() {
    for (int y = 1; y < m_nextPressure.height() - 1; y++) {
        for (int x = 1; x < m_nextPressure.width() - 1; x++) {
            if (m_cellType(x,y) == CellType::Solid) {
                m_nextPressure(x,y) = 0;
                continue;
            }
            int count = 0;
            double sum = 0;
            if (m_cellType(x+1,y) != CellType::Solid) {
                sum += m_pressure(x + 1, y); count++;
            }
            if (m_cellType(x-1,y) != CellType::Solid) {
                sum += m_pressure(x - 1, y); count++;
            }
            if (m_cellType(x,y+1) != CellType::Solid) {
                sum += m_pressure(x, y + 1); count++;
            }
            if (m_cellType(x,y-1) != CellType::Solid) {
                sum += m_pressure(x, y - 1); count++;
            }
            if (count == 0) m_nextPressure(x,y) = 0;
            else m_nextPressure(x, y) =
                (sum -
                 m_grid.dx() * m_grid.dx() * m_divergence(x, y)) /
                count;
        }
    }
}

void Simulation::applyViscosityJacobiIteration(double dt) {
    const double alpha = viscosity * dt / (m_grid.dx()*m_grid.dx());
    for (int y = 1; y < m_horizontalVelocity.height() - 1; y++) {
        for (int x = 1; x < m_horizontalVelocity.width() - 1; x++) {
            m_nextHorizontalVelocity(x, y) =
                (m_horizontalVelocity0(x, y) +
                 (m_horizontalVelocity(x - 1, y) + m_horizontalVelocity(x + 1, y) +
                  m_horizontalVelocity(x, y - 1) + m_horizontalVelocity(x, y + 1)) *
                     alpha) /
                (1 + 4 * alpha);
        }
    }

    for (int y = 1; y < m_verticalVelocity.height() - 1; y++) {
        for (int x = 1; x < m_verticalVelocity.width() - 1; x++) {
            m_nextVerticalVelocity(x, y) =
                (m_verticalVelocity0(x, y) +
                 (m_verticalVelocity(x - 1, y) + m_verticalVelocity(x + 1, y) +
                  m_verticalVelocity(x, y - 1) + m_verticalVelocity(x, y + 1)) *
                     alpha) /
                (1 + 4 * alpha);
        }
    }
}