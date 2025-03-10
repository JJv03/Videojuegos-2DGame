#include "enemy.h"
#include <iostream>

Enemy::Enemy(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone)
    : Entity(_sprite, _hitboxes), activationZone(_activationZone)
{
    deactivationZone = _activationZone;
}

void Enemy::updateEnemyRespawn(float deltaTime, sf::Sprite *gSimonSprite)
{
    sf::FloatRect playerBounds = gSimonSprite->getGlobalBounds();

    bool playerInsideActivationZone = activationZone.findIntersection(playerBounds).has_value();
    bool playerInsideDeactivationZone = deactivationZone.findIntersection(playerBounds).has_value();

    if (playerInsideActivationZone && !playerWasNearActivation && !isActive)
    {
        isActive = true;
    }

    if (isActive && !playerInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    playerWasNearActivation = playerInsideActivationZone;
}

void Enemy::update(float deltaTime)
{
    if (isActive)
    {
        applyGravity(deltaTime);

        sf::Vector2f horizontalMovement = {speed.x * deltaTime, 0.f};
        sprite->move(horizontalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.x += horizontalMovement.x;
        }

        deactivationZone.position.x += horizontalMovement.x;
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

        deactivationZone.position.y += verticalMovement.y;
    }
}

void Enemy::checkCollisions(const std::vector<sf::FloatRect> &boundsList)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    for (auto &hitbox : hitboxes)
    {
        for (const auto &bounds : boundsList)
        {
            if (const std::optional<sf::FloatRect> intersection = hitbox.findIntersection(bounds))
            {
                float overlapX = intersection->size.x;
                float overlapY = intersection->size.y;

                if (&bounds == &boundsList[0]) // Suelo
                {
                    sprite->move({0.f, -overlapY});
                    for (auto &h : hitboxes)
                        h.position.y -= overlapY;
                    deactivationZone.position.y -= overlapY;
                    isOnGround = true;
                }
                else if (overlapX < overlapY) // Colisión horizontal
                {
                    if (hitbox.position.x < bounds.position.x + bounds.size.x / 2.f)
                    {
                        // Colisión desde la izquierda
                        sprite->move({-overlapX, 0.f});
                        for (auto &h : hitboxes)
                            h.position.x -= overlapX;
                        deactivationZone.position.x -= overlapX;
                    }
                    else
                    {
                        // Colisión desde la derecha
                        sprite->move({overlapX, 0.f});
                        for (auto &h : hitboxes)
                            h.position.x += overlapX;
                        deactivationZone.position.x += overlapX;
                    }
                    speed.x = -speed.x;
                }
                else // Colisión vertical
                {
                    if (hitbox.position.y < bounds.position.y + bounds.size.y / 2.f)
                    {
                        // Colisión desde arriba
                        sprite->move({0.f, -overlapY});
                        for (auto &h : hitboxes)
                            h.position.y -= overlapY;
                        deactivationZone.position.y -= overlapY;
                        isOnGround = true;
                    }
                    else
                    {
                        // Colisión desde abajo
                        sprite->move({0.f, overlapY});
                        for (auto &h : hitboxes)
                            h.position.y += overlapY;
                        deactivationZone.position.y += overlapY;
                    }
                }
            }
        }
    }
}

bool Enemy::checkHitByEnemy(const sf::FloatRect simonBounds)
{
    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            std::cout << "El jugador ha sido golpeado por el enemigo!" << std::endl;
            return true;
        }
    }
    return false;
}

void Enemy::resetPosition()
{
    sf::Vector2f offset = originalPosition - sprite->getPosition();

    sprite->setPosition(originalPosition);

    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    deactivationZone.position += offset;

    speed = ORIGINAL_SPEED;
}