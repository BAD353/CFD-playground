#include <algorithm>
#include <rendering/ScalarFieldRendering.hpp>

ScalarFieldRenderer::ScalarFieldRenderer(int cellSize, int width, int height)
    : m_cellSize(cellSize), m_pixels(width * height * 4), m_sprite(m_texture) {
    m_texture.resize({
        static_cast<unsigned>(width),
        static_cast<unsigned>(height)
    });

    m_sprite = sf::Sprite(m_texture);

    m_sprite.setTexture(m_texture);
    m_texture.setSmooth(false);
    m_sprite.setScale({m_cellSize, m_cellSize});
}

void ScalarFieldRenderer::drawDivergence(sf::RenderWindow &window,
                                         const Field2D<double> &field) {
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double divergence = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 0;
            m_pixels[ind + 1] = 0;
            m_pixels[ind + 2] = 0;
            m_pixels[ind + 3] = 255;
            if (divergence > 0) {
                double value = std::clamp(divergence, 0.0, 1.0) * 255.0;
                m_pixels[ind + 0] = value;
            } else {
                double value = std::clamp(-divergence, 0.0, 1.0) * 255.0;
                m_pixels[ind + 2] = value;
            }
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawDye(sf::RenderWindow &window,
                                  const Field2D<double> &field) {
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double dye = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 255;
            m_pixels[ind + 1] = 255;
            m_pixels[ind + 2] = 255;
            m_pixels[ind + 3] = dye*255;
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}

void ScalarFieldRenderer::drawPressure(sf::RenderWindow &window,
                                         const Field2D<double> &field) {
    for (int y = 0; y < field.height(); y++) {
        for (int x = 0; x < field.width(); x++) {
            double pressure = field(x, y);
            int ind = (x + y * field.width()) * 4;
            m_pixels[ind + 0] = 0;
            m_pixels[ind + 1] = 0;
            m_pixels[ind + 2] = 0;
            m_pixels[ind + 3] = 255;
            if (pressure > 0) {
                double value = std::clamp(pressure, 0.0, 1.0) * 255.0;
                m_pixels[ind + 0] = value;
            } else {
                double value = std::clamp(-pressure, 0.0, 1.0) * 255.0;
                m_pixels[ind + 2] = value;
            }
        }
    }
    m_texture.update(m_pixels.data());
    window.draw(m_sprite);
}