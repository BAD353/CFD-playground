#include "simulation/Simulation.hpp"

void Simulation::compute_divergence() {
    Field2D<double> divergence(m_grid);
    double dx = m_grid.dx();
    double dy = m_grid.dy();
    for (int y = 1; y < m_grid.height() - 1; y++) {
        for (int x = 1; x < m_grid.width() - 1; x++) {
            divergence(x, y) =
                (m_velocity(x + 1, y).x - m_velocity(x - 1, y).x) / (2 * dx) +
                (m_velocity(x, y + 1).y - m_velocity(x, y - 1).y) / (2 * dy);
        }
    }
    std::swap(m_divergence, divergence);
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
