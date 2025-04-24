#include "dracula.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize dracula with sprite, hitboxes, position, and game level/stage
Dracula::Dracula(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                 const int &level, const int &stage) : Boss(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = DRACULA_SPEED;
    life = DRACULA_LIFE;
    score = DRACULA_SCORE;
    damage = DRACULA_DAMAGE;
}

// Update dracula logic: handle spawning, movement, and deactivation
void Dracula::update(float deltaTime, const sf::FloatRect &playerActivationZone)
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

    // MOVEMENT LOGIC
    if (isActive)
    {
        // MAQUINA DE ESTADOS DEL BOSS
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL

        updateAnimation(deltaTime);
    }
}

void Dracula::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            // DROP (la fase uno creo que no da nada)
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            // DROP (la fase uno creo que no da nada)
        }
    }
}

// Render dracula and debug info (spawn zone)
void Dracula::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void Dracula::updateAnimation(float deltaTime)
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

void Dracula::hello() const
{
    std::cout << "Soy Dracula" << std::endl;
}