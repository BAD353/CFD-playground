#include <simulation/Simulation.hpp>

void Simulation::applyVelocityBoundary() {
    for (int y = 0; y < m_velocity.height(); y++) {
        m_velocity(0, y).x = 0.0;
        m_velocity(m_velocity.width() - 1, y).x = 0.0;
    }

    for (int x = 0; x < m_velocity.width(); x++) {
        m_velocity(x, 0).y = 0.0;
        m_velocity(x, m_velocity.height() - 1).y = 0.0;
    }
}

void Simulation::applyPressureBoundary() {
    for (int y = 0; y < m_pressure.height(); y++) {
        m_pressure(0, y) = m_pressure(1, y);
        m_pressure(m_pressure.width() - 1, y) = m_pressure(m_pressure.width() - 2, y);
    }

    for (int x = 0; x < m_pressure.width(); x++) {
        m_pressure(x, 0) = m_pressure(x, 1);
        m_pressure(x, m_pressure.height() - 1) = m_pressure(x, m_pressure.height() - 2);
    }
}