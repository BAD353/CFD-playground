#include <algorithm>
#include <cmath>
#include <iostream>
#include "rendering/VelocityRendering.hpp"
#include "math/Vec2.hpp"
using namespace std;

namespace
{
    constexpr float COS30 = 0.8660254037844386f;
    constexpr float SIN30 = 0.5f;

    sf::Color hsvToRgb(double h, double s, double v)
    {
        h = std::fmod(h, 360.0);
        if (h < 0)
            h += 360.0;

        double c = v * s;
        double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
        double m = v - c;

        double r, g, b;

        if (h < 60)
        {
            r = c;
            g = x;
            b = 0;
        }
        else if (h < 120)
        {
            r = x;
            g = c;
            b = 0;
        }
        else if (h < 180)
        {
            r = 0;
            g = c;
            b = x;
        }
        else if (h < 240)
        {
            r = 0;
            g = x;
            b = c;
        }
        else if (h < 300)
        {
            r = x;
            g = 0;
            b = c;
        }
        else
        {
            r = c;
            g = 0;
            b = x;
        }

        return sf::Color(
            static_cast<uint8_t>((r + m) * 255),
            static_cast<uint8_t>((g + m) * 255),
            static_cast<uint8_t>((b + m) * 255));
    }

    sf::Vector2f rotate30(const sf::Vector2f &v)
    {
        return {
            v.x * COS30 - v.y * SIN30,
            v.x * SIN30 + v.y * COS30};
    }

    sf::Vector2f rotateMinus30(const sf::Vector2f &v)
    {
        return {
            v.x * COS30 + v.y * SIN30,
            -v.x * SIN30 + v.y * COS30};
    }

    void drawArrow(sf::RenderWindow &window, sf::Vector2f center, sf::Vector2f direction)
    {

        sf::Color arrowColor = hsvToRgb(std::atan2(direction.y, direction.x) * 180.0 / M_PI, 1.0, 1.0);
        const float arrowScale = 0.5f;
        std::array line =
            {
                sf::Vertex(center - direction * 0.5f, arrowColor),
                sf::Vertex(center + direction * 0.5f, arrowColor)};

        std::array leftline =
            {
                sf::Vertex(center + direction * 0.5f - rotate30(direction) * arrowScale, arrowColor),
                sf::Vertex(center + direction * 0.5f, arrowColor)};

        std::array rightline =
            {
                sf::Vertex(center + direction * 0.5f - rotateMinus30(direction) * arrowScale, arrowColor),
                sf::Vertex(center + direction * 0.5f, arrowColor)};

        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        window.draw(leftline.data(), leftline.size(), sf::PrimitiveType::Lines);
        window.draw(rightline.data(), rightline.size(), sf::PrimitiveType::Lines);
    }
}

VelocityRenderer::VelocityRenderer(double cellSize) : m_cellSize(cellSize) {}

void VelocityRenderer::draw(sf::RenderWindow &window, const Field2D<Vec2> &field)
{
    const int step = 2;
    const double arrow_max_length = m_cellSize * 0.8;
    for (int y = 0; y < field.height(); y += step)
    {
        for (int x = 0; x < field.width(); x += step)
        {
            Vec2 velocity = field(x, y);
            sf::Vector2f center({x * m_cellSize + m_cellSize / 2, y * m_cellSize + m_cellSize / 2});
            sf::Vector2f direction(std::clamp(velocity.x / m_maxArrowVelocity, (double)-1, (double)1) * arrow_max_length, std::clamp(velocity.y / m_maxArrowVelocity, (double)-1, (double)1) * arrow_max_length);
            drawArrow(window, center, direction);
        }
    }
}