#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Entity(_sprite, _hitboxes)
{
    // Guarda la posición inicial
    originalPosition = sprite->getPosition();
}

void Enemy::updateEnemyRespawn(float deltaTime, const sf::FloatRect &gPlayerActivationZone, const sf::FloatRect &gPlayerDeactivationZone)
{
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    for (const auto &hitbox : hitboxes)
    {
        if (gPlayerActivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideActivationZone = true;
        }
        if (gPlayerDeactivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideDeactivationZone = true;
        }
    }

    // Si el jugador está fuera de la zona de activación, se permite que el enemigo se reactive en el futuro
    if (!enemyInsideActivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // Solo activamos si el jugador está en la zona, el enemigo no está activo y el jugador se alejó previamente
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        std::cout << "activo" << std::endl;
        isActive = true;
    }

    // Se desactiva si el enemigo está activo y salió de la zona de desactivación
    if (isActive && !enemyInsideDeactivationZone)
    {
        std::cout << "desactivo" << std::endl;
        isActive = false;
        resetPosition();
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

// SEGURAMENTE HAYA QUE CAMBIAR COSAS Y DEJAR LO ÚNICO QUE ES COMÚN PARA TODOS
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
                    }
                    else
                    {
                        // Colisión desde la derecha
                        sprite->move({overlapX, 0.f});
                        for (auto &h : hitboxes)
                            h.position.x += overlapX;
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
                        isOnGround = true;
                    }
                    else
                    {
                        // Colisión desde abajo
                        sprite->move({0.f, overlapY});
                        for (auto &h : hitboxes)
                            h.position.y += overlapY;
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