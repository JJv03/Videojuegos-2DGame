#pragma once

#include "entity.h"

class Enemy : public Entity
{
protected:
    bool isActive;
    float speed;
    sf::Vector2f initialPosition;

public:
    Enemy(const sf::FloatRect &_hitbox, sf::Sprite &_sprite, float _speed);
    virtual void update(float deltaTime, float cameraX);
    void checkActivation(float cameraX, float windowWidth);
    bool getActiveState() const;
    void activate();
    void deactivate();
};
