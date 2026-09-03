#include <SFML/Graphics.hpp>

#include "airfoil/Airfoil.hpp"
#include "mesh/Grid.hpp"
#include "rendering/ScalarFieldRendering.hpp"
#include "rendering/VelocityRendering.hpp"
#include "simulation/Simulation.hpp"

#include <chrono>
using namespace std;

enum class RenderMode {
    NoneRendering,
    VelocityRendering,
    DivergenceRendering,
    PressureRendering,
    VelocityMagintudeRendering,
    Count
};

enum class DrawingMode { VelocityDye, Obstacle, Count };

int main() {
    constexpr int gridWidth = 400;
    constexpr int gridHeight = 300;
    constexpr double cellSize = 2.0f;
    constexpr double dt = 0.02;

    Grid grid(gridWidth, gridHeight, 1.0, 1.0);

    Simulation simulation(grid);
    simulation.initializeVelocity();

    double chordWidthRatio = 0.7;

    Airfoil airfoil(0.02, 0.40, 0.12, gridWidth * chordWidthRatio, 0,
                    {gridWidth / 2.0 - gridWidth * chordWidthRatio / 2, gridHeight / 2.0}, gridWidth, gridHeight);

    simulation.updateAllCells(airfoil.mesh());
    // int size = 20;
    // for (int x = -size; x <= size; x++) {
    //     int sz = size - abs(x);
    //     for (int y = -sz; y <= sz; y++) {
    //         simulation.updateCellType({gridWidth * 0.3 + x, gridHeight / 2.0 + y}, CellType::Solid, 1);
    //     }
    // }

    VelocityRenderer vectorRenderer(cellSize);
    ScalarFieldRenderer scalarRenderer(cellSize, gridWidth, gridHeight);

    sf::RenderWindow window(
        sf::VideoMode({gridWidth * static_cast<unsigned int>(cellSize), gridHeight * static_cast<unsigned int>(cellSize)}),
        "MyCFD");

    int step_count = 0;

    sf::Vector2i previousMousePos;
    bool previousMouseValid = false;

    RenderMode renderMode = RenderMode::VelocityRendering;
    bool isRenderModeKeyPressed = false;

    DrawingMode drawingMode = DrawingMode::VelocityDye;
    bool isDrawingModeKeyPressed = false;

    int frameCount = 0;

    int radius = 5.0;
    bool isRadiusIncreaseKeyPressed = false;
    bool isRadiusDecreaseKeyPressed = false;

    while (window.isOpen()) {
        const auto initial_time = chrono::high_resolution_clock::now();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (drawingMode == DrawingMode::VelocityDye) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (previousMouseValid) {
                        sf::Vector2f mouseVelocity = static_cast<sf::Vector2f>(mousePos - previousMousePos);

                        constexpr float brushStrength = 0.5f;

                        Vec2 force{mouseVelocity.x * brushStrength, mouseVelocity.y * brushStrength};

                        Vec2 position{static_cast<double>(mousePos.x) / cellSize, static_cast<double>(mousePos.y) / cellSize};

                        simulation.applyMouseForce(position, force, radius, dt);
                    }

                    previousMousePos = mousePos;
                    previousMouseValid = true;
                } else {
                    previousMouseValid = false;
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    Vec2 position{static_cast<double>(mousePos.x) / cellSize, static_cast<double>(mousePos.y) / cellSize};
                    simulation.injectDye(position, radius, dt);
                }
            } else if (drawingMode == DrawingMode::Obstacle) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    Vec2 position{static_cast<double>(mousePos.x) / cellSize, static_cast<double>(mousePos.y) / cellSize};
                    simulation.updateCellType(position, CellType::Solid, radius);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    Vec2 position{static_cast<double>(mousePos.x) / cellSize, static_cast<double>(mousePos.y) / cellSize};
                    simulation.updateCellType(position, CellType::Fluid, radius);
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add)) {
                if (!isRadiusIncreaseKeyPressed)
                    radius++;
                isRadiusIncreaseKeyPressed = true;
            } else {
                isRadiusIncreaseKeyPressed = false;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract)) {
                if (!isRadiusDecreaseKeyPressed && radius > 1)
                    radius--;
                isRadiusDecreaseKeyPressed = true;
            } else {
                isRadiusDecreaseKeyPressed = false;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) {
                if (!isRenderModeKeyPressed) {
                    renderMode =
                        static_cast<RenderMode>((static_cast<int>(renderMode) + 1) % static_cast<int>(RenderMode::Count));
                }
                isRenderModeKeyPressed = true;
            } else {
                isRenderModeKeyPressed = false;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M)) {
                if (!isDrawingModeKeyPressed) {
                    drawingMode =
                        static_cast<DrawingMode>((static_cast<int>(drawingMode) + 1) % static_cast<int>(DrawingMode::Count));
                }
                isDrawingModeKeyPressed = true;
            } else {
                isDrawingModeKeyPressed = false;
            }
        }
        simulation.update(dt);
        const auto after_simulation = chrono::high_resolution_clock::now();

        step_count++;

        window.clear();

        if (renderMode == RenderMode::VelocityRendering) {
            vectorRenderer.draw(window, simulation.horizontalVelocity(), simulation.verticalVelocity());
        } else if (renderMode == RenderMode::DivergenceRendering) {
            scalarRenderer.drawDivergence(window, simulation.divergence());
        } else if (renderMode == RenderMode::PressureRendering) {
            scalarRenderer.drawPressure(window, simulation.pressure());
        } else if (renderMode == RenderMode::VelocityMagintudeRendering) {
            scalarRenderer.drawVelocityMagnitude(window, simulation.horizontalVelocity(), simulation.verticalVelocity());
        } else if (renderMode == RenderMode::NoneRendering) {
            // pass
        }
        scalarRenderer.drawDye(window, simulation.dye());
        scalarRenderer.drawCellType(window, simulation.cellType());
        window.display();

        const auto final_time = chrono::high_resolution_clock::now();
        double total_time = chrono::duration<double>(final_time - initial_time).count();
        if (frameCount % 10 == 0) {
            cout << "FPS: " << 1.0 / total_time << endl;
            double simulation_time = chrono::duration<double>(after_simulation - initial_time).count();
            double render_time = chrono::duration<double>(final_time - after_simulation).count();
            cout << "Simulation time: " << simulation_time / total_time * 100 << "% of total time" << endl;
            cout << "Render time: " << render_time / total_time * 100 << "% of total time" << endl;
        }
        frameCount++;
    }

    return 0;
}