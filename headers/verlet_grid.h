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
    std::vector<uint32_t> ball_indices;

    Cell()
    {
        ball_indices.reserve(30);
    }

    void addBall(uint32_t index) {
        ball_indices.push_back(index);
    }

    void clear() {
        ball_indices.clear();
    }

    [[nodiscard]] size_t getObjectCount() const {
        return ball_indices.size();
    }
};

struct Grid {
    uint32_t window_width, window_height;
    uint32_t grid_width, grid_height;
    float cell_size;
    std::vector<Cell> cells;

    Grid(uint32_t w, uint32_t h, float cs = 25.f)
        : window_width(w), window_height(h), cell_size(cs) 
    {
        grid_width  = static_cast<uint32_t>(std::ceil(w / cell_size));
        grid_height = static_cast<uint32_t>(std::ceil(h / cell_size));
        cells.resize(grid_width * grid_height);
    }

    // Debug function
    size_t getTotalBallInGrid() const
    {
        size_t sum = 0;
        for(auto& cell : cells) {
            sum += cell.ball_indices.size();
        }
        return sum;
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

    // Input: ball position
    sf::Vector2i getCellCoords(const float& x, const float& y) const {
        return { static_cast<int>(x / cell_size),
                 static_cast<int>(y / cell_size) };
    }

    [[nodiscard]]
    float getCellSize()
    {
        return cell_size;
    }

    Cell& getCell(const int& cell_x, const int& cell_y) 
    {
        return cells[cell_y * grid_width + cell_x];
    }


    void addBall(uint32_t ball_idx, const VerletBall& ball) 
    {
        sf::Vector2i cell_coords = getCellCoords(ball.position.x, ball.position.y);
        getCell(cell_coords.x, cell_coords.y).addBall(ball_idx);
    }

    // void addBall(uint32_t ball_idx, const VerletBall& ball) 
    // {
    //     sf::Vector2i cell_coords = getCellCoords(ball.position.x, ball.position.y);
        
    //     // Compute bounds of the ball
    //     float radius = ball.radius;
    //     float left   = ball.position.x - radius;
    //     float right  = ball.position.x + radius;
    //     float top    = ball.position.y - radius;
    //     float bottom = ball.position.y + radius;

    //     // Compute the range of affected cells
    //     sf::Vector2i min_coords = getCellCoords(left, top);
    //     sf::Vector2i max_coords = getCellCoords(right, bottom);

    //     // Add ball to all intersecting cells
    //     for (int y = min_coords.y; y <= max_coords.y; ++y) {
    //         for (int x = min_coords.x; x <= max_coords.x; ++x) {
    //             if (isValidCell(x, y))
    //                 getCell(x, y).addBall(ball_idx);
    //         }
    //     }
    // }

};