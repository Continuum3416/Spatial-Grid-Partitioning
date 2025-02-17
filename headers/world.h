#pragma once
#include "verlet_grid.h"
#include "../src/rainbow.h"




class PhysicsSolver {
public:
    std::vector<VerletBall> objects;
    Grid grid;
    sf::Vector2f world_size;
    uint32_t sub_steps = 8;

    PhysicsSolver(sf::Vector2i size)
        : grid(size.x, size.y, 10.f)
        , world_size(static_cast<float>(size.x), static_cast<float>(size.y))
        , sub_steps(1)
    {
        grid.clear();
    }

    void reserve(const int& res)
    {
        objects.reserve(res);
    }

    void setSubsSteps(const uint32_t& sub_steps)
    {
        this->sub_steps = sub_steps;
    }

    // add object to system from outside
    VerletBall& addObject(float radius, sf::Vector2f position, float speed, float angle)
    {
        objects.emplace_back(radius, position, speed, angle);
        return objects.back();
    }

    [[nodiscard]]
    size_t getObjectCount() const
    {
        return objects.size();
    }

    void update(float dt)
    {
        const float sub_dt = dt / sub_steps;
        for (uint16_t n{0}; n < sub_steps; ++n) 
        {
            updateObjects(sub_dt);
            handleBorderCollision(sf::Vector2i(50, 50), sf::Vector2i(1150, 1150));
            addObjectToGrid();
            resolveCollisions();
        }
    }

private:
    void checkCellCollision(Cell& current_cell, Cell& other_cell) {
        for (size_t i{0}; i < current_cell.ball_indices.size(); ++i) {
            VerletBall& ballA = objects[current_cell.ball_indices[i]];
            for (size_t j{i + 1}; j < other_cell.ball_indices.size(); ++j) {
                VerletBall& ballB = objects[other_cell.ball_indices[j]];

                const sf::Vector2f delta = ballB.position - ballA.position;
                const float dist2        = delta.x * delta.x + delta.y * delta.y;
                const float min_dist     = ballA.radius + ballB.radius;

                if (dist2 < min_dist * min_dist && dist2 > EPSILON) 
                {
                    const float response_coef = RESTITUTION;
                    const float dist          = std::sqrt(dist2);
                    const float overlap       = min_dist - dist;
                    const sf::Vector2f normal = delta / dist;

                    const float mass_ratioA = ballA.radius / min_dist;
                    const float mass_ratioB = ballB.radius / min_dist;

                    const sf::Vector2f correction = normal * response_coef * overlap;
                    ballA.position -= correction * mass_ratioB;
                    ballB.position += correction * mass_ratioA;
                }
            }
        }
    }

    void resolveCollisions() {
        for(uint32_t x{1}; x < grid.grid_width - 1; ++x) {
            for(uint32_t y{1}; y < grid.grid_height - 1; ++y) {
                auto& current_cell = grid.getCell(x, y);
                for(int dx = -1; dx <= 1; ++dx) {
                    for(int dy = -1; dy <= 1; ++dy) {
                        auto& other_cell = grid.getCell(x + dx, y + dy);
                        checkCellCollision(current_cell, other_cell);
                    }
                }
            }
        }
    }
    
    void updateObjects(float dt) {
        for(auto& obj: objects) {
            obj.updatePosition(dt);
        }
    }

    void addObjectToGrid() {
        grid.clear();
        for (size_t i = 0; i < objects.size(); ++i) {
            VerletBall& obj = objects[i];
            if (obj.position.x > obj.radius && obj.position.x < world_size.x - obj.radius &&
                obj.position.y > obj.radius && obj.position.y < world_size.y - obj.radius) 
            {
                // sf::Vector2i cell_coords = grid.getCellCoords(obj.position.x, obj.position.y);
                // obj.color = getColorFromCell(cell_coords.x, cell_coords.y);
                grid.addBall(i, obj);
            }
        }
    }

    void handleBorderCollision(const sf::Vector2i& top_left, const sf::Vector2i& bottom_right)
    {
        for(auto& ball : objects)
        {
            sf::Vector2f& position          = ball.position;
            sf::Vector2f& previous_position = ball.previous_position;
            sf::Vector2f  current_velocity  = position - previous_position;
            const float& radius             = ball.radius;
    
            // Handle wall collisions
            if (position.x + radius > bottom_right.x) {
                position.x = bottom_right.x - radius;
                previous_position.x = position.x + RESTITUTION * current_velocity.x;
            } else if (position.x - radius < top_left.x) {
                position.x = top_left.x + radius;
                previous_position.x = position.x + RESTITUTION * current_velocity.x;
            }
    
            // Handle floor collision
            if (position.y + radius > bottom_right.y) {
                position.y = bottom_right.y - radius;
                previous_position.y = position.y + RESTITUTION * current_velocity.y;
                previous_position.x += FRICTION_COEFFICIENT * current_velocity.x;
            } else if (position.y - radius < top_left.y) {
                position.y = top_left.y + radius;
                previous_position.y = position.y + RESTITUTION * current_velocity.y;
            }
        }
    }
};