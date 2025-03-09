#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Entity
{
public:
    sf::Sprite *sprite;
    std::vector<sf::FloatRect> hitboxes;

    Entity() = default;
    Entity(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes);
};