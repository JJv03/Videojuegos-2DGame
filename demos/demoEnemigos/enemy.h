#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
public:
    sf::FloatRect activationZone;

    bool isActive = false;
    bool playerWasNear = false;

    Enemy() = default;
    Enemy(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone);

    void update(float deltaTime, const std::vector<sf::FloatRect> &colliders);
    void resetPosition();
};