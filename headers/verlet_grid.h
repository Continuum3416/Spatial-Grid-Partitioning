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
    std::vector<size_t> ball_indices;

    void addBall(size_t index) {
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
        grid_width  = static_cast<int>(std::ceil(w / cell_size));
        grid_height = static_cast<int>(std::ceil(h / cell_size));
        cells.resize(grid_width * grid_height);
    }

    // Debug function
    size_t getTotalBallInGrid()
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

    void addBall(size_t ball_idx, const VerletBall& ball) {
        sf::Vector2i cell_coords = getCellCoords(ball.position.x, ball.position.y);
        if (isValidCell(cell_coords.x, cell_coords.y)) {
            getCell(cell_coords.x, cell_coords.y).addBall(ball_idx);
        }
    }
};