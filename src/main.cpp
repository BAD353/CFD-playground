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
    constexpr int gridHeight = 200;
    constexpr double cellSize = 3.0f; // in pixels
    constexpr double dt = 0.001;

    Grid grid(gridWidth, gridHeight, 0.01, 0.01); // 1cm x 1cm cells

    Simulation simulation(grid);
    simulation.initializeVelocity();

    double chordWidthRatio = 0.6;

    double angleOfAttack = 0;
    constexpr double airfoilCenterX = 0.4 * gridWidth;
    constexpr double airfoilCenterY = 0.5 * gridHeight;

    Airfoil airfoil(0.02, 0.20, 0.12, gridWidth * chordWidthRatio, angleOfAttack,
                    {airfoilCenterX - gridWidth * chordWidthRatio / 2, airfoilCenterY}, gridWidth, gridHeight);

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
    bool isAoAIncreaseKeyPressed = false;
    bool isAoADecreaseKeyPressed = false;

    bool isPaused = false;
    bool isPauseKeyPressed = false;

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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                if (!isAoAIncreaseKeyPressed) {
                    angleOfAttack++;
                    airfoil.updateAngleOfAttack(angleOfAttack);
                    simulation.updateAllCells(airfoil.mesh());
                    cout << "Angle of Attack: " << angleOfAttack << endl;
                }
                isAoAIncreaseKeyPressed = true;
            } else {
                isAoAIncreaseKeyPressed = false;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                if (!isAoADecreaseKeyPressed) {
                    angleOfAttack--;
                    airfoil.updateAngleOfAttack(angleOfAttack);
                    simulation.updateAllCells(airfoil.mesh());
                }
                isAoADecreaseKeyPressed = true;
            } else {
                isAoADecreaseKeyPressed = false;
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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
                if (!isPauseKeyPressed) {
                    isPaused = !isPaused;
                }
                isPauseKeyPressed = true;
            } else {
                isPauseKeyPressed = false;
            }
        }
        if (!isPaused) {
            simulation.update(dt, frameCount % 10 == 0);
        }
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

        Vec2 force = simulation.computeForce();
        double forceAngle = abs(atan2(force.y, force.x)) * 180.0 / M_PI - angleOfAttack;
        double lift = force.y * cos(forceAngle * M_PI / 180.0) - force.x * sin(forceAngle * M_PI / 180.0);
        double drag = force.y * sin(forceAngle * M_PI / 180.0) + force.x * cos(forceAngle * M_PI / 180.0);

        drawArrow(window, {gridWidth * cellSize / 2, gridHeight * cellSize / 2}, {force.x, force.y}, 1.0);

        window.display();

        const auto final_time = chrono::high_resolution_clock::now();
        double total_time = chrono::duration<double>(final_time - initial_time).count();
        if (!isPaused && frameCount % 10 == 0) {
            cout << "FPS: " << 1.0 / total_time << endl;
            double simulation_time = chrono::duration<double>(after_simulation - initial_time).count();
            double render_time = chrono::duration<double>(final_time - after_simulation).count();
            cout << "Simulation time: " << simulation_time / total_time * 100 << "% of total time" << endl;
            cout << "Render time: " << render_time / total_time * 100 << "% of total time" << endl;

            cout << "Force: " << force << endl;
            cout << "Force: " << forceAngle << endl;
            cout << "Lift: " << lift << endl;
            cout << "Drag: " << drag << endl;
            cout << "Lift/Drag ratio: " << lift / drag << endl;
            simulation.debugPrintFlow();
        }
        frameCount++;
    }

    return 0;
}