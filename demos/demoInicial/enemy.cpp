#include "enemy.h"
#include <cmath>
#include <iostream>

Enemy::Enemy(const sf::FloatRect &_hitbox, sf::Sprite &_sprite, float _speed)
    : Entity(_hitbox, _sprite), speed(_speed), isActive(false), initialPosition(_sprite.getPosition()) {}

void Enemy::update(float deltaTime, float cameraX)
{
    if (isActive)
    {
        if (sprite)
        {
            sf::Vector2f targetPosition(cameraX, sprite->getPosition().y);

            sf::Vector2f direction = targetPosition - sprite->getPosition();

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length != 0)
            {
                direction /= length;
            }
            else
            {
                direction = sf::Vector2f(0.f, 0.f);
            }

            sprite->move(direction * speed * deltaTime);

            std::cout << "Enemy Position: (" << sprite->getPosition().x << ", " << sprite->getPosition().y << ")\n";
            std::cout << "Direction: (" << direction.x << ", " << direction.y << ")\n";
            std::cout << "Length: " << length << "\n";
            std::cout << "Speed: " << speed << "\n";
        }
        else
        {
            std::cerr << "Error: sprite es nulo en Enemy::update\n";
        }
    }
}

// Esto debera ser con el jugador pero solo tengo camara
void Enemy::checkActivation(float cameraX, float windowWidth)
{
    float cameraLeft = cameraX - windowWidth / 2.0f;
    float cameraRight = cameraX + windowWidth / 2.0f;

    float enemyX = sprite->getPosition().x;

    std::cout << "Camera X: " << cameraX << ", Camera Left: " << cameraLeft << ", Camera Right: " << cameraRight << std::endl;
    std::cout << "Enemy X: " << enemyX << std::endl;

    if (enemyX >= cameraLeft && enemyX <= cameraRight)
    {
        if (!isActive)
        {
            sprite->setPosition(initialPosition);
        }
        activate();
        std::cout << "activado" << std::endl;
    }
    else
    {
        deactivate();
        std::cout << "desactivado" << std::endl;
    }
}

bool Enemy::getActiveState() const
{
    return isActive;
}

void Enemy::activate()
{
    isActive = true;
}

void Enemy::deactivate()
{
    isActive = false;
}
