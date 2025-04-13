#include "fishman.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize fishman with sprite, hitboxes, position, and game level/stage
FishMan::FishMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                 const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = FISHMAN_SPEED;
    life = FISHMAN_LIFE;
    score = FISHMAN_SCORE;
    damage = FISHMAN_DAMAGE;
}

// Update fishman logic: handle spawning, movement, and deactivation
void FishMan::update(float deltaTime, const sf::FloatRect &playerActivationZone,
                     const sf::FloatRect &playerDeactivationZone, const sf::FloatRect &playerBounds)
{
    // SPAWN LOGIC: activate fishman when player enters spawn zone
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    if (fishManToSpawn)
    {
        spawnerActive = true;
    }
    else
    {
        spawnerActive = false;
    }

    if (playerInZone && !isActive && !spawnerActive)
    {
        fishManSpawnTimers = 1.5f; // Delay before spawning

        fishManToSpawn = true;
    }

    // Spawn fishman after timer expires
    if (fishManToSpawn)
    {
        fishManSpawnTimers -= deltaTime;

        if (fishManSpawnTimers <= 0.0f)
        {
            movePositionToBorder(playerActivationZone, playerBounds);

            isActive = true;
            fishManToSpawn = false;
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

            updateAnimation(deltaTime);
        }
    }
}

// BORRAR
// Handle collisions
void FishMan::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
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
}

void FishMan::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);
    }
    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (applyDamage(whip->whipDmg))
        {
            resetPosition();
        }
    }
    else if (dynamic_cast<Player *>(&other))
    {
    }
}

// Reset fishman to initial state
void FishMan::resetPosition()
{
    Enemy::resetPosition();

    speed = FISHMAN_SPEED;
    life = FISHMAN_LIFE;

    spawnTime = 0.0f;

    animTimer = 0.0f;
    currentFrame = 0;
}

// Move fishman to spawn position at the edge of player's activation zone
void FishMan::movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds)
{

    if (!sprite)
        return;

    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x;
    float playerCenterY = playerBounds.position.y + playerBounds.size.y / 2.0f;
    float alignedY = playerCenterY - 5.0f;

    // Save original position
    sf::Vector2f oldPosition = sprite->getPosition();

    // New position
    sf::Vector2f newPosition(rightEdgeX, alignedY);
    sprite->setPosition(newPosition);

    // Update hitboxes to match new position
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

// Render fishman and debug info (spawn zone)
void FishMan::draw(sf::RenderWindow &window, bool debugDraw)
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
void FishMan::updateAnimation(float deltaTime)
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

void FishMan::hello() const
{
    std::cout << "Soy FishMan" << std::endl;
}