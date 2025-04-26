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

    gKilledBoss = false;

    // Waiting times
    waitingIdle = 0.5f;
    waitingInReady = 0.5f;
    waitingInSteady = 0.5f;
    waitingInReadyCounter = 0.f;
    waitingIdleCounter = 0.f;
    waitingInSteadyCounter = 0.f;

    waitingInFlyCounter = 0.f;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);
    animationManager->addAnimation(draculaSpiritIdle, this->idleDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritReady, this->readyDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritSteady, this->steadyDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritGo, this->goDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritFlying, this->flyingDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritLand, this->landDraculaSpiritFrames);
    animationManager->addAnimation(draculaSpiritFire, this->fireDraculaSpiritFrames);

    this->animationManager = animationManager;
    currentAnimation = draculaSpiritIdle;
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
                this->currentState = DraculeSpiritState::IDLE;
                break;
            }
        }
    }
    // MOVEMENT LOGIC
    if (isActive)
    {
        // MAQUINA DE ESTADOS DEL BOSS
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL

        // Facing logic of draculaSpirit looking at the player
        sf::Vector2f playerPos(player.sprite->getGlobalBounds().position.x + player.sprite->getGlobalBounds().size.x / 2, player.sprite->getGlobalBounds().position.y + player.sprite->getGlobalBounds().size.y / 2);
        sf::Vector2f draculaSpiritPos(sprite->getGlobalBounds().position.x + sprite->getGlobalBounds().size.x / 2, sprite->getGlobalBounds().position.y + sprite->getGlobalBounds().size.y / 2);
        float distance = playerPos.x - draculaSpiritPos.x;
        if (playerPos.x < draculaSpiritPos.x) {
            sprite->setScale(sf::Vector2f(1.f, 1.f)); // Negative X scale flips the sprite
            facingRight = -1;
        } 
        else {
            sprite->setScale(sf::Vector2f(-1.f, 1.f));
            facingRight = 1;
        }
        switch(currentState){
            case DraculeSpiritState::ASLEEP:
                if(!animationManager->isPlaying(noAnimation)){
                    animationManager->playAnimation(noAnimation);
                }
                break;

            case DraculeSpiritState::IDLE:
                if(!animationManager->isPlaying(draculaSpiritIdle)){
                    animationManager->playAnimation(draculaSpiritIdle);
                }
                this->flyDone = false;
                if (distance < 70 || waitingIdleCounter >= waitingIdle)
                {
                    // Randomly choose 
                    {
                        int chance = rand() % 3;
                        /*if (chance == 0)
                        {
                        }*/
                        
                    }
                    this->currentState = DraculeSpiritState::READY;
                    waitingIdleCounter = 0.f;
                }
                else{
                    waitingIdleCounter += deltaTime;
                }
                
                break;

            case DraculeSpiritState::READY:
                if(!animationManager->isPlaying(draculaSpiritReady)){
                    animationManager->playAnimation(draculaSpiritReady);
                }
                if (distance < 100)
                {
                    waitingInReadyCounter += deltaTime;
                    if (waitingInReadyCounter >= waitingInReady)
                    {
                        this->currentState = DraculeSpiritState::STEADY;
                        waitingInReadyCounter = 0.f;
                    }
                }
                else
                {
                    this->currentState = DraculeSpiritState::IDLE;
                    waitingInReadyCounter = 0.f;
                }
                
                break;

            case DraculeSpiritState::STEADY:
                if(!animationManager->isPlaying(draculaSpiritSteady)){
                    animationManager->playAnimation(draculaSpiritSteady);
                }
                if (distance < 100)
                {
                    waitingInSteadyCounter += deltaTime;
                    if (waitingInSteadyCounter >= waitingInSteady)
                    {
                        this->currentState = DraculeSpiritState::GO;
                        this->speed.y = -450.f * deltaTime; 
                        this->speed.x = 80.f * deltaTime; 
                        waitingInSteadyCounter = 0.f;
                        this->directionFlying = facingRight;
                    }
                }
                else
                {
                    this->currentState = DraculeSpiritState::READY;
                    waitingInSteadyCounter = 0.f;
                }
                break;

            case DraculeSpiritState::GO:
                if(!animationManager->isPlaying(draculaSpiritGo)){
                    animationManager->playAnimation(draculaSpiritGo);
                }
                {
                
                // Velocidad de caída (puedes ajustar estos valores)
                this->speed.y = this->speed.y + 980* deltaTime * deltaTime; // Velocidad vertical
                
                // Mueve el sprite

                if( mapBounds.position.x + 250 >= sprite->getPosition().x + this->directionFlying * this->speed.x &&
                    mapBounds.position.x + 32 <= sprite->getPosition().x + this->directionFlying * this->speed.x){
                        sprite->move(sf::Vector2f(this->directionFlying * this->speed.x, this->speed.y));
                }
                else{
                    this->speed.x = 0.f;
                    sprite->move(sf::Vector2f(this->directionFlying * this->speed.x, this->speed.y));
                }
                

                if (this->directionFlying != this->facingRight && !this->flyDone)
                {
                    this->flyDone = true;
                    this->currentState = DraculeSpiritState::FLYING;
                }
                
                // Verifica si ha alcanzado la altura del jugador (o el suelo)
                if(sprite->getPosition().y > 151.0f) {
                    sprite->move(sf::Vector2f(0.f, 1.0f));
                    this->currentState = DraculeSpiritState::LANDING;
                }
                }
                break;

            case DraculeSpiritState::FLYING:
                if(this->waitingInFlyCounter < waitingInFly){
                    this->waitingInFlyCounter += deltaTime;
                    if(!animationManager->isPlaying(draculaSpiritFlying)){
                        animationManager->playAnimation(draculaSpiritFlying);
                    }
                    sprite->move(sf::Vector2f(this->directionFlying * this->speed.x, 0.f));

                }
                else{
                    this->waitingInFlyCounter = 0.f;
                    this->currentState = DraculeSpiritState::GO;
                }
                
                
                
                
                break;

            case DraculeSpiritState::LANDING:
                if(!animationManager->isPlaying(draculaSpiritFlying)){
                    animationManager->playAnimation(draculaSpiritFlying);
                }
                this->currentState = DraculeSpiritState::IDLE;
                break;

            case DraculeSpiritState::FIRE:

                break;

            default:
                break;
        }
        sf::FloatRect spriteBounds = sprite->getGlobalBounds();
        hitboxes[0] = spriteBounds;
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
    /*sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }*/
}

void DraculaSpirit::hello() const
{
    std::cout << "Soy DraculaSpirit" << std::endl;
}