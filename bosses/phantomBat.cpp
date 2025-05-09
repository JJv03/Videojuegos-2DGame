#include "phantomBat.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
PhantomBat::PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = PBAT_SPEED;
    life = PBAT_LIFE;
    score = PBAT_SCORE;
    damage = PBAT_DAMAGE;

    gKilledBoss = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(sleepPhantomBat, this->idlePhBatFrames);
    animationManager->addAnimation(flyPhantomBat, this->flyPhBatFrames);

    this->animationManager = animationManager;
    currentAnimation = sleepPhantomBat;

    currentState = State::WAITING;

    weights[0] = 1;
    weights[1] = 1;
    weights[2] = 1;

    startingMove = false;
    startingAttack = false;
    startingEnhanced = false;
}

// Update phantomBat logic: handle spawning, movement, and deactivation
void PhantomBat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    if (!isActive && !dead)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                maxLife = life;
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {   
        // std::cout << "GOAL: " << goal.x << " " << goal.y << std::endl;
        // BOSS LIFE FOR THE GUI
        // MAQUINA DE ESTADOS DEL BOSS: PRIMERO ESTA UN TIEMPO QUIETO Y DESPUES EMPIZA A MOVERSE/ATACAR
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL
        // ESTA EN REPOSO HASTA QUE SE BLOQUEA LA OPCION DE IR HACIA ATRÁS QUE ES CUANDO SE EMPIEZA A MOVER
        position = sprite->getGlobalBounds().position;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        // Wait 2 seconds and then go to center
        timer += deltaTime;
        if(starting){            
            if (timer >= sleepInterval) {
                timer = 0.0f;
                starting = false;
                currentAnimation = flyPhantomBat;
                goal = sf::Vector2f(map.x + size.x / 2, map.y + size.y / 2);
                // std::cout << "Map pos: " << map.x << " " << map.y << std::endl;
                // std::cout << "Map size: " << size.x << " " << size.y << std::endl;
                // std::cout << "Goal tras dormir: " << goal.x << " " << goal.y << std::endl;
                getLinelSpeed(moveInterval);
            }
        }
        else{
            if(goingToCenter){
                if(timer >= moveInterval){
                    timer = 0.0f;
                    speed = sf::Vector2f(0, 0);
                    goingToCenter = false;
                    // std::cout << "Pos en el centro: " << position.x << " " << position.y << std::endl;
                }
            }
            else{
                auto mode = configManager.getDifficulty();

                if(triedAI && !enhancedActivated){
                    enhancedTimer += deltaTime;
                    if(enhancedTimer >= enhancedInterval){
                        enhancedTimer = 0;
                        triedAI = false;
                    }
                }

                switch (currentState)
                {
                    case State::ENHANCED:
                        std::cout << "ENHANCED" << std::endl;
                        if(!startingEnhanced){
                            // Scape depending on the direction of the player
                            if(playerDir >= 0){         // Left
                                goal = sf::Vector2f(mapDims.position.x + 55, mapDims.position.y + 25);
                            }
                            else if(playerDir < 0){   // Right
                                goal = sf::Vector2f(mapDims.position.x + mapDims.size.x - 55, mapDims.position.y + 25);
                            }
                            startingEnhanced = true;

                            enhancedTimer = 0.f;
                        }
                        enhancedTimer += deltaTime;
                        getLinelSpeed(enhancedSpeed);
                        if(enhancedTimer >= enhancedSpeed){
                            // Return to waiting
                            enhancedTimer = 0.f;
                            timer = 0.f;
                            doubleMoveTimer = 0.f;
                            speed = sf::Vector2f(0, 0);
                            enhancedActivated = false;
                            triedAI = false;
                            if(mode.hard_mode){
                                selectNewState();
                            }
                            else{
                                currentState = State::WAITING;
                            }
                        }
                        break;
                    
                    case State::WAITING:
                        std::cout << "WAITING" << std::endl;
                        enhancedAI(mode.hard_mode, playerBounds);
                        speed = sf::Vector2f(0, 0);
                        timer += deltaTime;
                        if (timer >= waitingInterval) {
                            // std::cout << "End Waiting" << std::endl;
                            if(mode.hard_mode){
                                selectNewState();
                            }
                            else{
                                startingMove = false;
                                currentState = State::MOVING;
                            }
                        }
                        break;
                    
                    case State::MOVING:
                        std::cout << "MOVING" << std::endl;
                        // Decide moverse hacia el lado opuesto
                        if(!startingMove){
                            goal = (position.x + 24 > map.x + size.x / 2)
                            ? sf::Vector2f(position.x - 50.f, position.y)
                            : sf::Vector2f(position.x + 50.f, position.y);
                            // std::cout << "GOAL: " << goal.x << " " << goal.y << std::endl;
                            getLinelSpeed(moveLeftRight);
                            startingMove = true;

                            timer = 0.f;
                        }
                        enhancedAI(mode.hard_mode, playerBounds);
                        if(timer >= moveLeftRight){
                            // std::cout << "End Moving left / right" << std::endl;
                            speed = sf::Vector2f(0, 0);
                            
                            if(mode.hard_mode){
                                selectNewState();
                            }
                            else{
                                startingAttack = false;
                                currentState = State::ATTACKING;
                            }
                        }
                        break;

                    case State::ATTACKING:
                        std::cout << "ATTACKING" << std::endl;
                        if(!startingAttack){
                            int chance = rand() % 4;    // WEIGHT FOR THIS TOO
                            if(chance == 0){ // 1/4
                                // Direct to the player
                                objectivePlayer(playerBounds);
                            }
                            else{   // 3/4
                                // Random position
                                randomObjective();
                            }
                            startingAttack = true;

                            timer = 0.f;
                        }
                        enhancedAI(mode.hard_mode, playerBounds);
                        if(timer >= moveInterval){
                            // std::cout << "End Ataccking" << std::endl;
                            speed = sf::Vector2f(0, 0);
                            if(mode.hard_mode){
                                selectNewState();
                            }
                            else{
                                currentState = State::WAITING;
                            }
                        }
                        else{
                            getDoubleSpeed();
                            doubleMoveTimer += deltaTime;
                        }
                        break;
                }
            }
        }

        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }
    }
    updateAnimation(deltaTime);
}

void PhantomBat::enhancedAI(bool isOn, const sf::FloatRect &playerBounds){
    sf::Vector2f batCenterPos(position.x + 12, position.y + 8);
    sf::Vector2f playerPos(playerBounds.position.x + playerBounds.size.x, playerBounds.position.y);
    float dx = batCenterPos.x - playerPos.x;
    float dy = batCenterPos.y - playerPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    // std::cout << "Distance: " << distance << std::endl;
    bool isClose = distance < 65;
    if(isOn && !triedAI && isClose && (gIsWhipBeingUsed || gIsSubWeaponBeingUsed)){
        triedAI = true;     // To prevent permatrying
        int chance = rand() % 3;
        if(chance == 0){ // Meter factor cercanía del jugador
            // std::cout << "Luckyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy" << std::endl;
            enhancedActivated = true;
            currentState = State::ENHANCED;
        }
    }
    // In case Enhanced AI mode activate add this possible state (it has a prob to happen, not always). If there's a weapon (whip or secundary)
    // the phantomBat is able to avoid the attack flying up
}

void PhantomBat::randomObjective(){
    const float limitWidth = 60.0f;     // 48 size
    const float limitHeight = 35.0f;    // 24 size

    doubleMoveTimer = 0.f;
    startPosition = position;

    float minX = mapDims.position.x + 15;
    float maxX = mapDims.position.x + mapDims.size.x - limitWidth;
    float minY = mapDims.position.y + 15;
    float maxY = mapDims.position.y + mapDims.size.y - limitHeight;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    
    goal = sf::Vector2f(distX(gen), distY(gen)); 
    // std::cout << "New goal: " << goal.x << " " << goal.y << std::endl;
}

void PhantomBat::objectivePlayer(const sf::FloatRect &playerBounds){
    doubleMoveTimer = 0.f;
    startPosition = position;
    goal = sf::Vector2f(playerBounds.position.x + 10, playerBounds.position.y - 10);
}

void PhantomBat::getLinelSpeed(float timeToMove){
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;

    speed = sf::Vector2f(deltaX / timeToMove, -deltaY / timeToMove);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void PhantomBat::getDoubleSpeed() {
    float t = doubleMoveTimer / moveInterval;
    if (t > 1.f) t = 1.f;

    // horizontal component: constant velocity
    float totalDx   = goal.x - startPosition.x;
    float xVel      = totalDx / moveInterval;

    // “linear” vertical component
    float totalDy   = goal.y - startPosition.y;
    float yVelLin   = totalDy / moveInterval;

    // Parabolic arch: 
    // f(t) = -4*H*(t-0.5)^2 + H => Extra vertical part
    // f'(t) = d/dt = -8*H*(t - 0.5)
    float arcDeriv = -8.f * arcHeight * (t - 0.5f);
    float yVelArc  = arcDeriv / moveInterval;

    float yVel = yVelLin + yVelArc;

    speed = sf::Vector2f(xVel, -yVel);
    // std::cout << "Double speed: " << speed.x << " " << speed.y << std::endl;
}

void PhantomBat::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        sf::Vector2f spriteCenter = position + sprite->getGlobalBounds().size / 2.f;
        game.particleSystem.spawnHitParticle(spriteCenter);
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        sf::Vector2f spriteCenter = position + sprite->getGlobalBounds().size / 2.f;
        game.particleSystem.spawnHitParticle(spriteCenter);
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    currentBossLife = life;
}

// Render phantomBat and debug info (spawn zone)
void PhantomBat::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }

    if(gDrawHitboxes){
        sf::RectangleShape goalMarker;
        goalMarker.setSize(sf::Vector2f(10.f, 10.f));
        goalMarker.setFillColor(sf::Color::Red);
        goalMarker.setPosition(goal);
        goalMarker.setOrigin(sf::Vector2f(5.f, 5.f));

        window.draw(goalMarker);
    }
}

// Update animation frame and flip sprite based on direction
void PhantomBat::updateAnimation(float deltaTime)
{
    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);
}

void PhantomBat::resetPosition()
{
    Boss::resetPosition();

    speed = PBAT_SPEED;
    life = PBAT_LIFE;

    currentAnimation = sleepPhantomBat;
    dead = false;
    starting = true;
    timer = 0.f;
    doubleMoveTimer = 0.f;
    goingToCenter = true;
    currentState = State::WAITING;

    weights[0] = 1;
    weights[1] = 1;
    weights[2] = 1;

    startingMove = false;
    startingAttack = false;
    startingEnhanced = false;
}

void PhantomBat::selectNewState(){
    updateWeights();

    startingMove = false;
    startingAttack = false;
    startingEnhanced = false;

    int totalWeight = weights[0] + weights[1] + weights[2];
    if (totalWeight == 0) return;

    int r = rand() % totalWeight;

    if(r < weights[0]){
        currentState = State::WAITING;
        std::cout << "Selected WAITING" << std::endl;
    } else if(r < weights[0] + weights[1]){
        currentState = State::MOVING;
        std::cout << "Selected MOVING" << std::endl;
    } else{
        currentState = State::ATTACKING;
        std::cout << "Selected ATTACKING" << std::endl;
    }

    timer = 0.f;
    speed = sf::Vector2f(0.f, 0.f);
}

void PhantomBat::updateWeights(){
    // Reset weights
    weights[0] = 1; // WAITING
    weights[1] = 1; // MOVING
    weights[2] = 1; // ATTACKING

    float lifeRatio = static_cast<float>(life) / static_cast<float>(PBAT_LIFE);

    // Vida baja: favorecer ATTACKING
    if (lifeRatio < 0.3f) {
        weights[2] += 2;
    }

    // Jugador usando armas: favorecer ataque y evasión
    if (gIsWhipBeingUsed || gIsSubWeaponBeingUsed) {
        weights[2] += 2; // ATTACKING
    }

    // Aleatoriedad: a veces eliminar WAITING
    if (rand() % 3 == 0) {
        weights[0] = 0;
    }

    // Penalizar repetir el mismo estado
    if (weights[static_cast<int>(currentState)] > 0) {
        weights[static_cast<int>(currentState)] -= 1;
    }

    // Asegurarse de que al menos un estado tenga peso > 0
    int total = weights[0] + weights[1] + weights[2];
    if (total == 0) {
        weights[0] = 1; // WAITING por defecto
    }
    std::cout << "Weight WAITING: " << weights[0] << std::endl;
    std::cout << "Weight MOVING: " << weights[1] << std::endl;
    std::cout << "Weight ATTACKING: " << weights[2] << std::endl;
}

void PhantomBat::hello() const
{
    std::cout << "Soy PhantomBat" << std::endl;
}
