#include "zombie.h"
#include <iostream>

// Initialize zombie with default stats
Zombie::Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Enemy(_sprite, _hitboxes)
{
    speed = ZOMBIE_SPEED;
    life = ZOMBIE_LIFE;
    score = ZOMBIE_SCORE;
    damage = ZOMBIE_DAMAGE;
}

// Main update loop
void Zombie::update(float deltaTime)
{
    if (isActive)
    {
        applyGravity(deltaTime);

        // Horizontal movement
        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }
        // Vertical movement
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }

        updateAnimation(deltaTime);
    }
}

// Handle collision
void Zombie::checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    for (auto &hitbox : hitboxes)
    {
        // Environment collisions
        checkBasicCollisions(tileMap, hitbox);

        // Weapon collisions
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

// Reset to initial state
void Zombie::resetPosition()
{
    Enemy::resetPosition();

    speed = ZOMBIE_SPEED;
    life = ZOMBIE_LIFE;

    animTimer = 0.0f;
    currentFrame = 0;
}

// Move zombie at activation zone edge (for spawning)
void Zombie::movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist)
{
    if (!sprite)
        return;

    // Calculate spawn position right outside activation zone
    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x + dist + 20.0f;
    float originalY = sprite->getPosition().y;

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

// Update animation frame and flip sprite based on direction
void Zombie::updateAnimation(float deltaTime)
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