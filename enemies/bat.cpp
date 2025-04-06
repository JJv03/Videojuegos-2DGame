#include "bat.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize bat with sprite, hitboxes, position, and game level/stage
Bat::Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
         const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = BAT_SPEED;
    life = BAT_LIFE;
    score = BAT_SCORE;
    damage = BAT_DAMAGE;
}

// Update bat logic: handle spawning, movement, and deactivation
void Bat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone)
{
    // SPAWN LOGIC: activate bat when player enters spawn zone
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    if (batToSpawn)
    {
        spawnerActive = true;
    }
    else
    {
        spawnerActive = false;
    }

    if (playerInZone && !isActive && !spawnerActive)
    {
        batSpawnTimers = 1.5f; // Delay before spawning

        batToSpawn = true;
    }

    // Spawn bat after timer expires
    if (batToSpawn)
    {
        batSpawnTimers -= deltaTime;

        if (batSpawnTimers <= 0.0f)
        {
            movePositionToBorder(playerActivationZone);

            isActive = true;
            batToSpawn = false;
        }
    }

    // MOVEMENT LOGIC: Movement and oscillation when active
    if (isActive)
    {

        bool zombieInsideDeactivationZone = false;

        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                zombieInsideDeactivationZone = true;
                break;
            }
        }

        if (!zombieInsideDeactivationZone)
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
            float verticalOscillation = cos(spawnTime * OSCILLATION_SPEED) * OSCILLATION_AMPLITUDE;
            sprite->move({0.f, verticalOscillation});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y += verticalOscillation;
            }

            updateAnimation(deltaTime);
        }
    }
}

// Handle collisions
void Bat::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                          const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    for (auto &hitbox : hitboxes)
    {

        // Check weapon collisions
        if (playerIsAtacking)
        {

            if (weaponBounds.findIntersection(hitbox).has_value())
            {
                std::cout << "Vida = " << life << std::endl;
                life -= playerDamage;
                std::cout << "Daño recibido = " << playerDamage << " | " << "Vida = " << life << std::endl;
                if (life <= 0.0f)
                {
                    isActive = false;
                    resetPosition();
                    std::cout << "Enemigo eliminado. Puntos = " << score << std::endl;
                    break;
                }
            }
        }
    }

    // Check player collisions
    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            isActive = false;
            resetPosition();
            break;
        }
    }
}

// Reset bat to initial state
void Bat::resetPosition()
{
    Enemy::resetPosition();

    speed = BAT_SPEED;
    life = BAT_LIFE;

    spawnTime = 0.0f;

    animTimer = 0.0f;
    currentFrame = 0;
}

// Move bat to spawn position at the edge of player's activation zone
void Bat::movePositionToBorder(const sf::FloatRect &playerActivationZone)
{

    if (!sprite)
        return;

    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x;
    float originalY = (playerActivationZone.position.y + playerActivationZone.size.y / 2.0f) - 5.0f;

    // Save original position
    sf::Vector2f oldPosition = sprite->getPosition();

    // New position
    sf::Vector2f newPosition(rightEdgeX, originalY);
    sprite->setPosition(newPosition);

    // Update hitboxes to match new position
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

// Render bat and debug info (spawn zone)
void Bat::draw(sf::RenderWindow &window, bool debugDraw)
{
    if (sprite && isActive)
    {
        Enemy::draw(window, debugDraw);
    }

    if (debugDraw)
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

// Update animation frame and flip sprite based on direction
void Bat::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite)
        return;

    animTimer += deltaTime;

    if (animTimer >= ANIM_FRAME_TIME)
    {
        animTimer = 0.0f;
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;

        /*if (currentFrame == 0) {

        } else {

        }*/
    }

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