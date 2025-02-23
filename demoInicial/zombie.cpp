#include "zombie.h"
#include <cmath>
#include <iostream>

Zombie::Zombie(const sf::FloatRect &_hitbox, sf::Sprite &_sprite, float _speed)
    : Enemy(_hitbox, _sprite, _speed) {}

// Esto debera ser con el jugador pero solo tengo camara
void Zombie::update(float deltaTime, float cameraX)
{
    if (isActive)
    {
        Enemy::update(deltaTime, cameraX);
    }
}
