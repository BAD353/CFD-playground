#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

void Simulation::applyPressureJacobiIteration() {
    for (int y = 1; y < m_nextPressure.height() - 1; y++) {
        for (int x = 1; x < m_nextPressure.width() - 1; x++) {
            m_nextPressure(x, y) =
                (m_pressure(x + 1, y) + m_pressure(x - 1, y) +
                 m_pressure(x, y + 1) + m_pressure(x, y - 1) -
                 m_grid.dx() * m_grid.dx() * m_divergence(x, y)) /
                4.0;
        }
    }
}

void Simulation::applyViscosityJacobiIteration(double dt) {
    const double alpha = viscosity * dt / (m_grid.dx()*m_grid.dx());
    for (int y = 1; y < m_velocity.height() - 1; y++) {
        for (int x = 1; x < m_velocity.width() - 1; x++) {
            m_nextVelocity(x, y) =
                (m_velocity0(x, y) +
                 (m_velocity(x - 1, y) + m_velocity(x + 1, y) +
                  m_velocity(x, y - 1) + m_velocity(x, y + 1)) *
                     alpha) /
                (1 + 4 * alpha);
        }
    }
}