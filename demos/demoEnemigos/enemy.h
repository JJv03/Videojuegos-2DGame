#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    const float VERTICAL_SPEED = 750.0f;
    const sf::Vector2f ORIGINAL_SPEED = {75.0f, 0.0f};

    sf::Vector2f speed = ORIGINAL_SPEED;
    bool isOnGround = false;
    sf::Vector2f originalPosition;

public:
    sf::FloatRect activationZone;
    sf::FloatRect deactivationZone;

    bool isActive = false;
    bool playerWasNearActivation = false;

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone, sf::FloatRect _deactivationZone);

    static Enemy createEnemy(const sf::Vector2f &position);

    void updateEnemyRespawn(float deltaTime, sf::Sprite *gSimonSprite);
    void update(float deltaTime);
    void applyGravity(float deltaTime);
    void checkCollisions(const std::vector<sf::FloatRect> &boundsList);
    bool checkHitByEnemy(const sf::FloatRect simonBounds);
    void resetPosition();
};