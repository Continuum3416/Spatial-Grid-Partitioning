#include <SFML/Graphics.hpp>
#include <iostream>
#define HAVE_SFML
#include "../utils/random.h"
#include "../headers/world.h"
#include "renderer.h"
#include "rainbow.h"
#include "event.h"


int main(int argc, char* argv[]) 
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Spatial Partitioning C++", sf::Style::Default, settings);
    window.setFramerateLimit(frameRate);
    sf::Font font;
    font.loadFromFile("fonts/cmunrm.ttf");
    utils::Random randomizer;
    Renderer renderer(window);
    PhysicsSolver solver(sf::Vector2i(windowWidth, windowHeight));
    EventHandler handle_event(window);
    Information information(window, font);
    
    // Initialize ball settings
    const float spawn_delay           = 0.0001f;
    const float initial_speed         = 2.f;              // Ball speed in m/s
    const sf::Vector2f spawn_position = {500.f, 250.f};
    const uint32_t max_balls          = 75000;
    solver.reserve(max_balls);

    // Clocks
    sf::Clock ball_clock, total_time_clock;

    /// Instant ball generation to save time
    bool instant_generation = true;
    if(instant_generation) {
        std::vector<VerletBall> balls;
        balls.reserve(max_balls);
        for (uint32_t i = 0; i < max_balls; ++i) 
        {
            float x = randomizer.generateRandomFloat(50, windowWidth - 50);
            float y = randomizer.generateRandomFloat(50, windowHeight - 50);
            float angle = randomizer.generateRandomFloat(0, 2 * PI_f);  // Random movement direction
            const float radius           = 1.f;
            sf::Color random_color = getRainbow(static_cast<float>(i));
    
            auto& obj = solver.addObject(radius, {x, y}, initial_speed, angle);
            obj.color = random_color;
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            handle_event.closeWindow(event);
        }
        //handle_event.dragAndShoot<VerletBall>(event, solver);

        if(!instant_generation){
            if (solver.getObjectCount() < max_balls && ball_clock.getElapsedTime().asSeconds() >= spawn_delay)
            {
                ball_clock.restart();
                const float t                = total_time_clock.getElapsedTime().asSeconds();
                const float radius           = 2.f;
                const float angle            = -45.f * (PI_f) / 180;
                const sf::Color random_color = getRainbow(t);
                auto& obj = solver.addObject(radius, spawn_position, initial_speed, angle);
                obj.color = random_color;
            }
        }

        window.clear(sf::Color::Black);
        solver.update(deltaTime);
        renderer.renderTriangle(solver);
        //renderer.renderDragArrow(handle_event);
        information.displayInformation(total_time_clock, solver);
        window.display();
    }

    return 0;
}

// Build & Run:
// cmake --build .\build\ --config Debug; .\build\Debug\grid.exe
// cmake --build .\build\ --config Release; .\build\Release\grid.exe