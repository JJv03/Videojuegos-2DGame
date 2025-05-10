#include "redSkeleton.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Initialize ghost with stats and vision field
RedSkeleton::RedSkeleton(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = RED_SKELETON_SPEED;
    life = RED_SKELETON_LIFE;
    score = RED_SKELETON_SCORE;
    damage = RED_SKELETON_DAMAGE;

    waitingDeadTimeCounter = 0.f;
    waitingIdleTimeCounter = 0.f;
    walkTimeCounter = 0.f;
    waitingRespawnTimeCounter = 0.f;
    waitingDespawnTimeCounter = 0.f;

    moveRight = false;
    atTheEdge = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Skeleton AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(redSkeletonDead, this->deadFrames);
    animationManager->addAnimation(redSkeletonSpawn, this->spawnFrames);
    animationManager->addAnimation(redSkeletonIdle, this->idleFrames);
    animationManager->addAnimation(redSkeletonWalk, this->walkingFrames);

    this->animationManager = animationManager;
    currentAnimation = redSkeletonDead;

    currentState = State::DEAD;
    prevState = State::DEAD;
}

// Main update loop
void RedSkeleton::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
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
        sf::Vector2f position = sprite->getGlobalBounds().position;
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }
        
        if(currentState == State::IDLE || currentState == State::WALKING){
            if(moveRight){
                sprite->setScale({-1.f, 1.f});
            } else {
                sprite->setScale({1.f, 1.f});
            }
        }
        

        float distance = 0.f;

        distance = abs(position.x - playerPos.x);
        
        // STATE MACHINE
        switch(currentState){
            case State::DEAD:
                currentAnimation = redSkeletonDead;

                if(abs((sprite->getGlobalBounds().position.y + 32.f) - playerPos.y) <= 32.f){
                    moveRight = sprite->getGlobalBounds().position.x <= playerPos.x;

                    waitingDeadTimeCounter += deltaTime;
    
                    if(waitingDeadTimeCounter >= DEAD_TIME){
                        waitingDeadTimeCounter = 0.f;
                        currentState = State::RESPAWNING;
                    }
                }
                
                break;
            
            case State::RESPAWNING:
                currentAnimation = redSkeletonSpawn;
                waitingRespawnTimeCounter += deltaTime;

                if(waitingRespawnTimeCounter >= SPAWN_TIME){
                    waitingRespawnTimeCounter = 0.f;
                    currentState = State::IDLE;
                }
                break;

            case State::WALKING:
                currentAnimation = redSkeletonWalk;

                
                // Movement
                if(moveRight){
                    speed = sf::Vector2f(RED_SKELETON_SPEED.x, RED_SKELETON_SPEED.y);
                }
                else{
                    speed = sf::Vector2f(-RED_SKELETON_SPEED.x, RED_SKELETON_SPEED.y);
                }
                 
                walkTimeCounter += deltaTime;
                if(walkTimeCounter >= WALK_TIME || atTheEdge){
                    walkTimeCounter = 0.f;
                    prevState = currentState;
                    currentState = State::IDLE;
                }

                if (speed.x != 0)
                {
                    sprite->move({speed.x * deltaTime, 0.f});
                    for (auto &hitbox : hitboxes)
                    {
                        hitbox.position.x += speed.x * deltaTime;
                    }
                }
                
                break;

            case State::IDLE:
                currentAnimation = redSkeletonIdle;

                // Wait a bit then walk
                speed = sf::Vector2f(0, 0);
                waitingIdleTimeCounter += deltaTime;
                if(waitingIdleTimeCounter >= WALK_TIME){
                    waitingIdleTimeCounter = 0.f;

                    bool isPlayerRight = sprite->getGlobalBounds().position.x <= playerPos.x;

                    if(distance >= DISTANCE_TO_PLAYER + 8 && (isPlayerRight != moveRight)){    // 32/4, 1/4 of a tile
                        moveRight = !moveRight;
                    }

                    if(atTheEdge){
                        moveRight = !moveRight;
                        atTheEdge = false;
                    }
            
                    currentState = State::WALKING;
                }
                break;

                case State::DESPAWNING:
                    currentAnimation = redSkeletonSpawn;
                    waitingDespawnTimeCounter += deltaTime;

                    if(waitingDespawnTimeCounter >= SPAWN_TIME){
                        waitingDespawnTimeCounter = 0.f;
                        currentState = State::DEAD;
                    }
                    break;

                default:
                break;
        }
        
        if(currentState == State::IDLE || currentState == State::WALKING){
            applyGravity(deltaTime);

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

        isOnGround = false;
    }
    
}

void RedSkeleton::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);

        if (isOnGround && Enemy::checkForLedge(other))
        {
            atTheEdge = true;
        }
    }

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            currentState = State::DESPAWNING;
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            currentState = State::DESPAWNING;
        }
    }
}

// Reset to initial state
void RedSkeleton::resetPosition()
{
    Enemy::resetPosition();

    speed = RED_SKELETON_SPEED;
    life = RED_SKELETON_LIFE;
    score = RED_SKELETON_SCORE;
    damage = RED_SKELETON_DAMAGE;

    waitingDeadTimeCounter = 0.f;
    waitingIdleTimeCounter = 0.f;
    walkTimeCounter = 0.f;
    waitingRespawnTimeCounter = 0.f;
    waitingDespawnTimeCounter = 0.f;

    moveRight = false;
    atTheEdge = false;

    currentAnimation = redSkeletonDead;
    currentState = State::DEAD;
    prevState = State::DEAD;

    sprite->setScale({1.0f, 1.0f});
}

// Render with optional debug visuals
void RedSkeleton::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
}

// Update animation frame and direction
void RedSkeleton::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}

std::vector<sf::FloatRect> RedSkeleton::getBounds() const{
    if(animationManager->isPlaying(noAnimation) ||
        currentState == State::DEAD ||
        currentState == State::RESPAWNING ||
        currentState == State::DESPAWNING)
    {
        return std::vector<sf::FloatRect>({sf::FloatRect()});
    }
    return std::vector<sf::FloatRect>({sprite->getGlobalBounds()});
}

void RedSkeleton::hello() const
{
    std::cout << "Soy RedSkeleton" << std::endl;
}