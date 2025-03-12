#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    const float VERTICAL_SPEED = 750.0f;
    const sf::Vector2f ORIGINAL_SPEED = {-75.0f, 0.0f};

    sf::Vector2f speed = ORIGINAL_SPEED;
    sf::Vector2f originalPosition;
    bool isOnGround = false;

    bool needsPlayerToLeaveZone = false;

public:
    bool isActive = false;

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void updateEnemyRespawn(float deltaTime, const sf::FloatRect &gPlayerActivationZone, const sf::FloatRect &gPlayerDeactivationZone);
    void applyGravity(float deltaTime);
    void checkCollisions(const std::vector<sf::FloatRect> &boundsList);
    bool checkHitByEnemy(const sf::FloatRect simonBounds);
    void resetPosition();

    // Getter para la velocidad
    sf::Vector2f getSpeed() const { return speed; }
};