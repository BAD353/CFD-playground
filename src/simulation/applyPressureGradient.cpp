#include <simulation/Simulation.hpp>

void Simulation::applyPressureGradient() {
    for (int y = 1; y < m_pressure.height(); y++) {
        for (int x = 0; x < m_pressure.width(); x++) {
            double dpdy =
                (m_pressure(x, y) - m_pressure(x, y - 1)) / (m_grid.dy());

            m_nextVerticalVelocity(x, y) =  m_verticalVelocity(x, y) - dpdy;
        }
    }
    for (int y = 0; y < m_pressure.height(); y++) {
        for (int x = 1; x < m_pressure.width(); x++) {
            double dpdx =
                (m_pressure(x, y) - m_pressure(x - 1, y)) / (m_grid.dx());
       
            m_nextHorizontalVelocity(x, y) =  m_horizontalVelocity(x, y) - dpdx;
        }
    }
}