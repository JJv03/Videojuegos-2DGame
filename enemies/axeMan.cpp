#include "axeMan.h"
#include <iostream>
#include <cmath>
#include "../game.h"
#include "axe.h"

// Initialize ghost with stats and vision field
AxeMan::AxeMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = sf::Vector2f(0, 0);
    life = AXEMAN_LIFE;
    score = AXEMAN_SCORE;
    damage = AXEMAN_DAMAGE;

    timerWalk = 0.f;
    timerWait = 0.f;
    timerAttack = 0.f;
    attackInterval = 0.f;
    atTheEdge = false;
    selectedTime = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize AxeMan AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(axeManMovement, this->axeMovementFrames);

    this->animationManager = animationManager;
    currentAnimation = axeManMovement;

    currentState = State::WALKINGCLOSE;
    prevState = State::WALKINGCLOSE;
}

// Main update loop
void AxeMan::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                   const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    if (playerActivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideActivationZone = true;
    }
    if (playerDeactivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideDeactivationZone = true;
    }

    // If the player is outside the deactivation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideDeactivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // We only activate if the player is in the area, the enemy is not active and the player has previously moved away
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        isActive = true;
    }

    // Deactivates if the enemy is active and has left the deactivation zone
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    if (isActive)
    {   
        position = sprite->getGlobalBounds().position;
        // if (checkMapBoundaries(mapBounds))
        // {
        //     return;
        // }

        isPlayerRight = sprite->getGlobalBounds().position.x <= playerPos.x;

        if(isPlayerRight){
            sprite->setScale({-1.f, 1.f});
        } else {
            sprite->setScale({1.f, 1.f});
        }

        float distance = 0.f;
        if (isPlayerRight){
            distance = playerPos.x - position.x;
        }
        else{
            distance = position.x - playerPos.x;
        }

        // Random timer to attack
        if(!selectedTime){
            selectedTime = true;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(1.5f, 5.0f);

            attackInterval = dist(gen);
            timerAttack = 0;
        }

        timerAttack += deltaTime;
        if(timerAttack >= attackInterval){
            prevState = currentState;
            currentState = State::ATTACK;
            selectedTime = true;
            timerAttack = 0;
        }

        // STATE MACHINE
        switch(currentState){
            case State::WALKINGCLOSE:
                // Movement
                if(isPlayerRight){
                    speed = sf::Vector2f(AXEMAN_SPEED.x, AXEMAN_SPEED.y);
                }
                else{
                    speed = sf::Vector2f(-AXEMAN_SPEED.x, AXEMAN_SPEED.y);
                }
                
                timerWalk += deltaTime;
                if(timerWalk >= walkInterval || atTheEdge){
                    timerWalk = 0.f;
                    prevState = currentState;
                    currentState = State::WAITINGWALK;
                }
                break;
            case State::WALKINGAWAY:
                // Movement
                if(isPlayerRight){
                    speed = sf::Vector2f(-AXEMAN_SPEED.x, AXEMAN_SPEED.y);
                }
                else{
                    speed = sf::Vector2f(AXEMAN_SPEED.x, AXEMAN_SPEED.y);
                }
                
                timerWalk += deltaTime;
                if(timerWalk >= walkInterval || atTheEdge){
                    timerWalk = 0.f;
                    prevState = currentState;
                    currentState = State::WAITINGWALK;
                }
                break;
            case State::WAITINGWALK:
                // Wait a bit then walk
                speed = sf::Vector2f(0, 0);
                timerWait += deltaTime;
                if(timerWait >= waitInterval){
                    timerWait = 0.f;
                    if(atTheEdge){
                        if(prevState == State::WALKINGAWAY){
                            currentState = State::WALKINGCLOSE;
                        }
                        else if(prevState == State::WALKINGCLOSE){
                            currentState = State::WALKINGAWAY;
                        }
                        else{
                            currentState = State::WAITINGWALK;
                        }
                        atTheEdge = false;
                    }
                    else{
                        if(distance < distanceToPlayer + 8){    // 32/4, 1/4 of a tile
                            currentState = State::WALKINGAWAY;
                        }
                        else{
                            currentState = State::WALKINGCLOSE;
                        }
                    }
                }
                break;
            case State::ATTACK:
                int distance = std::abs(position.y - playerPos.y);
                // std::cout << distance << std::endl;
                if ((!axes[0] || !axes[0]->getActive()) && distance <= 40){
                    int chance = rand() % 2;
                    float posY = 8;
                    if(chance == 0){
                        posY = position.y + 8;
                        std::cout << "Arribaaaaaaaaaaaaaa" << std::endl;
                    }
                    else{
                        posY = position.y + 20;
                        std::cout << "Abajoooooooooooooooooooooo" << std::endl;
                    }
                    if(isPlayerRight){
                        axes[0] = createAxe(sf::Vector2f(position.x + 32, posY), mapBounds, isPlayerRight, AXEMAN_DAMAGE);
                    }
                    else{
                        axes[0] = createAxe(sf::Vector2f(position.x - 12, posY), mapBounds, isPlayerRight, AXEMAN_DAMAGE);
                    }
                    axes[0]->setActive(true);
                }
                currentState = prevState;
                break;
        }
        if(axes[0] && axes[0]->sprite && axes[0]->getActive()){
            axes[0]->update(deltaTime, mapBounds);
            // cont ++;
        }
        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }

        updateAnimation(deltaTime);
    }
    
}

void AxeMan::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);

        if (isOnGround && Enemy::checkForLedge(other))
        {
            atTheEdge = true;
            std::cout << "I dont have the high ground" << std::endl;
        }
    }

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            for(auto& a : axes){
                if(a) a->reset();
            }
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            for(auto& a : axes){
                if(a) a->reset();
            }
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
}

// Reset to initial state
void AxeMan::resetPosition()
{
    Enemy::resetPosition();

    speed = AXEMAN_SPEED;
    life = AXEMAN_LIFE;
    score = AXEMAN_SCORE;
    damage = AXEMAN_DAMAGE;

    timerWalk = 0.f;
    timerWait = 0.f;
    timerAttack = 0.f;
    attackInterval = 0.f;
    atTheEdge = false;

    selectedTime = false;
    timerAttack = 0.f;
    attackInterval = 0.f;

    currentAnimation = axeManMovement;

    currentState = State::WALKINGCLOSE;
    prevState = State::WALKINGCLOSE;

    for(auto& a : axes){
        if(a) a->reset();
    }

    sprite->setScale({1.0f, 1.0f});
}

// Render with optional debug visuals
void AxeMan::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
    if (axes[0] && axes[0]->sprite && axes[0]->getActive())
    {
        //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
        axes[0]->draw(window);
    }
}

// Update animation frame and direction
void AxeMan::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}

void AxeMan::hello() const
{
    std::cout << "Soy AxeMan" << std::endl;
}