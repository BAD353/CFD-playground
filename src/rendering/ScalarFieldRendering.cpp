#include <algorithm>
#include <rendering/ScalarFieldRendering.hpp>

sf::Color colorMap(double x) {
    x = std::clamp(x, 0.0, 1.0);

    sf::Color a, b;
    double t;

    if (x < 0.25) {
        // Dark blue -> light blue
        t = x / 0.25;

        a = sf::Color(0, 0, 180);
        b = sf::Color(0, 180, 255);
    } else if (x < 0.5) {
        // Light blue -> green
        t = (x - 0.25) / 0.25;

        a = sf::Color(0, 180, 255);
        b = sf::Color(0, 255, 0);
    } else if (x < 0.7) {
        // Green -> yellow
        t = (x - 0.5) / 0.2;

        a = sf::Color(0, 255, 0);
        b = sf::Color(255, 255, 0);
    } else if (x < 0.85) {
        // Yellow -> orange
        t = (x - 0.7) / 0.15;

        a = sf::Color(255, 255, 0);
        b = sf::Color(255, 140, 0);
    } else {
        // Orange -> red
        t = (x - 0.85) / 0.15;

        a = sf::Color(255, 140, 0);
        b = sf::Color(255, 0, 0);
    }

    return sf::Color(static_cast<uint8_t>(a.r + (b.r - a.r) * t), static_cast<uint8_t>(a.g + (b.g - a.g) * t),
                     static_cast<uint8_t>(a.b + (b.b - a.b) * t));
}

ScalarFieldRenderer::ScalarFieldRenderer(int cellSize, int width, int height)
    : m_cellSize(cellSize), m_pixels(width * height * 4), m_sprite(m_texture) {
    m_texture.resize({static_cast<unsigned>(width), static_cast<unsigned>(height)});

    m_sprite = sf::Sprite(m_texture);

    m_sprite.setTexture(m_texture);
    m_texture.setSmooth(false);
    m_sprite.setScale({m_cellSize, m_cellSize});
}

void ScalarFieldRenderer::drawDivergence(sf::RenderWindow &window, const Field2D<double> &field) {
    double maxAbsDivergence = max(abs(field.min()), abs(field.max()));
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double divergence = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 0;
            m_pixels[ind + 1] = 0;
            m_pixels[ind + 2] = 0;
            m_pixels[ind + 3] = 255;
            if (divergence > 0) {
                double value = std::clamp(divergence, 0.0, 1.0*maxAbsDivergence) * 255.0;
                m_pixels[ind + 0] = value;
            } else {
                double value = std::clamp(-divergence, 0.0, 1.0*maxAbsDivergence) * 255.0;
                m_pixels[ind + 2] = value;
            }
        }
    }
    cout << "Max abs divergence: " << maxAbsDivergence << endl;
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawDye(sf::RenderWindow &window, const Field2D<double> &field) {
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double dye = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 255;
            m_pixels[ind + 1] = 255;
            m_pixels[ind + 2] = 255;
            m_pixels[ind + 3] = dye * 255;
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawPressure(sf::RenderWindow &window, const Field2D<double> &field) {
    double maxPressure = field.max();
    double minPressure = field.min();

    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double pressure = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 0;
            m_pixels[ind + 1] = 0;
            m_pixels[ind + 2] = 0;
            m_pixels[ind + 3] = 255;
            if (pressure > 0) {
                double value = std::clamp(pressure / maxPressure, 0.0, 1.0) / 2 + 0.5;
                sf::Color color = colorMap(value);
                m_pixels[ind + 0] = color.r;
                m_pixels[ind + 1] = color.g;
                m_pixels[ind + 2] = color.b;
                m_pixels[ind + 3] = 255;
            } else {
                double value = 0.5 - std::clamp(-pressure / std::abs(minPressure), 0.0, 1.0) / 2;
                sf::Color color = colorMap(value);
                m_pixels[ind + 0] = color.r;
                m_pixels[ind + 1] = color.g;
                m_pixels[ind + 2] = color.b;
                m_pixels[ind + 3] = 255;
            }
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawCellType(sf::RenderWindow &window, const Field2D<CellType> &field) {
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            CellType CellType = field(x, y);
            int ind = (x + y * field.width()) * 4;
            if (CellType == CellType::Solid) {
                m_pixels[ind + 0] = 50;
                m_pixels[ind + 1] = 50;
                m_pixels[ind + 2] = 100;
                m_pixels[ind + 3] = 255;
            } else {
                m_pixels[ind + 0] = 0;
                m_pixels[ind + 1] = 0;
                m_pixels[ind + 2] = 0;
                m_pixels[ind + 3] = 0;
            }
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawVelocityMagnitude(sf::RenderWindow &window, const Field2D<double> &horizontalVelocity,
                                                const Field2D<double> &verticalVelocity) {
    double maxLength = 15;
    // for (int y = 0; y < horizontalVelocity.height(); y++) {
    //     for (int x = 0; x < verticalVelocity.width(); x++) {
    //         double u = horizontalVelocity(x, y);
    //         double v = verticalVelocity(x, y);
    //         double length = sqrt(u * u + v * v);
    //         maxLength = max(maxLength, length);
    //     }
    // }
    for (int y = 0; y < horizontalVelocity.height(); y++) {
        for (int x = 0; x < verticalVelocity.width(); x++) {
            double u = horizontalVelocity(x, y);
            double v = verticalVelocity(x, y);
            double length = sqrt(u * u + v * v);
            int ind = (x + y * verticalVelocity.width()) * 4;
            double value = std::clamp(length / maxLength, 0.0, 1.0);
            sf::Color color = colorMap(value);
            m_pixels[ind + 0] = color.r;
            m_pixels[ind + 1] = color.g;
            m_pixels[ind + 2] = color.b;
            m_pixels[ind + 3] = 255;
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}