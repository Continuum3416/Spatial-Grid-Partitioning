#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#define HAVE_SFML
#include "../utils/math.h"
#include "../utils/constants.h"

using namespace mathematical;
using namespace earth;

constexpr int32_t windowWidth        = 1200;
constexpr int32_t windowHeight       = 1200;
constexpr int32_t frameRate          = 120;
constexpr float SCALE                = 100.f;    // 1 meter = 100 pixels
constexpr float RESTITUTION          = 0.6f;     // Energy retention coefficient/response coefficient (0-1)
constexpr float FRICTION_COEFFICIENT = 0.1f;     // Friction coefficient for floor contact
constexpr float EPSILON              = 1e-4f;    // tolerance
constexpr float deltaTime = 1.f / static_cast<float>(frameRate);


struct VerletBall
{
    float radius                   = 10.f;
    sf::Vector2f position          = {0.f, 0.f};
    sf::Vector2f previous_position = {0.f, 0.f};
    sf::Vector2f velocity          = {0.f, 0.f};
    sf::Vector2f acceleration      = {0.f, 150.f};
    sf::Color color                = {sf::Color(0, 176, 255)};

    VerletBall(float radius, sf::Vector2f init_position, float init_speed, float angle)
        : radius(radius)
        , position(init_position)
        , velocity(std::cos(angle) * init_speed * SCALE, std::sin(angle) * init_speed * SCALE)
    {
        previous_position = position - velocity * deltaTime;
    }

    // x(n+1) = 2 * x(n) - x(n-1) + a * dt^2
    void updatePosition(float dt) 
    {
        const float DAMPING = 20.f;
        const sf::Vector2f last_update_move = position - previous_position;
        sf::Vector2f temp_position = position;
        position = 2.f * position - previous_position + (acceleration - last_update_move * DAMPING) * (dt * dt);
        previous_position = temp_position;
    }

    [[nodiscard]] 
    sf::Vector2f getVelocity(float dt) const 
    {
        return (position - previous_position) / dt;
    }

    [[nodiscard]] 
    float getSpeed(float dt) const 
    {
        sf::Vector2f v = getVelocity(dt);
        return utils::norm2f(v);
    }
};