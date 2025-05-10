#include "ghost.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Initialize ghost with stats and vision field
Ghost::Ghost(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = GHOST_SPEED;
    life = GHOST_LIFE;
    score = GHOST_SCORE;
    damage = GHOST_DAMAGE;
}

// Main update loop
void Ghost::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                   const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds)
{
    playerPosition = {playerPos.x, playerPos.y - 16.0f};

    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    for (const auto &hitbox : hitboxes)
    {
        if (playerActivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideActivationZone = true;
        }
        if (playerDeactivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideDeactivationZone = true;
        }
    }

    // If the player is outside the deactivation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideDeactivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // We only activate if the player is in the area, the enemy is not active and the player has previously moved away
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        const float safeMargin = MIN_PLAYER_DISTANCE_TO_SPAWN;
        sf::FloatRect safeZone(
            {sprite->getPosition().x - safeMargin, sprite->getPosition().y - safeMargin},
            {safeMargin * 2.0f, safeMargin * 2.0f});

        sf::Vector2f playerCenter = playerPosition + sf::Vector2f(16.0f, 32.0f);

        if (!safeZone.contains(playerCenter))
        {
            isActive = true;
        }
    }

    // Deactivates if the enemy is active and has left the deactivation zone
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    // MOVEMENT AI LOGIC
    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
            return;

        // If stunned, decreases time and does not move
        if (stunTime > 0.0f)
        {
            stunTime -= deltaTime;
            speed = {0.0f, 0.0f};
        }
        else
        {
            // Movement towards the player with constant speed
            sf::Vector2f direction = playerPosition - sprite->getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length != 0)
            {
                direction /= length;
                speed = direction * GHOST_SPEED_MAGNITUDE;

                // Applies movement
                sprite->move(speed * deltaTime);
                for (auto &hitbox : hitboxes)
                {
                    hitbox.position += speed * deltaTime;
                }
            }
        }

        updateAnimation(deltaTime);
    }
}

void Ghost::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        stunTime = STUN_DURATION;

        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        stunTime = STUN_DURATION;

        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
}

// Reset to initial state
void Ghost::resetPosition()
{
    Enemy::resetPosition();

    speed = GHOST_SPEED;
    life = GHOST_LIFE;

    stunTime = 0.0f;

    sprite->setScale({1.0f, 1.0f});
}

// Render with optional debug visuals
void Ghost::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
}

// Update animation frame and direction
void Ghost::updateAnimation(float deltaTime)
{
    // falta animacion ****

    // Flip sprite based on movement direction
    sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x <= 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}

void Ghost::hello() const
{
    std::cout << "Soy Ghost" << std::endl;
}