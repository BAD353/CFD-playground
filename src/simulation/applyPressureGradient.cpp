#include <simulation/Simulation.hpp>

void Simulation::applyPressureGradient() {
    for (int y = 1; y < m_velocity.height() - 1; y++) {
        for (int x = 1; x < m_velocity.width() - 1; x++) {
            double dpdx =
                (m_pressure(x + 1, y) - m_pressure(x - 1, y)) / (2 * m_grid.dx());
            double dpdy =
                (m_pressure(x, y + 1) - m_pressure(x, y - 1)) / (2 * m_grid.dy());

            m_nextVelocity(x, y).x =  m_velocity(x, y).x - dpdx;
            m_nextVelocity(x, y).y =  m_velocity(x, y).y - dpdy;
        }
    }
}