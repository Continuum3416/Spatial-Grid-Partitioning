#pragma once

#include<cmath>

#ifdef HAVE_SFML
#include <SFML/Graphics.hpp>

namespace utils{

float norm2f(const sf::Vector2f& vector){
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f normalize(const sf::Vector2f& vector) {
    float magnitude = norm2f(vector);
    return (magnitude > 0) ? vector / magnitude : sf::Vector2f(0.f, 0.f);
}

float dot(const sf::Vector2f& vector1, const sf::Vector2f& vector2){
    return vector1.x * vector2.x + vector1.y * vector2.y;
}

sf::Vector2f proj(const sf::Vector2f& A, const sf::Vector2f& B) {
    sf::Vector2f result;
    float dotProduct = dot(A, B);
    float magnitudeSquaredB = dot(B, B);
    if (magnitudeSquaredB != 0) {
        result = (dotProduct / magnitudeSquaredB) * B;
    } else {
        result = sf::Vector2f(0.f, 0.f); // Handle the case where B is a zero vector
    }
    return result;
}

}


#endif

