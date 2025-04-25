#include "draculaSpirit.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize draculaSpirit with sprite, hitboxes, position, and game level/stage
DraculaSpirit::DraculaSpirit(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                             const int &level, const int &stage) : Boss(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = DRACULASPIRIT_SPEED;
    life = DRACULASPIRIT_LIFE;
    score = DRACULASPIRIT_SCORE;
    damage = DRACULASPIRIT_DAMAGE;
}

// Update draculaSpirit logic: handle spawning, movement, and deactivation
void DraculaSpirit::update(float deltaTime, const Player &player, const sf::FloatRect &mapBounds){
    // SPAWN LOGIC
    if (!isActive)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (player.gPlayerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive)
    {
        // MAQUINA DE ESTADOS DEL BOSS
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL

        updateAnimation(deltaTime);
    }
}

void DraculaSpirit::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            // DROP
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            // DROP
        }
    }
}

// Render draculaSpirit
void DraculaSpirit::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void DraculaSpirit::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite)
        return;

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

void DraculaSpirit::hello() const
{
    std::cout << "Soy DraculaSpirit" << std::endl;
}