#pragma once
#include <SFML/Graphics.hpp>
#include "../headers/verlet.h"
#include "event.h"
#include <charconv>

class Renderer
{
private:
    sf::RenderTarget& render;
public:
    Renderer(sf::RenderTarget& render) 
        : render(render)
    {}

    void renderBalls(const PhysicsSolver& solver) const
    {
        const auto& objects = solver.objects;
        sf::CircleShape circle{1.0f};
        for (const auto& obj : objects)
        {
            circle.setRadius(obj.radius);
            circle.setOrigin(obj.radius, obj.radius);
            circle.setFillColor(obj.color);
            circle.setPosition(obj.position);
            render.draw(circle);
        }
    }

    void renderTriangle(const PhysicsSolver& solver) const
    {
        const auto& objects = solver.objects;
        sf::VertexArray vertices(sf::Triangles);

        for (const auto& obj : objects)
        {
            const int triangleCount = 3; // Approximate a circle with triangles
            float angleStep = 2 * PI_f / triangleCount;

            for (int i = 0; i < triangleCount; ++i)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;

                sf::Vector2f center = obj.position;
                sf::Vector2f point1 = center + sf::Vector2f(std::cos(angle1), std::sin(angle1)) * obj.radius;
                sf::Vector2f point2 = center + sf::Vector2f(std::cos(angle2), std::sin(angle2)) * obj.radius;

                sf::Color color = obj.color;

                // Add triangle (center, point1, point2)
                vertices.append(sf::Vertex(center, color));
                vertices.append(sf::Vertex(point1, color));
                vertices.append(sf::Vertex(point2, color));
            }
        }

        render.draw(vertices);
    }

    void renderPoints(const PhysicsSolver& solver) const
    {
        const auto& objects = solver.objects;
        sf::VertexArray vertices(sf::Points);

        for (const auto& obj : objects)
        {
            sf::Vector2f center = obj.position;
            sf::Color color = obj.color;
            vertices.append(sf::Vertex(center, color));
        }

        render.draw(vertices);
    }


    void renderDragArrow(const EventHandler& event) 
    {
        render.draw(event.trajectoryLine);
        render.draw(event.arrowhead);
    }
};


class Information
{
private:
    sf::RenderWindow& window;
    sf::Font& font;
    uint16_t font_size = 25;
    static const uint32_t max_string_size = 32;
    mutable sf::Clock fps_clock;

    [[nodiscard]]
    std::string getFPS() const
    {
        float time_per_frame = fps_clock.restart().asSeconds();
        float fps = 1.0f / time_per_frame;
        char buffer[max_string_size];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), static_cast<int>(fps));
        return std::string(buffer, ptr) + " FPS";
    }

    [[nodiscard]]
    std::string getObjectCount(const PhysicsSolver& solver) const
    {
        char buffer[max_string_size];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), solver.getObjectCount());
        return std::string(buffer, ptr) + " objects";
    }

    [[nodiscard]]
    std::string getElapsedTime(sf::Clock& total_time_clock) const
    { 
        float elapsed = total_time_clock.getElapsedTime().asSeconds();
        char buffer[max_string_size];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), elapsed, std::chars_format::fixed, 2);
        return std::string(buffer, ptr) + " sec";
    }

public:
    Information(sf::RenderWindow& window, sf::Font& font)
        : window(window)
        , font(font) 
    {}

    void setFontSize(uint16_t font_size)
    {
        this->font_size = font_size;
    }

    void displayInformation(sf::Clock& total_time_clock, const PhysicsSolver& solver) const
    {
        static sf::Text information_text("", font, font_size);

        std::string FPS = getFPS();
        std::string object_count = getObjectCount(solver);
        std::string formattedTime = getElapsedTime(total_time_clock);

        information_text.setString(FPS + "  " + object_count + "  " + formattedTime);
        window.draw(information_text);
    }
};