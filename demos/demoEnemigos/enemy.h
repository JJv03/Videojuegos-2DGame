#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    const float VERTICAL_SPEED = 750.0f;
    const sf::Vector2f ORIGINAL_SPEED = {-75.0f, 0.0f};

    sf::Vector2f speed = {-75.0f, 0.0f};
    bool isOnGround = false;

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
    void applyGravity(float deltaTime);
    void checkCollisions(const std::vector<sf::FloatRect> &boundsList);
    void resetPosition();
};