#include "dracula.h"
#include "../game.h"
#include <iostream>
#include <cmath>

DraculaBody::DraculaBody(std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes)
                            : EntitySprite(_draculaSprite, _draculaHitboxes) {}


void DraculaBody::onCollision(Entity &other, Game &game){

}

void DraculaBody::hello() const{
    std::cout << "Soy DraculaBody" << std::endl;
}

std::vector<sf::FloatRect> DraculaBody::getBounds() const{
    if(animationManager->isPlaying(noAnimation)){
        return std::vector<sf::FloatRect>({sf::FloatRect()});
    }
    return std::vector<sf::FloatRect>({this->sprite->getGlobalBounds()});
}


// -------------------------- DRACULA MASK --------------------------

// Constructor: Initialize dracula with sprite, hitboxes, position, and game level/stage
Dracula::Dracula(std::shared_ptr<sf::Sprite> _maskSprite, std::vector<sf::FloatRect> &_maskHitboxes,
                 std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes,
                 const int &level, const int &stage) 
                 : Boss(_maskSprite, _maskHitboxes), level(level), stage(stage)
{

    speed = DRACULA_SPEED;
    life = DRACULA_LIFE;
    score = DRACULA_SCORE;
    damage = DRACULA_DAMAGE;

    gKilledBoss = false;

    asleepTimeCounter = 0.f;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Dracula Mask AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);
    animationManager->addAnimation(draculaMask, this->maskDraculaFrames);

    this->animationManager = animationManager;
    currentAnimation = noAnimation;

    // -------------------------
    draculaBody = new DraculaBody(_draculaSprite, _draculaHitboxes);
    AnimationManager *bodyAnimationManager = new AnimationManager(*draculaBody->sprite, draculaBody);
    if (!bodyAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Dracula Body AnimationManager!" << std::endl;
    }


    bodyAnimationManager->addAnimation(noAnimation, this->noAnimationFrames);
    bodyAnimationManager->addAnimation(draculaIdle, draculaBody->idleDraculaFrames);
    bodyAnimationManager->addAnimation(draculaAttack, draculaBody->attackDraculaFrames);

    draculaBody->animationManager = bodyAnimationManager;
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
        drawBody(window);
        Boss::draw(window);
    }
}

void Dracula::drawBody(sf::RenderWindow &window)
{
    window.draw(*draculaBody->sprite); // Draw the boss sprite

    if (gDrawHitboxes) // Only draw hitboxes in debug mode
    {
        for (const auto &hitbox : draculaBody->getBounds())
        {
            sf::RectangleShape hitboxShape({hitbox.size.x, hitbox.size.y});
            hitboxShape.setPosition({hitbox.position.x, hitbox.position.y});
            hitboxShape.setFillColor(sf::Color::Transparent);
            hitboxShape.setOutlineColor(sf::Color::Red);
            hitboxShape.setOutlineThickness(1.0f);
            window.draw(hitboxShape);
        }
    }
}

// Update animation frame and flip sprite based on direction
void Dracula::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    switch(currentState){
        case DraculaState::ASLEEP:
            applyMaskBodyAnimation(noAnimation);
            break;

        case DraculaState::MASK_APPEAR:
            applyMaskBodyAnimation(draculaMask);
            break;

        case DraculaState::MASK_ELEVATE:
            applyMaskBodyAnimation(draculaMask);
            break;

        case DraculaState::BATTLE_IDLE:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_APPEAR:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_ATTACK:
            applyMaskBodyAnimation(draculaAttack);
            break;

        case DraculaState::BATTLE_DISAPPEAR:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::DEAD_MASK_OFF:
            applyMaskBodyAnimation(draculaIdle);
            deadMaskAnimation();
            break;

        default:
            break;
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

void Dracula::applyMaskBodyAnimation(animationID id){
    switch(id){
        case noAnimation:
            if(!this->animationManager->isPlaying(noAnimation)){
                this->animationManager->playAnimation(noAnimation);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;
            
        case draculaMask:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;

        case draculaIdle:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(draculaIdle)){
                draculaBody->animationManager->playAnimation(draculaIdle);
            }
            break;

        case draculaAttack:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(draculaAttack)){
                draculaBody->animationManager->playAnimation(draculaAttack);
            }
            break;
        
        default:
            if(!this->animationManager->isPlaying(noAnimation)){
                this->animationManager->playAnimation(noAnimation);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;
    }
}

void Dracula::deadMaskAnimation(){
    // TO DO
}

std::vector<sf::FloatRect> Dracula::getBounds() const{
    if(animationManager->isPlaying(noAnimation)){
        return std::vector<sf::FloatRect>({sf::FloatRect()});
    }
    return std::vector<sf::FloatRect>({sprite->getGlobalBounds()});
}

void Dracula::hello() const
{
    std::cout << "Soy Dracula" << std::endl;
}
