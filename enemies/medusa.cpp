#include "medusa.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize medusa with sprite, hitboxes, position, and game level/stage
Medusa::Medusa(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
               const sf::Vector2f &zoneSize, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = MEDUSA_SPEED;
    life = MEDUSA_LIFE;
    score = MEDUSA_SCORE;
    damage = MEDUSA_DAMAGE;
}

// Update medusa logic: handle spawning, movement, and deactivation
void Medusa::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                    const float playerDir, const sf::FloatRect &playerBounds, const PlayerPosition playerPos, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC: activate medusa when player enters spawn zone
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    if (medusaToSpawn)
    {
        spawnerActive = true;
    }
    else
    {
        spawnerActive = false;
    }

    if (playerInZone && !isActive && !spawnerActive)
    {
        medusaSpawnTimers = 0.25f; // Delay before spawning

        medusaToSpawn = true;
    }

    // Spawn medusa after timer expires
    if (medusaToSpawn)
    {
        medusaSpawnTimers -= deltaTime;

        if (medusaSpawnTimers <= 0.0f)
        {
            bool spawnFromRight = true;

            switch (playerPos)
            {
            case PlayerPosition::LEFT:
                spawnFromRight = true;
                break;

            case PlayerPosition::RIGHT:
                spawnFromRight = false;
                break;

            case PlayerPosition::CENTER:
                spawnFromRight = (playerDir > 0.f) ? false : true;
                break;
            }

            movePositionToBorder(playerActivationZone, playerBounds, spawnFromRight);
            float dir = spawnFromRight ? 1.0f : -1.0f;
            speed.x = speed.x * dir;

            isActive = true;
            medusaToSpawn = false;
        }
    }

    // MOVEMENT LOGIC: Movement and oscillation when active
    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }

        bool medusaInsideDeactivationZone = false;

        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                medusaInsideDeactivationZone = true;
                break;
            }
        }

        if (!medusaInsideDeactivationZone)
        {

            isActive = false;
            resetPosition();
        }
        else
        {
            float horizontalSpeed = speed.x * deltaTime;
            sprite->move({horizontalSpeed, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += horizontalSpeed;
            }

            spawnTime += deltaTime;
            // Oscillation effect (up-down movement)
            float verticalOscillation = cos(spawnTime * OSCILLATION_SPEED) * OSCILLATION_AMPLITUDE * deltaTime;
            sprite->move({0.f, verticalOscillation});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y += verticalOscillation;
            }

            updateAnimation(deltaTime);
        }
    }
}

void Medusa::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
}

// Reset medusa to initial state
void Medusa::resetPosition()
{
    Enemy::resetPosition();

    speed = MEDUSA_SPEED;
    life = MEDUSA_LIFE;

    spawnTime = 0.0f;

    currentAnimation = noAnimation;
}

// Move medusa to spawn position at the edge of player's activation zone
void Medusa::movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds, bool spawnFromRight)
{

    if (!sprite)
        return;

    float edgeX = spawnFromRight
                      ? playerActivationZone.position.x + playerActivationZone.size.x
                      : playerActivationZone.position.x - 20.0f;
    float playerCenterY = playerBounds.position.y + playerBounds.size.y / 2.0f;
    float alignedY = playerCenterY - 5.0f;

    // Save original position
    sf::Vector2f oldPosition = sprite->getPosition();

    // New position
    sf::Vector2f newPosition(edgeX, alignedY);
    sprite->setPosition(newPosition);

    // Update hitboxes to match new position
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

// Render medusa and debug info (spawn zone)
void Medusa::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }

    if (gDrawHitboxes)
    {
        // Draw spawn zone for debugging
        sf::RectangleShape zoneShape({spawnZone.size.x, spawnZone.size.y});
        zoneShape.setPosition({spawnZone.position.x, spawnZone.position.y});
        zoneShape.setFillColor(sf::Color(255, 255, 0, 50));
        zoneShape.setOutlineColor(sf::Color::Yellow);
        zoneShape.setOutlineThickness(1.0f);
        window.draw(zoneShape);
    }
}

void Medusa::resetSpawnState()
{
    medusaToSpawn = false;
    medusaSpawnTimers = 0.0f;
    spawnerActive = false;
}

// Update animation frame and flip sprite based on direction
void Medusa::updateAnimation(float deltaTime)
{

    // Flip sprite based on movement direction
    sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}

void Medusa::hello() const
{
    std::cout << "Soy Medusa" << std::endl;
}