#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

constexpr Vec2 gravity(0.0, -9.81);

void Simulation::applyGravityForce(double dt) {
    for (int y = 0; y < m_verticalVelocity.height(); y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            m_verticalVelocity(x, y) = m_verticalVelocity(x, y) + gravity.y * dt;
        }
    }
}

void Simulation::applyMouseForce(Vec2 position, Vec2 force, double radius,
                                 double dt) {
    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        for (int x = 0; x < m_horizontalVelocity.width(); x++) {
            double distance2 = (y + 0.5 - position.y) * (y + 0.5 - position.y) +
                               (x - position.x) * (x - position.x);

            if (distance2 < radius * radius) {
                double influence =
                    (radius * radius - distance2) / (radius * radius);
                m_horizontalVelocity(x, y) = m_horizontalVelocity(x, y) + force.x * influence;
            }
        }
    }
    for (int y = 0; y < m_verticalVelocity.height(); y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            double distance2 = (y - position.y) * (y - position.y) +
                               (x + 0.5 - position.x) * (x + 0.5 - position.x);

            if (distance2 < radius * radius) {
                double influence =
                    (radius * radius - distance2) / (radius * radius);
                m_verticalVelocity(x, y) = m_verticalVelocity(x, y) + force.y * influence;
            }
        }
    }
}