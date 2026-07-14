#include <cmath>
#include <field/Field2D.hpp>
#include <simulation/Simulation.hpp>
using namespace std;

Simulation::Simulation(const Grid &grid)
    : m_grid(grid), m_velocity(m_grid), m_nextVelocity(m_grid), m_velocity0(m_grid), m_alpha(0.1),
      m_divergence(m_grid), m_pressure(m_grid), m_nextPressure(m_grid), m_dye(m_grid), m_nextDye(m_grid) {
    m_velocity.fill(Vec2{0.0, 0.0});
    m_nextVelocity.fill(Vec2{0.0, 0.0});
    m_dye.fill(0.0);
    m_nextDye.fill(0.0);
}

void Simulation::initializeVelocity() {
    for (int y = 0; y < m_velocity.height(); y++) {
        for (int x = 0; x < m_velocity.width(); x++) {
            m_velocity(x, y) = {0, 0};
        }
    }
}

void Simulation::initNextVelocity() {
    for (int y = 0; y < m_velocity.height(); y++) {
        for (int x = 0; x < m_velocity.width(); x++) {
            m_nextVelocity(x, y) = m_velocity(x, y);
        }
    }
}

void Simulation::initZeroPressure() {
    m_pressure.fill(0.0);
}

void Simulation::initNextPressure() {
    for (int y = 0; y < m_pressure.height(); y++) {
        for (int x = 0; x < m_pressure.width(); x++) {
            m_nextPressure(x, y) = m_pressure(x, y);
        }
    }
}

Field2D<Vec2> const &Simulation::velocity() const {
    return m_velocity;
}

Vec2 Simulation::sampleVelocity(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy() - 0.5;

    gx = clamp(gx, 0.0, m_velocity.width() - 1.001);
    gy = clamp(gy, 0.0, m_velocity.height() - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    return m_velocity(x0, y0) * (1 - fx) * (1 - fy) +
           m_velocity(x1, y0) * fx * (1 - fy) +
           m_velocity(x0, y1) * (1 - fx) * fy + m_velocity(x1, y1) * fx * fy;
}

double Simulation::sampleDye(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy() - 0.5;

    gx = clamp(gx, 0.0, m_dye.width() - 1.001);
    gy = clamp(gy, 0.0, m_dye.height() - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    return m_dye(x0, y0) * (1 - fx) * (1 - fy) +
           m_dye(x1, y0) * fx * (1 - fy) +
           m_dye(x0, y1) * (1 - fx) * fy + m_dye(x1, y1) * fx * fy;
}

Field2D<double> const &Simulation::divergence() const {
    return m_divergence;
}

Field2D<double> const &Simulation::dye() const {
    return m_dye;
}

Field2D<double> const &Simulation::pressure() const {
    return m_pressure;
}

void Simulation::update(double dt) {
    // applyGravityForce(dt);
    m_velocity0 = m_velocity;
    for (int i = 0; i < 50; i++) {
        applyViscosityJacobiIteration(dt);
        std::swap(m_nextVelocity, m_velocity);
        applyVelocityBoundary();
    }

    applyAdvection(dt);
    std::swap(m_nextVelocity, m_velocity);
    initNextVelocity();

    applyVelocityBoundary();

    compute_divergence();

    initZeroPressure();
    for (int i = 0; i < 50; i++) {
        applyPressureJacobiIteration();
        std::swap(m_nextPressure, m_pressure);
        applyPressureBoundary();
    }

    applyPressureGradient();
    std::swap(m_nextVelocity, m_velocity);
    initNextVelocity();

    applyVelocityBoundary();

    applyDyeAdvection(dt);
    std::swap(m_dye, m_nextDye);
}
