#include <airfoil/Airfoil.hpp>
#include <cmath>

Airfoil::Airfoil(double maxCamber, double maxCamberPosition, double thickness, double chord, double angleOfAttack, Vec2 position,
                 int width, int height)
    : m_maxCamber(maxCamber), m_maxCamberPosition(maxCamberPosition), m_thickness(thickness), m_chord(chord),
      m_angleOfAttack(angleOfAttack), m_position(position), m_mesh(width, height) {
    m_mesh.fill(CellType::Fluid);
    createNormalizedPolygon();
    createWorldPolygon();
    createMesh();
}

Field2D<CellType> const &Airfoil::mesh() const {
    return m_mesh;
}

void Airfoil::createNormalizedPolygon() {
    int N = 100;
    double m = m_maxCamber;
    double p = m_maxCamberPosition;
    double t = m_thickness;
    std::vector<Vec2> upper;
    std::vector<Vec2> lower;
    for (int i = 0; i <= N; i++) {
        double x = static_cast<double>(i) / N;
        double yt = 5 * t * (0.2969 * std::sqrt(x) - 0.1260 * x - 0.3516 * x * x + 0.2843 * x * x * x - 0.1015 * x * x * x * x);
        double yc, dyc_dx;
        if (p == 0) {
            yc = 0;
            dyc_dx = 0;
        } else if (x < p) {
            yc = m / (p * p) * (2 * p * x - x * x);
            dyc_dx = 2 * m / (p * p) * (p - x);
        } else {
            yc = m / (1 - p) / (1 - p) * ((1 - 2 * p) + 2 * p * x - x * x);
            dyc_dx = 2 * m / (1 - p) / (1 - p) * (p - x);
        }
        double theta = atan(dyc_dx);
        upper.push_back({x - yt * sin(theta), yc + yt * cos(theta)});
        lower.push_back({x + yt * sin(theta), yc - yt * cos(theta)});
    }
    for (auto i : upper)
        m_normalizedPolygon.push_back(i);
    reverse(lower.begin(), lower.end());
    for (auto i : lower)
        m_normalizedPolygon.push_back(i);
}

Vec2 rotatePoint(Vec2 point, double angle) {
    return {
        point.x * cos(angle) - point.y * sin(angle),
        point.x * sin(angle) + point.y * cos(angle),
    };
}

bool pointInsidePolygon(Vec2 p, const std::vector<Vec2> &polygon) {
    bool inside = false;

    int n = polygon.size();

    for (int i = 0, j = n - 1; i < n; j = i++) {
        Vec2 a = polygon[i];
        Vec2 b = polygon[j];

        bool intersect = ((a.y > p.y) != (b.y > p.y)) && (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x);

        if (intersect)
            inside = !inside;
    }

    return inside;
}

void Airfoil::createWorldPolygon() {
    int N = m_normalizedPolygon.size();
    Vec2 pivot = {0.25, 0};
    m_worldPolygon.resize(N);
    for (int i = 0; i < N; i++) {
        Vec2 p = m_normalizedPolygon[i];
        
        p.y = -p.y; // flip y-axis

        p = p - pivot; // move to (0,0)

        p = p * m_chord; // scale

        p = rotatePoint(p, m_angleOfAttack * M_PI / 180.0);

        p = p + pivot * m_chord; // move back

        p = p + m_position;

        m_worldPolygon.push_back(p);
    }
}

void Airfoil::createMesh() {
    double minX = m_mesh.width();
    double maxX = 0;
    double minY = m_mesh.height();
    double maxY = 0;
    for (auto point : m_worldPolygon) {
        minX = min(minX, point.x);
        maxX = max(maxX, point.x);
        minY = min(minY, point.y);
        maxY = max(maxY, point.y);
    }
    int x0 = max(0, static_cast<int>(floor(minX)));
    int x1 = min(m_mesh.width() - 1, static_cast<int>(ceil(maxX)));
    int y0 = max(0, static_cast<int>(floor(minY)));
    int y1 = min(m_mesh.height() - 1, static_cast<int>(ceil(maxY)));
    for (int y = 0; y < m_mesh.height(); y++) {
        for (int x = 0; x < m_mesh.width(); x++) {
            m_mesh(x, y) = CellType::Fluid;
        }
    }
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            Vec2 p = {static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5};
            if (pointInsidePolygon(p, m_worldPolygon)) {
                m_mesh(x, y) = CellType::Solid;
            }
        }
    }
}