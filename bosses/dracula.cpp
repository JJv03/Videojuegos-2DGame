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

    gKilledBoss = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);
    animationManager->addAnimation(draculaMask, this->maskDraculaFrames);
    animationManager->addAnimation(draculaIdle, this->idleDraculaFrames);
    animationManager->addAnimation(draculaAttack, this->attackDraculaFrames);

    this->animationManager = animationManager;
    currentAnimation = sleepPhantomBat;
}

// Update dracula logic: handle spawning, movement, and deactivation
void Dracula::update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    if (!isActive && phase == 1)
    {
        isActive = true;
    }

    // MOVEMENT LOGIC
    if (isActive)
    {
        // MAQUINA DE ESTADOS DEL BOSS
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL
        switch(currentState){
            case DraculaState::ASLEEP:
                if(!animationManager->isPlaying(noAnimation)){
                    animationManager->playAnimation(noAnimation);
                }

                
                break;

            case DraculaState::MASK_APPEAR:

                break;

            case DraculaState::MASK_ELEVATE:

                break;

            case DraculaState::BATTLE_IDLE:

                break;

            case DraculaState::BATTLE_APPEAR:

                break;

            case DraculaState::BATTLE_ATTACK:

                break;

            case DraculaState::BATTLE_DISAPPEAR:

                break;

            case DraculaState::DEAD_MASK_OFF:

                break;

            default:
                break;
        }


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

// Render dracula and debug info
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