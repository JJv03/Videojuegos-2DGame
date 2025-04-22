#include "zombie.h"
#include "../game.h"
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

        // Right before checkCollisions
        isOnGround = false;

        updateAnimation(deltaTime);
    }
}

void Zombie::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);
    }
    else if (MiscellaneousTile *miscTile = dynamic_cast<MiscellaneousTile *>(&other))
    {
        if (!miscTile->isDestroyed && miscTile->isCollidable())
        {
            onCollision_MiscTile(miscTile->getBounds()[0]);
        }
    }
    else if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            resetPosition();
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
void Zombie::movePositionToBorder(const sf::FloatRect &playerActivationZone, float dist, bool fromRight)
{
    if (!sprite)
        return;

    // Calculate spawn position outside activation zone
    float spawnX;
    if (fromRight)
    {
        spawnX = playerActivationZone.position.x + playerActivationZone.size.x + dist + 5.0f;
    }
    else
    {
        spawnX = playerActivationZone.position.x - dist - 5.0f - sprite->getGlobalBounds().size.x;
    }
    float originalY = sprite->getPosition().y;

    // Save original position
    sf::Vector2f oldPosition = sprite->getPosition();

    // New position
    sf::Vector2f newPosition(spawnX, originalY);
    sprite->setPosition(newPosition);

    // Update hitboxes to match new position
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

void Zombie::setDirection(float dir)
{
    speed.x = speed.x * dir;
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

void Zombie::hello() const
{
    std::cout << "Soy Zombie" << std::endl;
}