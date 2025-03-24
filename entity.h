#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Entity
{
public:
    std::shared_ptr<sf::Sprite> sprite;
    std::vector<sf::FloatRect> hitboxes;

    Entity() = default;
    Entity(std::shared_ptr<sf::Sprite> sprite, std::vector<sf::FloatRect> &_hitboxes);
};