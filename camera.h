#pragma once
#include <SFML/Graphics.hpp>


class Camera
{
public:
    sf::Vector2f viewSize;
    sf::Vector2f startVertex;

    Camera(sf::Vector2f _viewSize);
    Camera(sf::FloatRect _viewRect);
    Camera(sf::Vector2f _startVertex, sf::Vector2f _viewSize);

    // Function that returns the adjusted view depending on <windowSize>
    sf::View getView(const sf::Vector2u& windowSize);
};