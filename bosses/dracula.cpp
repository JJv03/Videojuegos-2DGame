#include "dracula.h"
#include "../game.h"
#include <iostream>
#include <cmath>

DraculaBody::DraculaBody(std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes,
                            const float _damage, sf::Vector2f _position)
                            : EntitySprite(_draculaSprite, _draculaHitboxes), damage(_damage), position(_position) {}


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
                 const int &level, const int &stage, sf::Vector2f bodyPosition) 
                 : Boss(_maskSprite, _maskHitboxes), level(level), stage(stage)
{

    speed = DRACULA_SPEED;
    life = DRACULA_LIFE;
    score = DRACULA_SCORE;
    damage = DRACULA_DAMAGE;

    gKilledBoss = false;

    asleepTimeCounter = 0.f;
    maskWaitTimeCounter = 0.f;
    appearTimeCounter = 0.f;
    disappearTimeCounter = 0.f;

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
    draculaBody = new DraculaBody(_draculaSprite, _draculaHitboxes, damage, bodyPosition);
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
                asleepTimeCounter += deltaTime;

                if(asleepTimeCounter >= ASLEEP_TIME){
                    currentState = DraculaState::MASK_APPEAR;
                }
                break;

            case DraculaState::MASK_APPEAR:

                maskWaitTimeCounter += deltaTime;

                if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                    maskWaitTimeCounter = 0.f;
                    currentState = DraculaState::MASK_ELEVATE;
                }
                break;

            case DraculaState::MASK_ELEVATE:
                if(sprite->getPosition().y > MASK_MAX_HEIGHT){
                    if(sprite->getPosition().y - MASK_MAX_HEIGHT < 1.f){
                        float elevation = sprite->getPosition().y - MASK_MAX_HEIGHT;
                        sprite->move({0.f, -elevation});
                        draculaBody->sprite->move({0.f, -elevation});
                    } else {
                        float elevation = MASK_ELEVATE_SPEED.y * deltaTime;
                        sprite->move({0.f, -elevation});
                        draculaBody->sprite->move({0.f, -elevation});
                    }
                } else { // Mask in position
                    maskWaitTimeCounter += deltaTime;

                    if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                        appearTimeCounter = 0.f;
                        currentState = DraculaState::BODY_APPEAR;
                    }
                }
                
                break;

            case DraculaState::BODY_APPEAR:
                if(appearBody(deltaTime)){
                    currentState = DraculaState::BATTLE_IDLE;
                }
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

void Dracula::resetPosition()
{
    Boss::resetPosition();

    life = DRACULA_LIFE;
    draculaBody->sprite->setPosition(draculaBody->position);

    asleepTimeCounter = 0.f;
    maskWaitTimeCounter = 0.f;
    appearTimeCounter = 0.f;
    disappearTimeCounter = 0.f;

    currentState = DraculaState::ASLEEP;

    applyMaskBodyAnimation(noAnimation);
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
        
        case DraculaState::BODY_APPEAR:
            applyMaskBodyAnimation(draculaIdle);
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

bool Dracula::appear(float deltaTime){
    if(appearTimeCounter < APPEAR_TIME / 2.f){
        appearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else if (appearTimeCounter < APPEAR_TIME){
        appearTimeCounter += deltaTime;
        sprite->setColor(sf::Color::White);
        currentState = DraculaState::BATTLE_IDLE;
        return true;
    }
    return true;
}

bool Dracula::disappear(float deltaTime){
    if(disappearTimeCounter < DISAPPEAR_TIME / 2.f){
        disappearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else if (disappearTimeCounter < DISAPPEAR_TIME){
        disappearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 0));
        currentState = DraculaState::BATTLE_AWAY;
        return true;
    }
    return true;
}

bool Dracula::appearBody(float deltaTime){
    if(appearTimeCounter < APPEAR_TIME / 2.f){
        appearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else if (appearTimeCounter < APPEAR_TIME){
        appearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color::White);
        currentState = DraculaState::BATTLE_IDLE;
        return true;
    }
    return true;
}

bool Dracula::disappearBody(float deltaTime){
    if(disappearTimeCounter < DISAPPEAR_TIME / 2.f){
        disappearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else if (disappearTimeCounter < DISAPPEAR_TIME){
        disappearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 0));
        currentState = DraculaState::BATTLE_AWAY;
        return true;
    }
    return true;
}

void Dracula::hello() const
{
    std::cout << "Soy Dracula" << std::endl;
}
