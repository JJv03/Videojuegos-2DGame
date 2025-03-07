#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Entity
{
public:
    sf::Sprite *sprite;
    std::vector<sf::FloatRect> hitboxes;

    Entity() = default;
    Entity(const std::vector<sf::FloatRect> &_hitboxes, sf::Sprite &_sprite);
};