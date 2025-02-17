#pragma once
#include <SFML/Graphics.hpp>

static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * PI_f);
    const float b = sin(t + 0.66f * 2.0f * PI_f);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

static sf::Color getColorFromCell(int cell_x, int cell_y) 
{
    uint8_t r = (cell_x * 30) % 256;
    uint8_t g = (cell_y * 30) % 256;
    uint8_t b = ((cell_x + cell_y) * 15) % 256;
    return sf::Color(r, g, b);
}