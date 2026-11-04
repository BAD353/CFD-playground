#include "simulation/Simulation.hpp"

void Simulation::compute_divergence() {
    double dx = m_grid.dx();
    double dy = m_grid.dy();
    double tot_divergence = 0.0;
    int cellCount = 0;
    for (int y = 1; y < m_grid.height() - 1; y++) {
        for (int x = 1; x < m_grid.width() - 1; x++) {
            if (m_cellType(x, y) == CellType::Solid) {
                m_divergence(x, y) = 0;
                continue;
            }
            m_divergence(x, y) = (m_horizontalVelocity(x + 1, y) - m_horizontalVelocity(x, y)) / (dx) +
                                 (m_verticalVelocity(x, y + 1) - m_verticalVelocity(x, y)) / (dy);
            tot_divergence += m_divergence(x, y);
            cellCount++;
        }
    }
    double mean_divergence = cellCount > 0 ? tot_divergence / cellCount: 0;
    for (int y = 1; y < m_grid.height() - 1; y++) {
        for (int x = 1; x < m_grid.width() - 1; x++) {
            if (m_cellType(x, y) != CellType::Solid) {
                m_divergence(x, y) -= mean_divergence;
            }
        }
    }
    
}

double Simulation::find_max_divergence() {
    double max_div = 0.0;
    for (int y = 1; y < m_grid.height() - 1; y++) {
        for (int x = 1; x < m_grid.width() - 1; x++) {
            max_div = std::max(max_div, std::abs(m_divergence(x, y)));
        }
    }
    return max_div;
}
