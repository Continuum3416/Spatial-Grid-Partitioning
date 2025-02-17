#pragma once

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include "../headers/verlet.h"


class EventHandler {
private:
    sf::RenderWindow& window;
    
public:
    EventHandler(sf::RenderWindow& window) : window(window)
    {
        trajectoryLine.setPrimitiveType(sf::Lines);
    };

    sf::VertexArray trajectoryLine;
    sf::ConvexShape arrowhead;

    template <typename T>
    void dragAndShoot(const sf::Event& event, PhysicsSolver& solver) {
        static bool dragging = false;
        static sf::Vector2f initial_position;
        static sf::Vector2f target_position;
        static sf::Color color = sf::Color::Red;

        // Start dragging when the mouse is pressed
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            initial_position = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            target_position = initial_position; // initialize
            dragging = true;
            trajectoryLine.clear();  // Clear the line when a new drag starts
            trajectoryLine.setPrimitiveType(sf::Lines);
        }

        // Update the target position when dragging
        if (dragging && event.type == sf::Event::MouseMoved) {
            target_position = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

            // Draw trajectory line
            trajectoryLine.clear();
            trajectoryLine.append(sf::Vertex(initial_position, color));
            trajectoryLine.append(sf::Vertex(target_position, color));

            // Arrowhead direction
            sf::Vector2f direction =  initial_position - target_position;
            float angle = std::atan2(direction.y, direction.x) * 180.f / PI_f;

            // Create arrowhead
            arrowhead.setPointCount(3); // Triangle shape
            arrowhead.setPoint(0, sf::Vector2f(0.f, 0.f));
            arrowhead.setPoint(1, sf::Vector2f(-10.f, -5.f)); // Left side
            arrowhead.setPoint(2, sf::Vector2f(-10.f, 5.f));  // Right side
            arrowhead.setFillColor(color);

            // arrowhead positioning
            arrowhead.setPosition(initial_position);
            arrowhead.setRotation(angle);
        } 

        // Release the mouse
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            if (dragging) {
                dragging = false;
                trajectoryLine.clear();  // Remove the line when released
                sf::Vector2f direction = initial_position - target_position;
                float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                float speed = magnitude / 20.f;
                float angle = std::atan2(direction.y, direction.x);
                solver.addObject(4.f, target_position, speed, angle);
            }
            // Clear the arrow
            arrowhead.setPointCount(0); 
        }
    }

    void closeWindow(const sf::Event& event){
        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }
    }

    bool isPaused = false;
    void pauseAnimation() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            isPaused = !isPaused;
            sf::sleep(sf::seconds(20.f));
        }
    }
};