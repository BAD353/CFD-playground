#include <cmath>
#include <field/Field2D.hpp>
#include <simulation/Simulation.hpp>
#include <cuda/main.cuh>
#include <cstdint>
#include <chrono>
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
    return sampleField(gx, gy, m_horizontalVelocity, [this](int x, int y) { return isHorizontalFaceSolid(x, y); });
}

double Simulation::sampleVerticalVelocity(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy();
    return sampleField(gx, gy, m_verticalVelocity, [this](int x, int y) { return isVerticalFaceSolid(x, y); });
}

double Simulation::sampleDye(double worldX, double worldY) const {
    double gx = worldX / m_grid.dx() - 0.5;
    double gy = worldY / m_grid.dy() - 0.5;
    return sampleField(gx, gy, m_dye, [this](int x, int y) { return m_cellType(x, y) == CellType::Solid; });
}

bool Simulation::isPositionSolid(Vec2 pos) const {
    int cx = static_cast<int>(std::floor(pos.x / m_grid.dx()));
    int cy = static_cast<int>(std::floor(pos.y / m_grid.dy()));
    cx = std::clamp(cx, 0, m_cellType.width() - 1);
    cy = std::clamp(cy, 0, m_cellType.height() - 1);
    return m_cellType(cx, cy) == CellType::Solid;
}

double Simulation::sampleField(double gx, double gy, const Field2D<double> &field, const std::function<bool(int,int)> &isSolid) const {

    gx = clamp(gx, 0.0, field.width() - 1.001);
    gy = clamp(gy, 0.0, field.height() - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    double w00 = (1 - fx) * (1 - fy);
    double w10 = fx * (1 - fy);
    double w01 = (1 - fx) * fy;
    double w11 = fx * fy;

    if (isSolid(x0, y0)) w00 = 0;
    if (isSolid(x1, y0)) w10 = 0;
    if (isSolid(x0, y1)) w01 = 0;
    if (isSolid(x1, y1)) w11 = 0;

    //Note: divide by total to renormalize
    return (w00 * field(x0, y0) + w10 * field(x1, y0) + w01 * field(x0, y1) + w11 * field(x1, y1)) / (w00 + w10 + w01 + w11);
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


vector<float> toFloatBuffer(const Field2D<double> &field) {
    vector<float> out(field.width() * field.height());
    const double *src = field.data();
    std::transform(src, src + out.size(), out.begin(),
                   [](double v) { return static_cast<float>(v); });
    return out;
}

void fromFloatBuffer(const vector<float> &src, Field2D<double> &field) {
    double *dst = field.data();
    std::transform(src.begin(), src.end(), dst,
                   [](float v) { return static_cast<double>(v); });
}

void Simulation::update(double dt, bool printDebug) {
    const auto initial = chrono::high_resolution_clock::now();
    m_dt = dt;
    // applyGravityForce(dt);
    m_horizontalVelocity0 = m_horizontalVelocity;
    m_verticalVelocity0 = m_verticalVelocity;
    m_nextHorizontalVelocity = m_horizontalVelocity;
    m_nextVerticalVelocity = m_verticalVelocity;

    const int w = m_grid.width();
    const int h = m_grid.height();

    vector<uint8_t> solid(w * h);
    for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
        solid[y * w + x] = (m_cellType(x, y) == CellType::Solid);

    // THIS IS ESSENTIALLY COMMENTED OUT FOR NOW (RUNS ONLY TWICE)
    for (int i = 0; i < 2; i++) {
       m_nextHorizontalVelocity = m_horizontalVelocity;
       m_nextVerticalVelocity = m_verticalVelocity;
       applyViscosityJacobiIteration(dt);
       std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
       std::swap(m_nextVerticalVelocity, m_verticalVelocity);
       applyVelocityBoundary();
    }
    // END OF COMMENTED OUT SECTION

    const auto after_viscosity = chrono::high_resolution_clock::now();

    vector<float> cuda_horizontalVelocity = toFloatBuffer(m_horizontalVelocity);
    vector<float> cuda_verticalVelocity = toFloatBuffer(m_verticalVelocity);
    vector<float> cuda_nextHorizontalVelocity = cuda_horizontalVelocity;
    vector<float> cuda_nextVerticalVelocity = cuda_verticalVelocity;

    runVelocityAdvection(m_grid.width(), m_grid.height(), cuda_horizontalVelocity.data(), cuda_verticalVelocity.data(), 
    cuda_nextHorizontalVelocity.data(), cuda_nextVerticalVelocity.data(), solid.data(), m_grid.dx(), m_grid.dy(), dt);

    fromFloatBuffer(cuda_nextHorizontalVelocity, m_nextHorizontalVelocity);
    fromFloatBuffer(cuda_nextVerticalVelocity, m_nextVerticalVelocity);

    // applyAdvection(dt);
    std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
    std::swap(m_nextVerticalVelocity, m_verticalVelocity);

    // double error = 0;
    // for (int i = 0; i < m_horizontalVelocity.width(); i++) {
    //     for (int j = 0; j < m_horizontalVelocity.height(); j++) {
    //         error = max(error,abs(m_horizontalVelocity(i,j) - cuda_nextHorizontalVelocity(i,j)));
    //     }
    // }
    // cout << "CUDA Horizontal Advection Error: " << error << endl;

    // error = 0;
    // for (int i = 0; i < m_verticalVelocity.width(); i++) {
    //     for (int j = 0; j < m_verticalVelocity.height(); j++) {
    //         error = max(error,abs(m_verticalVelocity(i,j) - cuda_nextVerticalVelocity(i,j)));
    //     }
    // }
    // cout << "CUDA Vertical Advection Error: " << error << endl;

    initNextVelocity();

    const auto after_advection = chrono::high_resolution_clock::now();


    applyVelocityBoundary();

    compute_divergence();
    // cout << "before: " << find_max_divergence() << endl;   

    // initZeroPressure();
    

    const auto before_cuda_pressure = chrono::high_resolution_clock::now();

    vector<float> cuda_pressure = toFloatBuffer(m_pressure);
    vector<float> cuda_divergence = toFloatBuffer(m_divergence);

    runJacobiPressureSolve(m_grid.width(), m_grid.height(), cuda_pressure.data(), cuda_divergence.data(), solid.data(), 5000, m_grid.dx());

    fromFloatBuffer(cuda_pressure, m_pressure);

    const auto after_cuda_pressure = chrono::high_resolution_clock::now();

//    for (int i = 0; i < 1500; i++) {
//        applyPressureJacobiIteration();
//        std::swap(m_nextPressure, m_pressure);
//        applyPressureBoundary();
//    }
//    double error = 0;
//    for (int i = 0; i < m_pressure.width(); i++) {
//        for (int j = 0; j < m_pressure.height(); j++) {
//            error = max(error,abs(m_pressure(i,j) - cuda_pressure(i,j)));
//        }
//    }
//    cout << "CUDA Error: " << error << endl;

    applyPressureGradient();
    std::swap(m_nextHorizontalVelocity, m_horizontalVelocity);
    std::swap(m_nextVerticalVelocity, m_verticalVelocity);
    initNextVelocity();

    applyVelocityBoundary();

    const auto after_pressure = chrono::high_resolution_clock::now();
    compute_divergence();
    // cout << "after: " << find_max_divergence() << endl;   

    applyDyeAdvection(dt);
    std::swap(m_dye, m_nextDye);
    const auto after_dye = chrono::high_resolution_clock::now();

    if (printDebug) {
        cout << "Total: " << chrono::duration_cast<chrono::milliseconds>(after_dye - initial).count() << " ms, " 
             << "Viscosity: " << chrono::duration_cast<chrono::milliseconds>(after_viscosity - initial).count() << " ms, "
             << "Advection: " << chrono::duration_cast<chrono::milliseconds>(after_advection - after_viscosity).count() << " ms, "
             << "Pressure: " << chrono::duration_cast<chrono::milliseconds>(after_pressure - after_advection).count() << " ms, "
             << "Dye: " << chrono::duration_cast<chrono::milliseconds>(after_dye - after_pressure).count() << " ms,"
             << "CUDA Pressure: " << chrono::duration_cast<chrono::milliseconds>(after_cuda_pressure - before_cuda_pressure).count() << " ms" << endl;
    }
}

Vec2 Simulation::computeForce() const {
    Vec2 totalForce(0.0, 0.0);

    for (int y = 1; y < m_cellType.height() - 1; y++) {
        for (int x = 1; x < m_cellType.width() - 1; x++) {

            if (m_cellType(x, y) != CellType::Solid)
                continue;

            if (m_cellType(x - 1, y) != CellType::Solid) {
                double p = m_pressure(x - 1, y);
                totalForce.x += p * m_grid.dy();
            }

            if (m_cellType(x + 1, y) != CellType::Solid) {
                double p = m_pressure(x + 1, y);
                totalForce.x -= p * m_grid.dy();
            }

            if (m_cellType(x, y - 1) != CellType::Solid) {
                double p = m_pressure(x, y - 1);
                totalForce.y += p * m_grid.dx();
            }

            if (m_cellType(x, y + 1) != CellType::Solid) {
                double p = m_pressure(x, y + 1);
                totalForce.y -= p * m_grid.dx();
            }
        }
    }

    return totalForce * airDensity / m_dt;
}

void const Simulation::debugPrintFlow() const {
    double totalInflow = 0.0; 
    double totalOutflow = 0.0; 
    for (int i = 0; i < m_horizontalVelocity.height(); i++) {
        totalInflow += m_horizontalVelocity(0,i);
        totalOutflow += m_horizontalVelocity(m_horizontalVelocity.width()-1,i);
    }
    double netOutflowUp = 0.0; 
    double netOutflowDown = 0.0;
    for (int i = 0; i < m_verticalVelocity.width(); i++) {
        netOutflowUp -= m_verticalVelocity(i,0);
        netOutflowDown += m_verticalVelocity(i,m_verticalVelocity.height()-1);
    }
    cout << "in from left: " << totalInflow << endl;
    cout << "out from right: " << totalOutflow << endl;
    cout << "out from up: " << netOutflowUp << endl;
    cout << "out from down: " << netOutflowDown << endl;
};