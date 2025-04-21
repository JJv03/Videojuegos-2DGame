#include "bat.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize bat with sprite, hitboxes, position, and game level/stage
Bat::Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
         const sf::Vector2f &zoneSize, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = BAT_SPEED;
    life = BAT_LIFE;
    score = BAT_SCORE;
    damage = BAT_DAMAGE;
}

// Update bat logic: handle spawning, movement, and deactivation
void Bat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                 const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
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
        batSpawnTimers = 3.0f; // Delay before spawning

        batToSpawn = true;
    }

    // Spawn bat after timer expires
    if (batToSpawn)
    {
        batSpawnTimers -= deltaTime;

        if (batSpawnTimers <= 0.0f)
        {
            bool spawnFromRight = true;
            if (playerDir > 0.f)
            {
                spawnFromRight = false;
            }

            movePositionToBorder(playerActivationZone, playerBounds, spawnFromRight);
            float dir = spawnFromRight ? 1.0f : -1.0f;
            speed.x = speed.x * dir;

            isActive = true;
            batToSpawn = false;
        }
    }

    // MOVEMENT LOGIC: Movement and oscillation when active
    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }

        bool batInsideDeactivationZone = false;

        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                batInsideDeactivationZone = true;
                break;
            }
        }

        if (!batInsideDeactivationZone)
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

void Bat::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            resetPosition();
        }
    }
    else if (dynamic_cast<Player *>(&other))
    {
        isActive = false;
        resetPosition();
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
void Bat::movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds, bool spawnFromRight)
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

// Render bat and debug info (spawn zone)
void Bat::draw(sf::RenderWindow &window)
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

void Bat::hello() const
{
    std::cout << "Soy Bat" << std::endl;
}