#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

void Simulation::applyAdvection(double dt) {
    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        for (int x = 1; x < m_horizontalVelocity.width() - 1; x++) {
            if (isHorizontalFaceSolid(x,y)) continue;
            Vec2 pos = m_grid.cellHorizontalCenter(x, y) -
                       Vec2(m_horizontalVelocity(x, y), (m_verticalVelocity(x - 1, y) + m_verticalVelocity(x, y) +
                                                         m_verticalVelocity(x - 1, y + 1) + m_verticalVelocity(x, y + 1)) /
                                                            4) *
                           dt;
            m_nextHorizontalVelocity(x, y) = sampleHorizontalVelocity(pos.x, pos.y);
        }
    }

    for (int y = 1; y < m_verticalVelocity.height() - 1; y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            if (isVerticalFaceSolid(x,y)) continue;
            Vec2 pos =
                m_grid.cellVerticalCenter(x, y) - Vec2((m_horizontalVelocity(x, y - 1) + m_horizontalVelocity(x + 1, y - 1) +
                                                        m_horizontalVelocity(x, y) + m_horizontalVelocity(x + 1, y)) /
                                                           4,
                                                       m_verticalVelocity(x, y)) *
                                                      dt;
            m_nextVerticalVelocity(x, y) = sampleVerticalVelocity(pos.x, pos.y);
        }
    }
}

void Simulation::applyDyeAdvection(double dt) {
    for (int y = 0; y < m_dye.height(); y++) {
        for (int x = 0; x < m_dye.width(); x++) {
            if (m_cellType(x,y) == CellType::Solid) continue;
            Vec2 pos = m_grid.cellCenter(x, y) - Vec2((m_horizontalVelocity(x, y) + m_horizontalVelocity(x + 1, y)) / 2,
                                                      (m_verticalVelocity(x, y) + m_verticalVelocity(x, y + 1) / 2)) *
                                                     dt;
            m_nextDye(x, y) = sampleDye(pos.x, pos.y);
        }
    }
}