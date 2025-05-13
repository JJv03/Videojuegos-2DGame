#include "death.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
Death::Death(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = DEATH_SPEED;
    life = DEATH_LIFE;
    score = DEATH_SCORE;
    damage = DEATH_DAMAGE;

    gKilledBoss = false;
    goingDown = true;

    up = true;
    right = false;
    dead = false;

    starting = true;
    timer = 0.f;
    doubleMoveTimer = 0.f;

    triedAI = false;

    currentState = State::ATTACKING;

    weights[0] = 1;
    weights[1] = 1;
    weights[2] = 1;

    damageReceived[0] = 0;
    damageReceived[1] = 0;
    damageReceived[2] = 0;

    startingMove = false;
    playerClose = false;
    playerAway = false;
    collisionWithHolyWater = false;
    paralizeCounter = 0.7f;
}

// Update Death logic: handle spawning, movement, and deactivation
void Death::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
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
        position = sprite->getGlobalBounds().position;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        // sf::Vector2f map = mapDims.position;
        // sf::Vector2f size = mapDims.size;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;

        if(starting){
            timer += deltaTime;
            if (timer >= sleepInterval) {
                timer = 0.0f;
                starting = false;
                goingDown = true;
                goal = sf::Vector2f(210, 46);
                // std::cout << "Map pos: " << map.x << " " << map.y << std::endl;
                // std::cout << "Map size: " << size.x << " " << size.y << std::endl;
                // std::cout << "Goal tras dormir: " << goal.x << " " << goal.y << std::endl;
                getLinelSpeed(moveInterval);
            }
        }
        else{
            if(goingDown){
                timer += deltaTime;
                if (timer >= moveInterval) {    // Arrived to the goal
                    timer = 0;
                    speed = sf::Vector2f(0, 0);
                    goingDown = false;
                }
            }
            else{
                auto mode = configManager.getDifficulty();

                float horizontalDistance = std::hypot(position.x - playerBounds.position.x, position.y - playerBounds.position.y);

                playerClose = horizontalDistance < 70.f;
                playerAway = horizontalDistance > 120.f;

                if(paralizeCounter>=paralizeTimer){
                    timer += deltaTime;
                    collisionWithHolyWater = false;
                    switch(currentState){
                        case State::ATTACKING:
                            // ATTACK LOGIC
                            if(!generated){
                                // auto audio = configManager.getAudio();
                                // gameSoundManager.playSound("17", gameSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                                generateScythes(playerBounds.position, mapDims);
                            }
                            if (timer >= attackInterval) {    // Attack timer
                                timer = 0;
                                generated = false;
                                int chance = rand() % 2;
                                if(mode.hard_mode && chance == 0 && !triedAI){ // Enhanced AI
                                    if(playerBounds.position.x < 15 || playerBounds.position.x > 225){
                                        std::cout << "ALL OUT ATTACK" << std::endl;
                                        triedAI = true;
                                        auto audio = configManager.getAudio();
                                        gameSoundManager.playSound("falling_stage2", gameSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                                        for(auto& scythe : scythes){
                                            if(scythe && scythe->sprite && scythe->getActive()){
                                                sf::Vector2f attackPosition = playerBounds.position;
                                                scythe->planPosSpeed(attackPosition);
                                            }
                                        }
                                    }
                                    else{
                                        chance = rand() % 2;
                                        if(chance == 0){
                                            std::cout << "ALL OUT ATTACK" << std::endl;
                                            triedAI = true;
                                            auto audio = configManager.getAudio();
                                            gameSoundManager.playSound("falling_stage2", gameSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                                            for(auto& scythe : scythes){
                                                if(scythe && scythe->sprite && scythe->getActive()){
                                                    sf::Vector2f attackPosition = playerBounds.position;
                                                    scythe->planPosSpeed(attackPosition);
                                                }
                                            }
                                        }
                                        else{
                                            triedAI = false;
                                            for(auto& scythe : scythes){
                                                if(scythe && scythe->sprite && scythe->getActive()){
                                                    sf::Vector2f randomPosition = getRandomScythesPos(playerBounds.position);
                                                    scythe->planPosSpeed(randomPosition);
                                                }
                                            }
                                        }
                                    }
                                }
                                else{
                                    triedAI = false;
                                    for(auto& scythe : scythes){
                                        if(scythe && scythe->sprite && scythe->getActive()){
                                            sf::Vector2f randomPosition = getRandomScythesPos(playerBounds.position);
                                            scythe->planPosSpeed(randomPosition);
                                        }
                                    }
                                }

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
                            if(!startingMove){
                                selectObjective();
                                startingMove = true;
                                timer = 0;
                            }

                            if (timer >= moveInterval) {    // Move timer
                                timer = 0;
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

                        case State::WAITING:
                            if(timer >= waitInterval){
                                timer = 0;
                                if(mode.hard_mode){
                                    selectNewState();
                                }
                                else{
                                    currentState = State::ATTACKING;
                                }
                            }
                            break;
                    }
                }
                else{
                    speed = sf::Vector2f(0, 0);
                    paralizeCounter += deltaTime;
                }
            }
        }

        // POSICIÓN DE SIMON PARA SABER DIRECCIÓN EN LA QUE MIRAR
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

        if(playerBounds.position.x < position.x){
            sprite->setScale(sf::Vector2f(1, 1));
        }
        else{
            sprite->setScale(sf::Vector2f(-1, 1));
        }

        // int cont = 0;
        for(auto& scythe : scythes){
            if(scythe && scythe->sprite && scythe->getActive()){
                scythe->update(deltaTime, mapBounds);
                // cont ++;
            }
        }
        // std::cout << cont << std::endl;
        currentBossLife = life;
    }
    
}

void Death::getLinelSpeed(float timeToMove){
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;

    speed = sf::Vector2f(deltaX / timeToMove, -deltaY / timeToMove);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void Death::getDoubleSpeed() {
    float t = doubleMoveTimer / moveInterval;
    if (t > 1.f) t = 1.f;

    // horizontal component: constant velocity
    float totalDx   = goal.x - startPosition.x;
    float xVel      = totalDx / moveInterval;

    // “linear” vertical component
    float totalDy   = goal.y - startPosition.y;
    float yVelLin   = totalDy / moveInterval;

    // Parabolic arch (inverted U): 
    // f(t) = 4*H*(t-0.5)^2 - H => Extra vertical part
    // f'(t) = d/dt = 8*H*(t - 0.5)
    float arcDeriv = 8.f * arcHeight * (t - 0.5f);
    float yVelArc  = arcDeriv / moveInterval;

    float yVel = yVelLin + yVelArc;

    speed = sf::Vector2f(xVel, -yVel);
    // std::cout << "Double speed: " << speed.x << " " << speed.y << std::endl;
}

void Death::selectObjective(){
    doubleMoveTimer = 0.f;
    startPosition = position;
    if(up){
        goal = sf::Vector2f(25, 105);
        up = false;
        right = false;
    }
    else{
        auto mode = configManager.getDifficulty();
        int posY = 105;
        if(mode.hard_mode){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> distY(25, 105);
            posY = distY(gen);
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        if(right){
            int posX = 25;
            if(mode.hard_mode){
                std::uniform_int_distribution<int> distX(25, 90);
                posX = distX(gen);
            }
            goal = sf::Vector2f(posX, posY);
            right = false;
        }
        else{
            int posX = 190;
            if(mode.hard_mode){
                std::uniform_int_distribution<int> distX(125, 190);
                posX = distX(gen);
            }
            goal = sf::Vector2f(posX, posY);
            right = true;
        }
    }
}

void Death::generateScythes(const sf::Vector2f &playerPos, const sf::FloatRect &mapDims)
{
    generated = true;
    int created = 0;

    for (int i = 0; i < 5 && created < 2; i++)
    {
        if (!scythes[i] || !scythes[i]->getActive())
        {
            sf::Vector2f randomPosition = getRandomScythesPos(playerPos);

            // Create the scythe in a random position
            scythes[i] = createScythe(randomPosition, mapDims, DEATH_DAMAGE);
            scythes[i]->setActive(true);

            created++;
        }
    }
}

sf::Vector2f Death::getRandomScythesPos(const sf::Vector2f &playerPos){
    static std::random_device rd;
    static std::mt19937 gen(rd());

    float minX = mapDims.position.x + 9;
    float maxX = mapDims.position.x + mapDims.size.x - 9;
    float minY = mapDims.position.y + 9;
    float maxY = mapDims.position.y + mapDims.size.y - 20;

    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);

    const float minDistanceToPlayer = 32.f;

    sf::Vector2f randomPosition;

    do {
        randomPosition = { distX(gen), distY(gen) };
    } while (std::hypot(randomPosition.x - playerPos.x, randomPosition.y - playerPos.y) < minDistanceToPlayer);

    return randomPosition;
}

void Death::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        int idx = static_cast<int>(currentState);
        if (idx >= 0 && idx < 3) {
            damageReceived[idx] += whip->whipDmg;
        }
        // sf::Vector2f spriteCenter = position + sprite->getGlobalBounds().size / 2.f;
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            currentBossLife = 0;
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            for(auto& s : scythes){
                if(s) s->reset();
            }
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        int idx = static_cast<int>(currentState);
        if (idx >= 0 && idx < 3) {
            damageReceived[idx] += subWeapon->subDamage;
        }
        // sf::Vector2f spriteCenter = position + sprite->getGlobalBounds().size / 2.f;
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            currentBossLife = 0;
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            for(auto& s : scythes){
                if(s) s->reset();
            }
            game.particleSystem.spawnBigFireParticle(position, false);
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
    
}

// Render Death and debug info (spawn zone)
void Death::draw(sf::RenderWindow &window)
{
    if (sprite && isActive && isInBossFight)
    {
        Boss::draw(window);
    }

    for(auto& scythe : scythes){
        if (scythe && scythe->sprite && scythe->getActive())
        {
            //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
            scythe->draw(window);
        }
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

void Death::resetPosition()
{
    Boss::resetPosition();

    speed = DEATH_SPEED;
    life = DEATH_LIFE;

    currentAnimation = sleepPhantomBat;
    dead = false;

    gKilledBoss = false;
    goingDown = true;
    currentState = State::ATTACKING;

    up = true;
    right = false;
    dead = false;

    starting = true;
    timer = 0.f;
    doubleMoveTimer = 0.f;

    triedAI = false;
    weights[0] = 1;
    weights[1] = 1;
    weights[2] = 1;

    damageReceived[0] = 0;
    damageReceived[1] = 0;
    damageReceived[2] = 0;

    startingMove = false;
    playerClose = false;
    playerAway = false;

    collisionWithHolyWater = false;
    paralizeCounter = 0.7f;

    for(auto& s : scythes){
        if(s) s->reset();
    }
}

void Death::selectNewState(){
    updateWeights();

    startingMove = false;

    int totalWeight = weights[0] + weights[1] + weights[2];
    if (totalWeight == 0) return;

    int r = rand() % totalWeight;

    if (r < weights[0]) {
        currentState = State::WAITING;
        std::cout << "Selected WAITING" << std::endl;
    } else if (r < weights[0] + weights[1]) {
        currentState = State::MOVING;
        std::cout << "Selected MOVING" << std::endl;
    } else {
        currentState = State::ATTACKING;
        std::cout << "Selected ATTACKING" << std::endl;
    }

    timer = 0.f;
    speed = sf::Vector2f(0.f, 0.f);
}

void Death::updateWeights() {
    // Reset base weights
    weights[0] = 1; // WAITING
    weights[1] = 1; // MOVING
    weights[2] = 1; // ATTACKING

    float lifeRatio = static_cast<float>(life) / static_cast<float>(DEATH_LIFE);

    // If life is low, prioritize moving to escape
    if (lifeRatio < 0.35f) {
        weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[1] += 2; // More likely to flee
    }

    // If the player is attacking, avoid standing still
    if (gIsWhipBeingUsed || gIsSubWeaponBeingUsed) {
        weights[0] = std::max(0, weights[0] - 1); // Remove WAITING if you are in a critical situation
        weights[1] += 1; // Move more to dodge
    }

    // Distance to the player
    if (playerClose) {
        weights[1] += 2; // Try to get away
    } else if (playerAway) {
        weights[2] += 3; // Take advantage of the distance to attack
    }

    // Penalize the most damaged state
    int maxDamageIdx = 0;
    for (int i = 1; i < 3; ++i) {
        if (damageReceived[i] > damageReceived[maxDamageIdx]) {
            maxDamageIdx = i;
        }
    }

    // Penalize that state by reducing its weight
    if (weights[maxDamageIdx] > 0) {
        weights[maxDamageIdx] = std::max(0, weights[maxDamageIdx] - 2);
        std::cout << maxDamageIdx << std::endl;
    }

    // Penalize repeating the same state
    int currentIdx = static_cast<int>(currentState);
    if (weights[currentIdx] > 1) {
        weights[currentIdx] -= 1;
    }

    // Rebalancing weights if all are zero
    int total = weights[0] + weights[1] + weights[2];
    if (total == 0) {
        weights[1] = 1; // MOVING by default if all else fails
    }

    std::cout << "Weight WAITING: " << weights[0] << std::endl;
    std::cout << "Weight MOVING: " << weights[1] << std::endl;
    std::cout << "Weight ATTACKING: " << weights[2] << std::endl;
}

void Death::hello() const
{
    std::cout << "Soy Death" << std::endl;
}
