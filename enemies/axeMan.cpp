#include "axeMan.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Initialize ghost with stats and vision field
AxeMan::AxeMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = AXEMAN_SPEED;
    life = AXEMAN_LIFE;
    score = AXEMAN_SCORE;
    damage = AXEMAN_DAMAGE;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Cannon AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(AxeManMovement, this->axeMovementFrames);

    this->animationManager = animationManager;
    currentAnimation = AxeManMovement;
}

// Main update loop
void AxeMan::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                   const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    if (playerActivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideActivationZone = true;
    }
    if (playerDeactivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideDeactivationZone = true;
    }

    // If the player is outside the deactivation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideDeactivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // We only activate if the player is in the area, the enemy is not active and the player has previously moved away
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        isActive = true;
    }

    // Deactivates if the enemy is active and has left the deactivation zone
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }

        updateAnimation(deltaTime);
    }
    
}

void AxeMan::onCollision(Entity &other, Game &game)
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

// Reset to initial state
void AxeMan::resetPosition()
{
    Enemy::resetPosition();

    speed = AXEMAN_SPEED;
    life = AXEMAN_LIFE;

    sprite->setScale({1.0f, 1.0f});
}

// Render with optional debug visuals
void AxeMan::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
}

// Update animation frame and direction
void AxeMan::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}

void AxeMan::hello() const
{
    std::cout << "Soy AxeMan" << std::endl;
}