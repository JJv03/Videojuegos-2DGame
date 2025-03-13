#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    const float VERTICAL_SPEED = 750.0f;

    sf::Vector2f speed = {-75.0f, 0.0f};
    sf::Vector2f originalPosition;

    bool isOnGround = false;
    bool needsPlayerToLeaveZone = false;

public:
    bool isActive = false;

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void updateEnemyRespawn(float deltaTime, const sf::FloatRect &gPlayerActivationZone, const sf::FloatRect &gPlayerDeactivationZone);

    virtual void applyGravity(float deltaTime);
    virtual void checkCollisions(const std::vector<sf::FloatRect> &boundsList);
    virtual bool checkHitByEnemy(const sf::FloatRect simonBounds);
    virtual void resetPosition();
    virtual void update(float deltaTime) = 0;

    // Getter y setter para la velocidad
    sf::Vector2f getSpeed() const { return speed; }
    void setSpeed(const sf::Vector2f &newSpeed) { speed = newSpeed; }
};