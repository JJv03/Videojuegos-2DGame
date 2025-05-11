#include "mummyMan.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize mummyMan with sprite, hitboxes, position, and game level/stage
MummyMan::MummyMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = MUMMY_SPEED;
    life = MUMMY_LIFE;
    score = MUMMY_SCORE;
    damage = MUMMY_DAMAGE;

    gKilledBoss = false;
    //isActive = true;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);
    animationManager->addAnimation(mummyIdle, this->idleMummyFrames);
    animationManager->addAnimation(mummyWalk, this->walkMummyFrames);
    animationManager->addAnimation(mummyAttack, this->attackkMummyFrames,false);

    this->animationManager = animationManager;
    if (position.x < 1500.f)
    {
        sprite->setScale(sf::Vector2f(1.f, 1.f));
        facingRight = -1;
    }
    else{
        sprite->setScale(sf::Vector2f(-1.f, 1.f));
        facingRight = 1;
    }
    
    currentAnimation = mummyIdle;
}

// Update phantomBat logic: handle spawning, movement, and deactivation
void MummyMan::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
    const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds,const Player &player)
{
    // SPAWN LOGIC
    if (!isActive && !dead)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                life = MUMMY_LIFE;
                maxLife = currentMummy1Life + currentMummy2Life;
                this->currentState = MummyState::IDLE;
                break;
            }
        }
    }


    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {
        sf::Vector2f playerPos(player.sprite->getGlobalBounds().position.x + player.sprite->getGlobalBounds().size.x / 2, player.sprite->getGlobalBounds().position.y + player.sprite->getGlobalBounds().size.y / 2);
        position.x = sprite->getGlobalBounds().position.x + sprite->getGlobalBounds().size.x / 2;
        position.y = sprite->getGlobalBounds().position.y + sprite->getGlobalBounds().size.y / 2;
        
        sf::Vector2f mummyPos = sprite->getPosition();
        sf::FloatRect mummyBounds = sprite->getGlobalBounds();
        float horizontalDistance = std::hypot(position.x - playerPos.x, position.y - playerPos.y);

        playerClose = horizontalDistance < 70.f;
        playerAway = horizontalDistance > 120.f;
        // Verificar límites del mapa
        bool atLeftEdge = mummyBounds.position.x  <= mapDims.position.x;
        bool atRightEdge = mummyBounds.position.x + mummyBounds.size.x >= mapBounds.position.x + mapBounds.size.x;
    
        auto mode = configManager.getDifficulty();
        // FSM
        switch (currentState)
        {
            case MummyState::WALK:
                currentAnimation = mummyWalk;
                
                // Cambio de dirección cuando llega a los bordes
                if (atLeftEdge || atRightEdge)
                {
                    facingRight = atLeftEdge ? 1.f : -1.f;
                    directionChanged = true;
                }

                // Cambio de dirección basado en la posición del jugador (sólo si no está en un borde)
                if (!atLeftEdge && !atRightEdge && 
                    ((playerPos.x < mummyPos.x && facingRight > 0) || 
                    (playerPos.x > mummyPos.x && facingRight < 0)))
                {
                    // Sólo cambiar dirección si el jugador está a cierta distancia
                    if (abs(playerPos.x - mummyPos.x) > 50.f)
                    {
                        facingRight = (playerPos.x < mummyPos.x) ? -1.f : 1.f;
                        directionChanged = true;
                    }
                }

                // Aplicar cambio de dirección
                if (directionChanged)
                {
                    sprite->setScale(sf::Vector2f(static_cast<float>(-facingRight), 1.f));
                    sprite->move(sf::Vector2f(facingRight * 16.f, 0.f));
                    directionChanged = false;
                }
                if (!(atLeftEdge && facingRight < 0) && !(atRightEdge && facingRight > 0))
                {
                    sprite->move(sf::Vector2f(facingRight * deltaTime * speed.x, 0.f));
                }

                if(mode.hard_mode){
                            selectNewState();
                        }
                else{
                    actionTimer += deltaTime;
                    if (!lanzado && actionTimer >= nextActionTime && ((playerPos.x < mummyPos.x && facingRight == -1.f) ||( playerPos.x > mummyPos.x && facingRight == 1.f)) )
                    {
                        currentState = MummyState::ATTACK;
                        nextActionTime = 1.f + static_cast<float>(rand() % 1000) / 500.f;
                        actionTimer = 0.f;
                        
                    }
                }
                break;

            case MummyState::ATTACK:
                currentAnimation = mummyAttack;
                if (animationManager->isAnimationFinished())
                {
                    
                    
                    sf::Vector2f position = sprite->getGlobalBounds().position;
                    position.y += 10.f;
                    bandage = createBandage(position, mapDims, MUMMY_DAMAGE);
                    bandage->setActive(true);
                    bandage->sprite->setScale(sf::Vector2f(static_cast<float>(-facingRight), 1.f));
                    bandage->velocity.x = 60.f * facingRight;
                    bandage->velocity.y = 85.f;
                    lanzado = true;
                    if(mode.hard_mode){
                            selectNewState();
                        }
                    else{
                        //speed.x = MUMMY_SPEED.x * (static_cast<float>((rand() % 10) + 1) / 10.f);
                        currentState = MummyState::WALK;
                    }
                    
                }
                break;

            default: // IDLE
                if (counterStarting >= time2Start)
                {
                    currentState = MummyState::WALK;
                    counterStarting = 0.f;
                    started = true;
                    firstTime = true;
                    attackWaitingCounter = 0.f;
                }
                else
                {
                    counterStarting += deltaTime;
                }
                break;
        }

        if (bandage && bandage->sprite && bandage->getActive())
        {
            bandage->update(deltaTime, mapBounds);
        }
        else
        {
            lanzado = false;
        }
        
    }

    sf::FloatRect spriteBounds = sprite->getGlobalBounds();
    hitboxes[0] = spriteBounds;
    
    updateAnimation(deltaTime);
}




void MummyMan::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite || currentState ==MummyState::IDLE ||( currentState ==MummyState::ATTACK && !configManager.getDifficulty().hard_mode) )
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            
            dead = true;
            gKilledBoss = true;
            if(bandage){
                bandage->reset();
            }
            if(isOtherDead){
                game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
                game.particleSystem.spawnBigFireParticle(position, false);
            }
            
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            dead = true;
            gKilledBoss = true;
            if(bandage){
                bandage->reset();
            }
            
            if(isOtherDead){
                game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
                game.particleSystem.spawnBigFireParticle(position, false);
            }
            
        }

    }
    if(id==1){
            currentMummy1Life = life;
        }else{
            currentMummy2Life = life;
        }
        currentBossLife = currentMummy1Life + currentMummy2Life;
    
}

// Render phantomBat and debug info (spawn zone)
void MummyMan::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
    if (bandage && bandage->sprite && bandage->getActive())
    {
        bandage->draw(window);
    }
    
}

// Update animation frame and flip sprite based on direction
void MummyMan::updateAnimation(float deltaTime)
{
    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);
}

void MummyMan::resetPosition()
{
    Boss::resetPosition();

    speed = MUMMY_SPEED;
    life = MUMMY_LIFE;
    if(bandage){
        bandage->setActive(false);
    }
    
    currentAnimation = mummyIdle;
    dead = false;
    starting = true;
    attacking = false;
    weights[0] = 1;
    weights[1] = 1;
    currentMummy1Life  = MUMMY_LIFE;
    currentMummy2Life = MUMMY_LIFE;
    playerClose = false;
    playerAway = false;
    nextActionTime = 1.f + static_cast<float>(rand() % 1000) / 500.f;
}


void MummyMan::selectNewState(){
    updateWeights();

    //startingMove = false;

    int totalWeight = weights[0] + weights[1];
    if (totalWeight == 0) return;

    int r = rand() % totalWeight;

    if (r < weights[0] && !playerAway) {
        this->currentState = MummyState::WALK;
        std::cout << "Selected WALK" << std::endl;
    } 
    else if(!lanzado){                     

        currentState = MummyState::ATTACK;
        std::cout << "Selected ATTACKING" << std::endl;
    }
    else{
        this->currentState = MummyState::WALK;
        std::cout << "Selected WALK" << std::endl;
    }

    //timer = 0.f;
    //speed = sf::Vector2f(0.f, 0.f);
}

void MummyMan::updateWeights() {
    // Reset base weights
    weights[0] = 1; // MOVING
    weights[1] = 1; // ATTACKING

    float lifeRatio = static_cast<float>(life) / static_cast<float>(MUMMY_LIFE);

    // If life is low, prioritize moving to escape
    if (lifeRatio < 0.35f) {
        //weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[0] += 2; // More likely to flee
        actionTimer = 0.f;
        speed.x = 1.2f * MUMMY_SPEED.x;
        nextActionTime = 0.5f + static_cast<float>(rand() % 1000) / 500.f; // entre 0.5 y 1.5 seg
    }
    else{
        speed.x = MUMMY_SPEED.x;
        nextActionTime = 1.f + static_cast<float>(rand() % 1000) / 500.f;
        
        
    }

    // If the player is attacking, avoid waiting for so long
    if (gIsWhipBeingUsed || gIsSubWeaponBeingUsed) {
        //weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[0] += 1; // Move more to dodge
        if(lifeRatio >= 0.35f){
            nextActionTime = 0.5f + static_cast<float>(rand() % 1000) / 500.f;
            actionTimer = 0.f;
            speed.x = 1.2f * MUMMY_SPEED.x;
            // paralizeCounter = 0.7f;
        }
        
        
    }
    

    // Distance to the player
    if (playerClose) {
        weights[0] += 2; // Try to jump to the player
        if(lifeRatio >= 0.35f){
            speed.x = 1.2f * MUMMY_SPEED.x;
            attackWaitingTime = 3.5f;
        }
        
    } else if (playerAway) {
        std::cout << "Away" << std::endl;
        attackWaitingTime = 1.5f;
        speed.x = MUMMY_SPEED.x;
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


void MummyMan::hello() const
{
    std::cout << "Soy MummyMan" << std::endl;
}
