#include "phantomBat.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
PhantomBat::PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage) : Boss(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = PBAT_SPEED;
    life = PBAT_LIFE;
    score = PBAT_SCORE;
    damage = PBAT_DAMAGE;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(sleepPhantomBat, this->idlePhBatFrames);
    animationManager->addAnimation(flyPhantomBat, this->flyPhBatFrames);

    this->animationManager = animationManager;
    currentAnimation = sleepPhantomBat;
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
            if (playerActivationZone.findIntersection(hitbox).has_value() && isInBossFight)
            {
                isActive = true;
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {
        // MAQUINA DE ESTADOS DEL BOSS: PRIMERO ESTA UN TIEMPO QUIETO Y DESPUES EMPIZA A MOVERSE/ATACAR
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL
        // ESTA EN REPOSO HASTA QUE SE BLOQUEA LA OPCION DE IR HACIA ATRÁS QUE ES CUANDO SE EMPIEZA A MOVER

        // Wait 2 seconds and then go to center
        if(starting){
            timer += deltaTime;
            
            if (timer >= sleepInterval) {
                timer = 0.0f;
                starting = false;
                currentAnimation = flyPhantomBat;
            }
        }
        else{
            auto mode = configManager.getDifficulty();
            if (!mode.hard_mode){   // Normal mode
                
            }
            else{                   // Enhanced AI mode

            }
        }
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
            game.createDropItem(DropType::MAGIC_CRYSTAL, sprite->getGlobalBounds().position);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sprite->getGlobalBounds().position);
        }
    }
}

// Render phantomBat and debug info (spawn zone)
void PhantomBat::draw(sf::RenderWindow &window)
{
    if (sprite)
    {
        Boss::draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void PhantomBat::updateAnimation(float deltaTime)
{
    // if (!isActive || !sprite)
    //     return;

    // // Flip sprite based on movement direction
    // sf::Vector2f currentSpeed = speed;

    // if (currentSpeed.x < 0)
    // {
    //     sprite->setScale({1.0f, 1.0f});
    // }
    // else if (currentSpeed.x > 0)
    // {
    //     sprite->setScale({-1.0f, 1.0f});
    // }

    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);
}

void PhantomBat::hello() const
{
    std::cout << "Soy PhantomBat" << std::endl;
}
