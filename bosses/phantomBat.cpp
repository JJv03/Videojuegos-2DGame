#include "phantomBat.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
PhantomBat::PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const sf::Vector2f &zoneSize, const int &level, const int &stage) : Boss(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = PBAT_SPEED;
    life = PBAT_LIFE;
    score = PBAT_SCORE;
    damage = PBAT_DAMAGE;
}

// Update phantomBat logic: handle spawning, movement, and deactivation
void PhantomBat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
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
        // MAQUINA DE ESTADOS DEL BOSS: PRIMERO ESTA UN TIEMPO QUIETO Y DESPUES EMPIZA A MOVERSE/ATACAR
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL

        updateAnimation(deltaTime);
    }
}

void PhantomBat::onCollision(Entity &other, Game &game)
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
}

// Render phantomBat and debug info (spawn zone)
void PhantomBat::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void PhantomBat::updateAnimation(float deltaTime)
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

void PhantomBat::hello() const
{
    std::cout << "Soy PhantomBat" << std::endl;
}
