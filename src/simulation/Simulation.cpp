#include <cmath>
#include <field/Field2D.hpp>
#include <simulation/Simulation.hpp>
using namespace std;

Simulation::Simulation(const Grid &grid)
    : m_grid(grid), m_horizontalVelocity0(grid.width() + 1, grid.height()), m_horizontalVelocity(grid.width() + 1, grid.height()),
      m_nextHorizontalVelocity(grid.width() + 1, grid.height()), m_verticalVelocity0(grid.width(), grid.height() + 1),
      m_verticalVelocity(grid.width(), grid.height() + 1), m_nextVerticalVelocity(grid.width(), grid.height() + 1),
      m_pressure(grid), m_nextPressure(grid), m_divergence(grid), m_dye(grid), m_nextDye(grid), m_cellType(grid) {
    m_horizontalVelocity.fill(0.0);
    m_nextHorizontalVelocity.fill(0.0);
    m_verticalVelocity.fill(0.0);
    m_nextVerticalVelocity.fill(0.0);
    m_dye.fill(0.0);
    m_nextDye.fill(0.0);
}

void Simulation::initializeVelocity() {
    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        for (int x = 0; x < m_horizontalVelocity.width(); x++) {
            m_horizontalVelocity(x, y) = 10.0;
        }
    }
    for (int y = 0; y < m_verticalVelocity.height(); y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            m_verticalVelocity(x, y) = 0.0;
        }
    }
}

void Simulation::initNextVelocity() {
    for (int y = 0; y < m_horizontalVelocity.height(); y++) {
        for (int x = 0; x < m_horizontalVelocity.width(); x++) {
            m_nextHorizontalVelocity(x, y) = m_horizontalVelocity(x, y);
        }
    }
    for (int y = 0; y < m_verticalVelocity.height(); y++) {
        for (int x = 0; x < m_verticalVelocity.width(); x++) {
            m_nextVerticalVelocity(x, y) = m_verticalVelocity(x, y);
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

Field2D<double> const &Simulation::horizontalVelocity() const {
    return m_horizontalVelocity;
}

Field2D<double> const &Simulation::verticalVelocity() const {
    return m_verticalVelocity;
}

double Simulation::sampleHorizontalVelocity(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx();
    double gy = worldY / m_grid.dy() - 0.5;
    return sampleField(gx, gy, m_horizontalVelocity);
}

double Simulation::sampleVerticalVelocity(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy();
    return sampleField(gx, gy, m_verticalVelocity);
}

double Simulation::sampleDye(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy() - 0.5;
    return sampleField(gx, gy, m_dye);
}

double Simulation::sampleField(double gx, double gy, const Field2D<double> &field) const {

    gx = clamp(gx, 0.0, field.width() - 1.001);
    gy = clamp(gy, 0.0, field.height() - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    return field(x0, y0) * (1 - fx) * (1 - fy) + field(x1, y0) * fx * (1 - fy) + field(x0, y1) * (1 - fx) * fy +
           field(x1, y1) * fx * fy;
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

Field2D<CellType> const &Simulation::cellType() const {
    return m_cellType;
}

bool Simulation::isHorizontalFaceSolid(int x, int y) const {
    if (x > 0 && m_cellType(x - 1, y) == CellType::Solid)
        return true;
    if (x < m_cellType.width() && m_cellType(x, y) == CellType::Solid)
        return true;
    return false;
}

bool Simulation::isVerticalFaceSolid(int x, int y) const {
    if (y > 0 && m_cellType(x, y - 1) == CellType::Solid)
        return true;
    if (y < m_cellType.height() && m_cellType(x, y) == CellType::Solid)
        return true;
    return false;
}

void Simulation::update(double dt) {
    // applyGravityForce(dt);
    m_horizontalVelocity0 = m_horizontalVelocity;
    m_verticalVelocity0 = m_verticalVelocity;
    for (int i = 0; i < 50; i++) {
        m_nextHorizontalVelocity = m_horizontalVelocity;
        m_nextVerticalVelocity = m_verticalVelocity;
        applyViscosityJacobiIteration(dt);
        std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
        std::swap(m_nextVerticalVelocity, m_verticalVelocity);
        applyVelocityBoundary();
    }

    applyAdvection(dt);
    std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
    std::swap(m_nextVerticalVelocity, m_verticalVelocity);
    initNextVelocity();

    applyVelocityBoundary();

    compute_divergence();
    // cout << find_max_divergence() << endl;   

    initZeroPressure();
    for (int i = 0; i < 50; i++) {
        applyPressureJacobiIteration();
        std::swap(m_nextPressure, m_pressure);
        applyPressureBoundary();
    }

    applyPressureGradient();
    std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
    std::swap(m_nextVerticalVelocity, m_verticalVelocity);
    initNextVelocity();

    applyVelocityBoundary();

    applyDyeAdvection(dt);
    std::swap(m_dye, m_nextDye);
}
