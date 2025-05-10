#include "whiteSkeleton.h"
#include "../game.h"
#include <iostream>

WhiteSkeleton::WhiteSkeleton(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = WHITE_SKELETON_SPEED;
    life = WHITE_SKELETON_LIFE;
    score = WHITE_SKELETON_SCORE;
    damage = WHITE_SKELETON_DAMAGE;

    walkTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;

    atTheEdge = false;
    isPlayerRight = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Skeleton AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(whiteSkeletonIdle, this->idleFrames);
    animationManager->addAnimation(whiteSkeletonWalkAttack, this->walkAttackFrames);

    this->animationManager = animationManager;
    currentAnimation = whiteSkeletonIdle;

    currentState = State::IDLE;
}

void WhiteSkeleton::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
            const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds){
    
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
        
        isPlayerRight = sprite->getGlobalBounds().position.x <= playerPos.x;

        if(isPlayerRight){
            sprite->setScale({-1.f, 1.f});
        } else {
            sprite->setScale({1.f, 1.f});
        }
        
        float distance = 0.f;
        
        distance = abs(position.x - playerPos.x);

        switch(currentState){
            case State::IDLE:
                break;

            case State::WALKINGAWAY:
                break;

            case State::WALKINGCLOSE:
                break;
                
            case State::ATTACKING:
                break;

            default:
                break;
        }

        applyGravity(deltaTime);

        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }

        updateAnimation(deltaTime);

        isOnGround = false;
    }
}


void WhiteSkeleton::draw(sf::RenderWindow &window){
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
}

void WhiteSkeleton::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
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
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
}

void WhiteSkeleton::updateAnimation(float deltaTime){
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}
    
void WhiteSkeleton::resetPosition(){
    Enemy::resetPosition();

    speed = WHITE_SKELETON_SPEED;
    life = WHITE_SKELETON_LIFE;
    score = WHITE_SKELETON_SCORE;
    damage = WHITE_SKELETON_DAMAGE;

    walkTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;

    atTheEdge = false;
    isPlayerRight = false;

    currentAnimation = whiteSkeletonIdle;
    currentState = State::IDLE;

    sprite->setScale({1.0f, 1.0f});
}

void WhiteSkeleton::hello() const{
    std::cout << "Hello from WhiteSkeleton!" << std::endl;
}