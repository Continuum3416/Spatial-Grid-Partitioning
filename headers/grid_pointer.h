#pragma once
#include <cstdint>
#include <vector>
#include <list>
#include <cmath>
#include <memory>
#include <array>
#define HAVE_SFML
#include "../utils/math.h"
#include "../utils/constants.h"
#include "verlet.h"


struct Cell {
    std::vector<VerletBall*> balls;

    void addBall(VerletBall& ball) {
        balls.push_back(&ball);
    }

    void clear() {
        balls.clear();
    }

    [[nodiscard]] size_t getObjectCount() const {
        return balls.size();
    }
};


class Grid {
public:
    uint32_t window_width, window_height;
    uint32_t grid_width, grid_height;
    float cell_size;
    std::vector<Cell> cells;

    Grid(uint32_t w, uint32_t h, float cs = 25.f)
        : window_width(w), window_height(h), cell_size(cs) 
    {
        grid_width = static_cast<int>(std::ceil(w / cell_size));
        grid_height = static_cast<int>(std::ceil(h / cell_size));
        cells.resize(grid_width * grid_height);
    }

    void clear() {
        for (auto& cell : cells) {
            cell.clear();
        }
    }

    bool isValidCell(const uint32_t& cell_x, const uint32_t& cell_y) const {
        return cell_x >= 0 && cell_x < grid_width 
            && cell_y >= 0 && cell_y < grid_height;
    }

    sf::Vector2i getCellCoords(const float& x, const float& y) const {
        return { static_cast<int>(x / cell_size),
                 static_cast<int>(y / cell_size) };
    }

    Cell& getCell(const int& cell_x, const int& cell_y) {
        return cells[cell_y * grid_width + cell_x];
    }

    void addBall(VerletBall& ball) {
        sf::Vector2i cell_coords = getCellCoords(ball.position.x, ball.position.y);
        if (isValidCell(cell_coords.x, cell_coords.y)) {
            getCell(cell_coords.x, cell_coords.y).addBall(ball);
        }
    }

};


class PhysicsSolver {
    public:
        std::vector<VerletBall> objects;
        Grid grid;
        sf::Vector2f world_size;
        uint32_t sub_steps;
    
        PhysicsSolver(sf::Vector2i size)
            : grid(size.x, size.y)
            , world_size(static_cast<float>(size.x), static_cast<float>(size.y))
            , sub_steps(1)
        {
            grid.clear();
        }
    
        void update(float dt)
        {
            addObjectToGrid();
            updateObjects(dt);
            handleBorderCollision(sf::Vector2i(100, 100), sf::Vector2i(1100, 1100));
            resolveCollisions();
        }
    
        void resolvePairCollision(VerletBall& ballA, VerletBall& ballB)
        {
            if(&ballA == &ballB) return;
            const sf::Vector2f delta = ballB.position - ballA.position;
            const float dist2        = delta.x * delta.x + delta.y * delta.y;
            const float min_dist     = ballA.radius + ballB.radius;
    
            if (dist2 < min_dist * min_dist) {
                const float dist          = std::sqrt(dist2);
                const float overlap       = min_dist - dist;
                const sf::Vector2f normal = delta / dist;
    
                const float mass_ratioA = ballA.radius / min_dist;
                const float mass_ratioB = ballB.radius / min_dist;
    
                const sf::Vector2f correction = normal * RESTITUTION * overlap;
                ballA.position -= correction * mass_ratioB;
                ballB.position += correction * mass_ratioA;
            }
        }
    
        void checkCellCollision(Cell& cell_1, Cell& cell_2)
        {
            for(VerletBall*& obj_dx_1 : cell_1.balls) {
                for(VerletBall*& obj_dx_2 : cell_2.balls) {
                    if(obj_dx_1 != obj_dx_2) {
                        resolvePairCollision(*obj_dx_1, *obj_dx_2);
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
    
        
        // add object to system from outside
        VerletBall& addObject(float radius, sf::Vector2f position, float speed, float angle)
        {
            objects.emplace_back(radius, position, speed, angle);
            return objects.back();
        }
    
        void updateObjects(float dt) {
            for(auto& obj: objects) {
                obj.updatePosition(dt);
            }
        }
    
        void addObjectToGrid()
        {
            grid.clear();
            for(VerletBall& obj: objects) {
                if (obj.position.x > obj.radius && obj.position.x < world_size.x - obj.radius &&
                    obj.position.y > obj.radius && obj.position.y < world_size.y - obj.radius) 
                {
                    grid.addBall(obj);
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
    
        size_t getObjectCount()
        {
            return objects.size();
        }
    };