#pragma once

#include "enemy.h"

class Zombie : public Enemy
{

public:
    Zombie(const sf::FloatRect &_hitbox, sf::Sprite &_sprite, float _speed);
    void update(float deltaTime, float cameraX) override;
};
