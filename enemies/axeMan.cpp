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
}

// Main update loop
void AxeMan::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                   const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    if (!isActive)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                break;
            }
        }
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
        std::cout << "HOLAAAAAAAAAAAAAA" << std::endl;
        Enemy::draw(window);
    }
}

// Update animation frame and direction
void AxeMan::updateAnimation(float deltaTime)
{
    
}

void AxeMan::hello() const
{
    std::cout << "Soy Ghost" << std::endl;
}