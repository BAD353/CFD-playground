#include <SFML/Graphics.hpp>

#include "mesh/Grid.hpp"
#include "rendering/ScalarFieldRendering.hpp"
#include "rendering/VelocityRendering.hpp"
#include "simulation/Simulation.hpp"
#include <chrono>
using namespace std;

#define NONE_RENDERING 0
#define VELOCITY_RENDERING 1
#define DIVERGENCE_RENDERING 2
#define PRESSURE_RENDERING 3

int main() {
    constexpr int gridWidth = 100;
    constexpr int gridHeight = 100;
    constexpr double cellSize = 6.0f;
    constexpr double dt = 0.01;

    Grid grid(gridWidth, gridHeight, 1.0, 1.0);

    Simulation simulation(grid);
    simulation.initializeVelocity();

    VelocityRenderer vectorRenderer(cellSize);
    ScalarFieldRenderer scalarRenderer(cellSize, gridWidth, gridHeight);

    sf::RenderWindow window(
        sf::VideoMode({gridWidth * static_cast<unsigned int>(cellSize),
                       gridHeight * static_cast<unsigned int>(cellSize)}),
        "MyCFD");

    int step_count = 0;

    sf::Vector2i previousMousePos;
    bool previousMouseValid = false;

    int renderMode = VELOCITY_RENDERING;
    bool isRenderModeKeyPressed = false;

    int frameCount = 0;

    while (window.isOpen()) {
        const auto initial_time = chrono::high_resolution_clock::now();
        
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (previousMouseValid) {
                    sf::Vector2f mouseVelocity =
                        static_cast<sf::Vector2f>(mousePos - previousMousePos);

                    constexpr float brushStrength = 0.5f;

                    Vec2 force{mouseVelocity.x * brushStrength,
                               mouseVelocity.y * brushStrength};

                    Vec2 position{static_cast<double>(mousePos.x) / cellSize,
                                  static_cast<double>(mousePos.y) / cellSize};

                    simulation.applyMouseForce(position, force, 5.0, dt);
                }

                previousMousePos = mousePos;
                previousMouseValid = true;
            } else {
                previousMouseValid = false;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                Vec2 position{static_cast<double>(mousePos.x) / cellSize,
                              static_cast<double>(mousePos.y) / cellSize};
                simulation.injectDye(position, 5.0, dt);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) {
                if (!isRenderModeKeyPressed) {
                    renderMode = (renderMode + 1) % 4;
                }
                isRenderModeKeyPressed = true;
            }
            else {
                isRenderModeKeyPressed = false;
            }
        }
        simulation.update(dt);
        const auto after_simulation = chrono::high_resolution_clock::now();

        step_count++;

        window.clear();

        if (renderMode == VELOCITY_RENDERING) {
            vectorRenderer.draw(window, simulation.velocity());
        } else if (renderMode == DIVERGENCE_RENDERING) {
            scalarRenderer.drawDivergence(window, simulation.divergence());
        } else if (renderMode == PRESSURE_RENDERING) {
            scalarRenderer.drawPressure(window, simulation.pressure());
        } else if (renderMode == NONE_RENDERING) {
            //pass
        }
        scalarRenderer.drawDye(window, simulation.dye());
        window.display();

        const auto final_time = chrono::high_resolution_clock::now();
        double total_time = chrono::duration<double>(final_time - initial_time).count();
        cout << "FPS: " << 1.0 / total_time << endl;
        double simulation_time = chrono::duration<double>(after_simulation - initial_time).count();
        double render_time = chrono::duration<double>(final_time - after_simulation).count();
        cout << "Simulation time: " << simulation_time / total_time * 100 << "% of total time" << endl;
        cout << "Render time: " << render_time / total_time * 100 << "% of total time" << endl;
    }

    return 0;
}