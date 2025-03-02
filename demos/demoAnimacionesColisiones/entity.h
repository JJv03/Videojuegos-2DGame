#pragma once

#include <SFML/Graphics.hpp>

class Entity
{
public:
    sf::Sprite* sprite;
    sf::FloatRect hitbox;

    Entity() = default;
    Entity(const sf::FloatRect& _hitbox, sf::Sprite& _sprite);
};