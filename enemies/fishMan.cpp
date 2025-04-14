#include "fishman.h"
#include <iostream>
#include <cmath>
#include <random>

// Constructor: Initialize fishman with sprite, hitboxes, position, and game level/stage
FishMan::FishMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                 const sf::Vector2f &zoneSize, const int &level, const int &stage)
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
        fishManSpawnTimers = 3.0f;
        fishManToSpawn = true;
    }

    // Spawn fishman after timer expires
    if (fishManToSpawn)
    {
        fishManSpawnTimers -= deltaTime;

        if (fishManSpawnTimers <= 0.0f)
        {
            moveToSpawnPosition(playerActivationZone, playerBounds);

            currentState = State::JUMPING;
            startJump();

            isActive = true;
            fishManToSpawn = false;
        }
    }

    // MOVEMENT & STATE LOGIC
    if (isActive)
    {
        applyGravity(deltaTime);

        // Check if enemy is out of deactivation zone
        bool enemyInsideDeactivationZone = false;
        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                enemyInsideDeactivationZone = true;
                break;
            }
        }

        if (!enemyInsideDeactivationZone)
        {
            isActive = false;
            resetPosition();
            return;
        }

        // Handle different states
        switch (currentState)
        {
        case State::SPAWNING:
            currentState = State::JUMPING;
            startJump();
            break;

        case State::JUMPING:
        {
            float verticalSpeed = -speed.y * deltaTime;
            sprite->move({0.f, verticalSpeed});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y += verticalSpeed;
            }

            if (isOnGround)
            {
                currentState = State::WALKING;

                facePlayer(playerBounds);
                speed.y = 0;
                attackTimer = 0.0f;
            }
        }
        break;

        case State::WALKING:
        {
            if (speed.y != 0)
            {
                sprite->move({0.f, -speed.y * deltaTime});
                for (auto &hitbox : hitboxes)
                {
                    hitbox.position.y -= speed.y * deltaTime;
                }
            }
            else if (speed.x != 0)
            {
                sprite->move({speed.x * deltaTime * 2, 0.f});
                for (auto &hitbox : hitboxes)
                {
                    hitbox.position.x += speed.x * deltaTime * 2;
                }
            }

            attackTimer += deltaTime;
            if (attackTimer >= ATTACK_INTERVAL)
            {
                startAttackPause();
                facePlayer(playerBounds);
            }
        }
        break;

        case State::PAUSED_FOR_ATTACK:
            pauseTimer -= deltaTime;
            if (pauseTimer <= 0.0f)
            {
                // AQUI SE LANZA EL PROYECTIL
                currentState = State::WALKING;
                attackTimer = 0.0f;
            }
            break;
        }

        updateAnimation(deltaTime);
    }

    // Right before checkCollisions
    isOnGround = false;
}

// Face the player direction
void FishMan::facePlayer(const sf::FloatRect &playerBounds)
{
    if (!sprite)
        return;

    // Get center positions
    float fishmanCenterX = sprite->getPosition().x;
    float playerCenterX = playerBounds.position.x + playerBounds.size.x / 2.0f;

    // Set direction based on relative position
    if (playerCenterX < fishmanCenterX)
    {
        // Player is to the left
        speed.x = -std::abs(speed.x);
        sprite->setScale({1.0f, 1.0f});
    }
    else
    {
        // Player is to the right
        speed.x = std::abs(speed.x);
        sprite->setScale({-1.0f, 1.0f});
    }
}

// Enter attack mode
void FishMan::startAttackPause()
{
    currentState = State::PAUSED_FOR_ATTACK;
    pauseTimer = ATTACK_PAUSE_TIME;
}

// Initiate the jump when spawning
void FishMan::startJump()
{
    speed.y = JUMP_VELOCITY;
    isOnGround = false;
}

// BORRAR
// Handle collisions
void FishMan::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                              const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;
}

// Handle collisions - only with ground
void FishMan::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    // Collision with solid tiles (ground only)
    if (dynamic_cast<SolidTile *>(&other))
    {
        if (speed.y <= 0)
        {
            onCollision_OnlyGround(other);
        }
    }
    // Collision with Whip (player attack)
    else if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (applyDamage(whip->whipDmg))
        {
            resetPosition();
        }
    }
    // Collision with Player
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

    currentState = State::SPAWNING;
    spawnTime = 0.0f;
    attackTimer = 0.0f;
    pauseTimer = 0.0f;

    animTimer = 0.0f;
    currentFrame = 0;

    isOnGround = false;
}

// Move fishman to spawn position randomly within activation zone
void FishMan::moveToSpawnPosition(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds)
{
    if (!sprite)
        return;

    // Calculate bounds for random spawn
    float minX = playerActivationZone.position.x + 25.0f;
    float maxX = playerActivationZone.position.x + playerActivationZone.size.x - 50.0f;

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> xDist(minX, maxX);

    // Save original position
    sf::Vector2f oldPosition = sprite->getPosition();

    // Generate random X position
    float randomX = xDist(gen);
    float alignedY = playerActivationZone.position.y + playerActivationZone.size.y;

    // New position
    sf::Vector2f newPosition(randomX, alignedY);
    sprite->setPosition(newPosition);

    // Update hitboxes to match new position
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    // Face the player initially
    facePlayer(playerBounds);
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

        // Different animations based on state
        /*switch (currentState)
        {
        case State::JUMPING:
            currentFrame = 0;
            break;

        case State::WALKING:
            currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
            break;

        case State::PAUSED_FOR_ATTACK:
            currentFrame = 1;
            break;

        default:
            currentFrame = 0;
            break;
        }*/
    }

    // Flip sprite based on movement direction
    if (speed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (speed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}

void FishMan::hello() const
{
    std::cout << "Soy FishMan" << std::endl;
}
