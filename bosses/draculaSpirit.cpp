#include "draculaSpirit.h"
#include "../game.h"
#include "../player.h"
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
    reproduced = false;

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
                life = DRACULASPIRIT_LIFE;
                maxLife = life;
                
                if(!reproduced){
                    reproduced = true;
                    this->currentState = DraculeSpiritState::IDLE;
                    respawned = true;
                    gameSoundManager.stopAllMusic();
                    auto audio = configManager.getAudio();
                    gameSoundManager.playMusicSequence("dracula2.1", "dracula2.2", true, gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
                
                }
                 break;
            }
        }
    }
    // MOVEMENT LOGIC
    if (isActive)
    {
        if(respawned){
            respawned = false;
            float ejeX[2] = {200.f, -200.f};
                    float ejeY[3] = {0.f, 65.f, -65.f};
                    int counter = 0;
                    for(auto& projectil : projectiles){
                        float  vx = ejeX[counter%2];
                        float vy = ejeY[counter%3];
                        counter++;
                        projectil = createProjectile(
                            {   sprite->getPosition().x - 8.f,
                                sprite->getPosition().y - 26.f},
                            {vx, vy},
                            ProjectileType::ROCK,
                            1.f);
                        projectil->sprite->setScale({1.f, 1.f});
                        projectil->setActive(true);
                    }
        }
        // MAQUINA DE ESTADOS DEL BOSS
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL

        // Facing logic of draculaSpirit looking at the player
        sf::Vector2f playerPos(player.sprite->getGlobalBounds().position.x + player.sprite->getGlobalBounds().size.x / 2, player.sprite->getGlobalBounds().position.y + player.sprite->getGlobalBounds().size.y / 2);
        position.x = sprite->getGlobalBounds().position.x + sprite->getGlobalBounds().size.x / 2;
        position.y = sprite->getGlobalBounds().position.y + sprite->getGlobalBounds().size.y / 2;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        //float distance = playerPos.x - draculaSpiritPos.x;
        if (playerPos.x < position.x) {
            sprite->setScale(sf::Vector2f(1.f, 1.f)); // Negative X scale flips the sprite
            facingRight = -1;
        } 
        else {
            sprite->setScale(sf::Vector2f(-1.f, 1.f));
            facingRight = 1;
        }
        float horizontalDistance = std::hypot(position.x - playerPos.x, position.y - playerPos.y);

        playerClose = horizontalDistance < 70.f;
        playerAway = horizontalDistance > 120.f;
        if(hasFired){
            fireNextCounter += deltaTime;
            if(fireNextCounter >= fireNextTimer){
                fireNextCounter = 0.f;
                hasFired = false;
            }
        }

        int chance = rand() % 3;
        int chanceFlying = rand() % 2;
        auto mode = configManager.getDifficulty();
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
                if(paralizeCounter>=paralizeTimer){
                    collisionWithHolyWater = false;
                    this->flyDone = false;
                    alreadyChanged=false;
                    alreadyFlying = false;
                    if (waitingIdleCounter >= waitingIdle)
                    {
                        if(mode.hard_mode){
                            selectNewState();
                        }
                        else{
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
                        }

                          
                        
                        
                        waitingIdleCounter = 0.f;
                    }
                    else{
                        waitingIdleCounter += deltaTime;
                }
                }
                else{
                    paralizeCounter += deltaTime;
                }
                
                
                break;

            case DraculeSpiritState::READY:
                //std::cout << "ready" << std::endl;
                if(collisionWithHolyWater){
                    this->currentState = DraculeSpiritState::IDLE;
                }
                else{
                    if(!animationManager->isPlaying(draculaSpiritReady)){
                        animationManager->playAnimation(draculaSpiritReady);
                    }
                    if (waitingInReadyCounter >= waitingInReady)
                        {

                            if (!hasFired && mode.hard_mode)
                            {
                                fireCounter = 0.f;
                                sprite->move({0.f,-16.f});
                                this->currentState = DraculeSpiritState::FIRE;
                            }
                            else{
                                waitingInSteadyCounter = 0.f;
                                this->currentState = DraculeSpiritState::STEADY;
                                waitingInReadyCounter = 0.f;
                            } 
                            
                        }
                    else{
                        waitingInReadyCounter += deltaTime;
                    }
                }
                
                /*else
                {
                    this->currentState = DraculeSpiritState::IDLE;
                    waitingInReadyCounter = 0.f;
                }*/
                
                break;

            case DraculeSpiritState::STEADY:
                //std::cout << "steady" << std::endl;
                if(collisionWithHolyWater){
                    this->currentState = DraculeSpiritState::IDLE;
                }
                else{
                    if(!animationManager->isPlaying(draculaSpiritSteady)){
                        animationManager->playAnimation(draculaSpiritSteady);
                    }
                    
                    if (waitingInSteadyCounter >= waitingInSteady)
                    {
                        this->currentState = DraculeSpiritState::GO;
                        this->speed.y = -450.f * deltaTime; 
                        this->speed.x = 80.f * deltaTime; 
                        
                        waitingInSteadyCounter = 0.f;
                        this->directionFlying = facingRight;
                        if(!(map.x + size.x -39.f > position.x + this->directionFlying * this->speed.x)){
                             outOfBoder = true;
                             this->directionFlying = -1.f;
                        } else if(!(map.x + 34.f  < position.x + this->directionFlying * this->speed.x)) {
                               
                            outOfBoder = true;
                            this->directionFlying = 1.f;
                        }
                    }
                    else{
                        waitingInSteadyCounter += deltaTime;
                    }
                }
                
                /*else
                {
                    this->currentState = DraculeSpiritState::READY;
                    waitingInSteadyCounter = 0.f;
                }*/
                break;

            case DraculeSpiritState::GO:
                
                if(!animationManager->isPlaying(draculaSpiritGo)){
                    animationManager->playAnimation(draculaSpiritGo);
                }
                {
                this->speed.y = this->speed.y + 1180* deltaTime * deltaTime; // Velocidad vertical
                
                
                    // Mueve el sprite
                    
                    
                    alreadyFlying = true;
                    
                    if( (map.x + size.x -39.f > position.x + this->directionFlying * this->speed.x &&
                        map.x + 34.f  < position.x + this->directionFlying * this->speed.x) || !hasInitJump || outOfBoder){
                            sprite->move(sf::Vector2f(this->directionFlying * this->speed.x, this->speed.y));
                    }
                    else{

                        this->speed.x=0.f;
                        sprite->move(sf::Vector2f(this->directionFlying * this->speed.x, this->speed.y));

                        //this->directionFlying ?
                    }
                
                // Velocidad de caída (puedes ajustar estos valores)
                
                
                
                

                if (this->directionFlying != this->facingRight && !this->flyDone)
                {
                    this->flyDone = true;
                    firstTimeDetected = false;
                    this->currentState = DraculeSpiritState::FLYING;
                }
                
                // Verifica si ha alcanzado la altura del jugador (o el suelo)
                if(sprite->getPosition().y >= 155.0f) {
                    
                    if(!hasInitJump){
                        hasInitJump=true;
                    }
                    outOfBoder = false;
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
                    
                    
                    if(mode.hard_mode && !hasFired && chanceFlying==0 && !firstTimeDetected){
                        hasFired = true;               
                
                        // Create and store the projectile using the generic creator
                        projectile = createProjectile(
                            {   sprite->getPosition().x + (facingRight * 5.0f),
                                sprite->getPosition().y - 27.0f},
                            {-30.f, 100.0f},
                            ProjectileType::FISHMAN,
                            1.f);
        
                        projectile->setActive(true);
        
                        hasProjectile = true;
        
                        projectile2 = createProjectile(
                            {   sprite->getPosition().x + (facingRight * 5.0f),
                                sprite->getPosition().y - 27.0f},
                            {10.f, 100.0f},
                            ProjectileType::FISHMAN,
                            1.f);
        
                        projectile2->setActive(true);
        
                        hasProjectile2 = true;
        
        
                        projectile3 = createProjectile(
                            {   sprite->getPosition().x + (facingRight * 5.0f),
                                sprite->getPosition().y - 27.0f},
                            {50.f, 100.0f},
                            ProjectileType::FISHMAN,
                            1.f);
        
                        projectile3->setActive(true);
        
                        hasProjectile3 = true;
                        sprite->move({0.f,16.f});
                    }
                }
                else{
                    this->waitingInFlyCounter = 0.f;
                    this->currentState = DraculeSpiritState::GO;
                }
                if(!firstTimeDetected){
                    firstTimeDetected = true;
                }
                
                
                
                
                break;

            case DraculeSpiritState::LANDING:
                //std::cout << "land" << std::endl;
                if(!animationManager->isPlaying(draculaSpiritLand)){
                    animationManager->playAnimation(draculaSpiritLand);
                }
                gameSoundManager.playSound("door", gameSoundManager.realVolume(configManager::getInstance().getAudio().master_volume, configManager::getInstance().getAudio().sound_volume));
                
                if(mode.hard_mode){
                    selectNewState();
                }
                else{
                    this->currentState = DraculeSpiritState::IDLE;
                }
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
                            {speedFire * facingRight, -1.f *speedFire/4.f},
                            ProjectileType::FISHMAN,
                            3.f);
        
                        projectile->setActive(true);
        
                        hasProjectile = true;
        
                        projectile2 = createProjectile(
                            {   sprite->getPosition().x + (facingRight * 5.0f),
                                sprite->getPosition().y - 27.0f},
                            {speedFire * facingRight, speedFire/8.f},
                            ProjectileType::FISHMAN,
                            3.f);
        
                        projectile2->setActive(true);
        
                        hasProjectile2 = true;
        
        
                        projectile3 = createProjectile(
                            {   sprite->getPosition().x + (facingRight * 5.0f),
                                sprite->getPosition().y - 27.0f},
                            {speedFire * facingRight, speedFire/2.f},
                            ProjectileType::FISHMAN,
                            3.f);
        
                        projectile3->setActive(true);
        
                        hasProjectile3 = true;
                        sprite->move({0.f,16.f});
                        
                        if(mode.hard_mode){
                            selectNewState();
                        }
                        else{
                            this->currentState = DraculeSpiritState::IDLE;
                        }
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

        for(auto& projectil : projectiles){
            if(projectil && projectil->sprite && projectil->getActive()){
                projectil->update(deltaTime, mapBounds);
            }
        }
        updateAnimation(deltaTime);
        currentBossLife = life;
    }
}

void DraculaSpirit::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
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
            hasInitJump = false;
            game.particleSystem.spawnReallyBigFireParticle(position, false);
            
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
            hasInitJump = false;
            game.particleSystem.spawnReallyBigFireParticle(position, false);
        }
        if (subWeapon->type == ItemType::FIRE_BOMB)
        {
            collisionWithHolyWater = true;
            paralizeCounter = 0.f;
        }
        else{
            collisionWithHolyWater = false;
        }
        
        
    }
    currentBossLife = life;
    if(life <= 0.f) currentState = DraculeSpiritState::ASLEEP;
}

// Render draculaSpirit
void DraculaSpirit::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
    if (projectile && projectile->getActive() && isActive )
    {
        //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
        projectile->draw(window);
    }
    if (projectile2 && projectile2->getActive() && isActive)
    {
        //std::cout << "P2 " << projectile2->sprite->getPosition().x  << "  " << projectile2->sprite->getPosition().y << std::endl;
        projectile2->draw(window);
    }
    if (projectile3 && projectile3->getActive() && isActive)
    {
        //std::cout << "P3 " << projectile3->sprite->getPosition().x  << "  " << projectile3->sprite->getPosition().y << std::endl;
        projectile3->draw(window);
    }
    for(auto& projectil : projectiles){
        if (projectil && projectil->sprite && projectil->getActive())
        {
            //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
            projectil->draw(window);
        }
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


void DraculaSpirit::resetPosition()
{
    //Boss::resetPosition();

    life = DRACULASPIRIT_LIFE;
    reproduced = false;
    weights[0] = 1;
    weights[1] = 1;

    playerClose = false;
    playerAway = false;
    waitingIdle = 0.5f;
    waitingInReady = 0.5f;
    waitingInSteady = 0.5f;
    speedFire = 80.f;
    fireTimer = 1.4f;
}

void DraculaSpirit::selectNewState(){
    updateWeights();

    //startingMove = false;

    int totalWeight = weights[0] + weights[1];
    if (totalWeight == 0) return;

    int r = rand() % totalWeight;

    if (r < weights[0]) {
        waitingInReadyCounter = 0.f;
        this->currentState = DraculeSpiritState::READY;
        std::cout << "Selected READY" << std::endl;
    } 
    else if(!hasFired){
        
                            
        fireCounter = 0.f;

        sprite->move({0.f,-16.f});
        currentState = DraculeSpiritState::FIRE;
        std::cout << "Selected ATTACKING" << std::endl;
    }
    else{
        waitingInReadyCounter = 0.f;
        this->currentState = DraculeSpiritState::READY;
        std::cout << "Selected READY" << std::endl;
    }

    //timer = 0.f;
    //speed = sf::Vector2f(0.f, 0.f);
}

void DraculaSpirit::updateWeights() {
    // Reset base weights
    weights[0] = 1; // MOVING
    weights[1] = 1; // ATTACKING

    float lifeRatio = static_cast<float>(life) / static_cast<float>(DRACULASPIRIT_LIFE);

    // If life is low, prioritize moving to escape
    if (lifeRatio < 0.35f) {
        //weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[0] += 2; // More likely to flee
        waitingIdle = 0.25f;
        waitingInReady = 0.25f;
        waitingInSteady = 0.25f;
        fireTimer = 0.6f;
        speedFire = 120.f;
        if(lifeRatio < 0.15f){
            weights[0] += 5; // More likely to flee
            waitingIdle = 0.1f;
            waitingInReady = 0.1f;
            waitingInSteady = 0.1f;
        }
    }
    else{
        waitingIdle = 0.5f;
        waitingInReady = 0.5f;
        waitingInSteady = 0.5f;
        
        
    }

    // If the player is attacking, avoid waiting for so long
    if (gIsWhipBeingUsed || gIsSubWeaponBeingUsed) {
        //weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[0] += 1; // Move more to dodge
        if(lifeRatio >= 0.35f){
            waitingIdle = 0.25f;
            waitingInReady = 0.25f;
            waitingInSteady = 0.25f;
            speedFire = 120.f;
            fireTimer = 0.6f;
            // paralizeCounter = 0.7f;
        }
        
        
    }
    

    // Distance to the player
    if (playerClose) {
        weights[0] += 2; // Try to jump to the player
        if(lifeRatio >= 0.35f){
            speedFire = 80.f;
            fireTimer = 1.4f;
        }
        
    } else if (playerAway) {
        std::cout << "Away" << std::endl;
        speedFire = 120.f;
        fireTimer = 0.6f;
        weights[1] += 5; // Try yo fire
    }

    // Penalize repeating the same state
    int currentIdx = static_cast<int>(currentState);
    if (weights[currentIdx] > 1) {
        weights[currentIdx] -= 1;
    }

    // Rebalancing weights if all are zero
    int total = weights[0] + weights[1];
    if (total == 0) {
        weights[0] = 1; // MOVING by default if all else fails
    }

    std::cout << "Weight MOVING: " << weights[0] << std::endl;
    std::cout << "Weight ATTACKING: " << weights[1] << std::endl;
}

void DraculaSpirit::hello() const
{
    std::cout << "Soy DraculaSpirit" << std::endl;
}