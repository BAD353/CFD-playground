#include "simulation/Simulation.hpp"

void Simulation::injectDye(Vec2 position, double radius, double dt) {
    const double dye_factor = 0.005;

    for (int y = 0; y < m_velocity.height(); y++) {
        for (int x = 0; x < m_velocity.width(); x++) {
            double distance2 = (y + 0.5 - position.y) * (y + 0.5 - position.y) +
                               (x + 0.5 - position.x) * (x + 0.5 - position.x);

            if (distance2 < radius * radius) {
                double influence =
                    (radius * radius - distance2) / (radius * radius);
                m_dye(x, y) = min(1.0, m_dye(x, y) + dye_factor * influence);
            }
        }
    }
}