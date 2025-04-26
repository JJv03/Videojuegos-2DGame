#include "draculaSpirit.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize draculaSpirit with sprite, hitboxes, position, and game level/stage
DraculaSpirit::DraculaSpirit(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                             const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage),
                             position(position),mapDims(mapDims)
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

    hasFired = true;

    fireCounter = 0.f;

    sprite->move({0.f,-4.f});
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
void DraculaSpirit::update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds){
    // SPAWN LOGIC
    if (!isActive && phase == 2)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (player.gPlayerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                //maxLife = life;
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
        position = sprite->getGlobalBounds().position;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        float distance = playerPos.x - draculaSpiritPos.x;
        if (playerPos.x < draculaSpiritPos.x) {
            sprite->setScale(sf::Vector2f(1.f, 1.f)); // Negative X scale flips the sprite
            facingRight = -1;
        } 
        else {
            sprite->setScale(sf::Vector2f(-1.f, 1.f));
            facingRight = 1;
        }

        if(hasFired){
            fireNextCounter += deltaTime;
            if(fireNextCounter >= fireNextTimer){
                fireNextCounter = 0.f;
                hasFired = false;
            }
        }

        int chance = rand() % 5;
        switch(currentState){
            case DraculeSpiritState::ASLEEP:
                //std::cout << "sleep" << std::endl;
                if(!animationManager->isPlaying(noAnimation)){
                    animationManager->playAnimation(noAnimation);
                }
                break;

            case DraculeSpiritState::IDLE:
                //std::cout << "idle" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritIdle)){
                    animationManager->playAnimation(draculaSpiritIdle);
                }
                this->flyDone = false;
                if (waitingIdleCounter >= waitingIdle)
                {
                    if (chance == 0 && !hasFired )
                    {
                        fireCounter = 0.f;
                        sprite->move({0.f,-16.f});
                        this->currentState = DraculeSpiritState::FIRE;
                    }
                    else{
                        waitingInReadyCounter = 0.f;
                        this->currentState = DraculeSpiritState::READY;
                    }   
                    
                    
                    waitingIdleCounter = 0.f;
                }
                else{
                    waitingIdleCounter += deltaTime;
                }
                
                break;

            case DraculeSpiritState::READY:
                //std::cout << "ready" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritReady)){
                    animationManager->playAnimation(draculaSpiritReady);
                }
                if (waitingInReadyCounter >= waitingInReady)
                    {
                        waitingInSteadyCounter = 0.f;
                        this->currentState = DraculeSpiritState::STEADY;
                        waitingInReadyCounter = 0.f;
                    }
                    else{
                        waitingInReadyCounter += deltaTime;
                    }
                /*else
                {
                    this->currentState = DraculeSpiritState::IDLE;
                    waitingInReadyCounter = 0.f;
                }*/
                
                break;

            case DraculeSpiritState::STEADY:
                //std::cout << "steady" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritSteady)){
                    animationManager->playAnimation(draculaSpiritSteady);
                }
                
                if (waitingInSteadyCounter >= waitingInSteady)
                {
                    this->currentState = DraculeSpiritState::GO;
                    this->speed.y = -350.f * deltaTime; 
                    this->speed.x = 80.f * deltaTime; 
                    
                    waitingInSteadyCounter = 0.f;
                    this->directionFlying = facingRight;
                }
                else{
                    waitingInSteadyCounter += deltaTime;
                }
                /*else
                {
                    this->currentState = DraculeSpiritState::READY;
                    waitingInSteadyCounter = 0.f;
                }*/
                break;

            case DraculeSpiritState::GO:
                
                //std::cout << "go" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritGo)){
                    animationManager->playAnimation(draculaSpiritGo);
                }
                {
                
                // Velocidad de caída (puedes ajustar estos valores)
                this->speed.y = this->speed.y + 980* deltaTime * deltaTime; // Velocidad vertical
                
                // Mueve el sprite

                if( map.x + size.x >= position.x + this->directionFlying * this->speed.x &&
                    map.x  <= position.x + this->directionFlying * this->speed.x){
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
                if(sprite->getPosition().y > 152.0f) {
                    sprite->move(sf::Vector2f(0.f, 1.5f));
                    this->currentState = DraculeSpiritState::LANDING;
                }
                }
                break;

            case DraculeSpiritState::FLYING:
                //std::cout << "fkly" << std::endl;
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
                //std::cout << "land" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritLand)){
                    animationManager->playAnimation(draculaSpiritLand);
                }
                this->currentState = DraculeSpiritState::IDLE;
                break;

            case DraculeSpiritState::FIRE:
                //std::cout << "fire" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritFire)){
                    animationManager->playAnimation(draculaSpiritFire);
                }
                if (fireCounter< fireTimer)
                {
                    
                    fireCounter += deltaTime;
                }
                else{
                    
                    hasFired = true;               
                
                    // Create and store the projectile using the generic creator
                    projectile = createProjectile(
                        {   sprite->getPosition().x + (facingRight * 5.0f),
                            sprite->getPosition().y - 27.0f},
                        {80.f * facingRight, -10.0f},
                        ProjectileType::FISHMAN,
                        1.f);
    
                    projectile->setActive(true);
    
                    hasProjectile = true;
    
                    projectile2 = createProjectile(
                        {   sprite->getPosition().x + (facingRight * 5.0f),
                            sprite->getPosition().y - 27.0f},
                        {80.f * facingRight, 10.0f},
                        ProjectileType::FISHMAN,
                        1.f);
    
                    projectile2->setActive(true);
    
                    hasProjectile2 = true;
    
    
                    projectile3 = createProjectile(
                        {   sprite->getPosition().x + (facingRight * 5.0f),
                            sprite->getPosition().y - 27.0f},
                        {80.f * facingRight, 30.0f},
                        ProjectileType::FISHMAN,
                        1.f);
    
                    projectile3->setActive(true);
    
                    hasProjectile3 = true;
                    sprite->move({0.f,16.f});
                    this->currentState = DraculeSpiritState::IDLE;
                }
                
                
               
                break;

            default:
                break;
        }
        sf::FloatRect spriteBounds = sprite->getGlobalBounds();
        hitboxes[0] = spriteBounds;
        
        if (projectile && projectile->getActive())
        {
            projectile->update(deltaTime, mapDims);
        }
        if (projectile2 && projectile2->getActive())
        {
            projectile2->update(deltaTime, mapDims);
        }
        if (projectile3 && projectile3->getActive())
        {
            projectile3->update(deltaTime, mapDims);
        }
        updateAnimation(deltaTime);
    }
}

void DraculaSpirit::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage((whip->whipDmg), game.player))
        {
            // DROP
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            //dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
            
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            // DROP
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            //dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    currentBossLife = life;
}

// Render draculaSpirit
void DraculaSpirit::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
    if (projectile && projectile->getActive())
    {
        //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
        projectile->draw(window);
    }
    if (projectile2 && projectile2->getActive())
    {
        //std::cout << "P2 " << projectile2->sprite->getPosition().x  << "  " << projectile2->sprite->getPosition().y << std::endl;
        projectile2->draw(window);
    }
    if (projectile3 && projectile3->getActive())
    {
        //std::cout << "P3 " << projectile3->sprite->getPosition().x  << "  " << projectile3->sprite->getPosition().y << std::endl;
        projectile3->draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void DraculaSpirit::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite)
        return;

    animationManager->update(deltaTime);
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