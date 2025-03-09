#include "enemy.h"

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
        sf::Vector2f movement = speed * deltaTime;
        sprite->move(movement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position += movement;
        }

        deactivationZone.position += movement;
    }
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
}