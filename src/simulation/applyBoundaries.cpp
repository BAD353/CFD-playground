#include <simulation/Simulation.hpp>

void Simulation::applyVelocityBoundary() {
    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        m_horizontalVelocity(0, y) = 10.0;
        // m_horizontalVelocity(m_horizon   talVelocity.width() - 1, y) = 3.0;
    }

    for (int x = 0; x < m_verticalVelocity.width(); x++) {
        // m_verticalVelocity(x, 0) = 0.0;
        // m_verticalVelocity(x, m_verticalVelocity.height() - 1) = 0.0;
    }

    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        for (int x = 0; x < m_horizontalVelocity.width(); x++) {
            if (isHorizontalFaceSolid(x, y))
                m_horizontalVelocity(x, y) = 0.0;
        }
    }

    for (int y = 0; y < m_verticalVelocity.height(); y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            if (isVerticalFaceSolid(x, y))
                m_verticalVelocity(x, y) = 0.0;
        }
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

    for (int x = 0; x < m_pressure.width(); x++) {
        for (int y = 0; y < m_pressure.height(); y++) {
            if (m_cellType(x, y) == CellType::Solid)
                m_pressure(x, y) = 0;
        }
    }
}