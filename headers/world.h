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
        : grid(size.x, size.y, 8.f)
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
        int margin = 50;
        const float sub_dt = dt / sub_steps;
        for (uint16_t n{0}; n < sub_steps; ++n) 
        {
            addObjectToGrid();
            updateObjects(sub_dt);
            handleBorderCollision(sf::Vector2i(margin, margin), sf::Vector2i(1200 - margin, 1200 - margin));
            resolveCollisions();
        }
    }

private:
    void checkCellCollision(uint32_t ball_idx, const Cell& c) 
    {
        for (uint32_t i{0}; i < c.getObjectCount(); ++i) {
            VerletBall& ballA = objects[ball_idx];
            VerletBall& ballB = objects[c.ball_indices[i]];
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

    void processCell(uint32_t index, const Cell& c) 
    {
        for(uint32_t i{0}; i < c.getObjectCount(); ++i) {
            const uint32_t ball_idx = c.ball_indices[i];
            checkCellCollision(ball_idx, grid.cells[index - 1]);
            checkCellCollision(ball_idx, grid.cells[index]);
            checkCellCollision(ball_idx, grid.cells[index + 1]);
            checkCellCollision(ball_idx, grid.cells[index + grid.grid_height - 1]);
            checkCellCollision(ball_idx, grid.cells[index + grid.grid_height    ]);
            checkCellCollision(ball_idx, grid.cells[index + grid.grid_height + 1]);
            checkCellCollision(ball_idx, grid.cells[index - grid.grid_height - 1]);
            checkCellCollision(ball_idx, grid.cells[index - grid.grid_height    ]);
            checkCellCollision(ball_idx, grid.cells[index - grid.grid_height + 1]);
        }
    }

    void resolveCollisions()
    {
        for (uint32_t idx{0}; idx < grid.cells.size(); ++idx) {
            processCell(idx, grid.cells[idx]);
        }
    }

    void updateObjects(float dt) 
    {
        for(auto& obj: objects) {
            obj.updatePosition(dt);
        }
    }

    void addObjectToGrid() 
    {
        grid.clear();
        for(uint32_t idx{0}; idx < objects.size(); ++idx) {
            VerletBall& obj = objects[idx];
            if (obj.position.x > obj.radius && obj.position.x < world_size.x - obj.radius &&
                obj.position.y > obj.radius && obj.position.y < world_size.y - obj.radius) 
            {
                // sf::Vector2i cell_coords = grid.getCellCoords(obj.position.x, obj.position.y);
                // obj.color = getColorFromCell(cell_coords.x, cell_coords.y);
                grid.addBall(idx, obj);
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
                //previous_position.x = position.x + RESTITUTION * current_velocity.x;
            } else if (position.x - radius < top_left.x) {
                position.x = top_left.x + radius;
                //previous_position.x = position.x + RESTITUTION * current_velocity.x;
            }
    
            // Handle floor collision
            if (position.y + radius > bottom_right.y) {
                position.y = bottom_right.y - radius;
                //previous_position.y = position.y + RESTITUTION * current_velocity.y;
                //previous_position.x += FRICTION_COEFFICIENT * current_velocity.x;
            } else if (position.y - radius < top_left.y) {
                position.y = top_left.y + radius;
                //previous_position.y = position.y + RESTITUTION * current_velocity.y;
            }
        }
    }
};