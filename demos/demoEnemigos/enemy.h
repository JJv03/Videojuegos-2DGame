#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    sf::Vector2f speed = {-75.0f, 0.0f};

public:
    sf::FloatRect activationZone;
    sf::FloatRect deactivationZone;
    sf::Vector2f originalPosition;

    bool isActive = false;
    bool playerWasNearActivation = false;

    Enemy() = default;
    Enemy(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone);

    void updateEnemyRespawn(float deltaTime, sf::Sprite *gSimonSprite);
    void update(float deltaTime);
    void resetPosition();
};