#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Entity(_sprite, _hitboxes)
{
    // Guarda la posición inicial
    originalPosition = sprite->getPosition();
}

void Enemy::draw(sf::RenderWindow &window, bool debugDraw)
{
    window.draw(*sprite);

    if (debugDraw)
    {
        for (const auto &hitbox : hitboxes)
        {
            sf::RectangleShape hitboxShape({hitbox.size.x, hitbox.size.y});
            hitboxShape.setPosition({hitbox.position.x, hitbox.position.y});
            hitboxShape.setFillColor(sf::Color::Transparent);
            hitboxShape.setOutlineColor(sf::Color::Red);
            hitboxShape.setOutlineThickness(1.0f);
            window.draw(hitboxShape);
        }
    }
}

void Enemy::applyGravity(float deltaTime)
{
    if (!isOnGround)
    {
        sf::Vector2f verticalMovement = {0.f, (VERTICAL_SPEED * deltaTime)};
        sprite->move(verticalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.y += verticalMovement.y;
        }
    }
}

bool Enemy::checkHitByEnemy(const sf::FloatRect simonBounds)
{
    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            std::cout << "Contacto con enemigo" << std::endl;
            return true;
        }
    }
    return false;
}

void Enemy::resetPosition()
{
    sf::Vector2f offset = originalPosition - sprite->getPosition();

    needsPlayerToLeaveZone = true;

    sprite->setPosition(originalPosition);

    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    speed = {-75.0f, 0.0f};
}