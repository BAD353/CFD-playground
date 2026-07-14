#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "simulation/Simulation.hpp"

void Simulation::applyAdvection(double dt) {
    // double cfl = m_velocity(15,15).length() * dt / m_grid.dx();
    // cout << m_velocity(15,15).length() << " " << dt << " " << m_grid.dx() <<
    // endl; cout << "velocity: " << m_velocity(15, 15)*dt << " cfl: " << cfl <<
    // endl;
    for (int y = 0; y < m_nextVelocity.height(); y++) {
        for (int x = 0; x < m_nextVelocity.width(); x++) {
            Vec2 pos = m_grid.cellCenter(x, y) - m_velocity(x, y) * dt;
            m_nextVelocity(x, y) = sampleVelocity(pos.x, pos.y);
        }
    }
}

void Simulation::applyDyeAdvection(double dt) {
    for (int y = 0; y < m_nextVelocity.height(); y++) {
        for (int x = 0; x < m_nextVelocity.width(); x++) {
            Vec2 pos = m_grid.cellCenter(x, y) - m_velocity(x, y) * dt;
            m_nextDye(x, y) = sampleDye(pos.x, pos.y);
        }
    }
}