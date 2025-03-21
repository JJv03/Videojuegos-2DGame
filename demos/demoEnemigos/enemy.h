#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"

class Enemy : public Entity
{
private:
    const float VERTICAL_SPEED = 750.0f;

    sf::Vector2f originalPosition;

public:
    bool isActive = false;
    bool isOnGround = false;
    bool needsPlayerToLeaveZone = false;

    sf::Vector2f speed = {-75.0f, 0.0f};

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void draw(sf::RenderWindow &window, bool debugDraw);

    virtual void applyGravity(float deltaTime);
    virtual bool checkHitByEnemy(const sf::FloatRect simonBounds);
    virtual void resetPosition();
    virtual void update(float deltaTime) = 0;
};