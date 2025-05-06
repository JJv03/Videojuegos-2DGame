#include "cannon.h"
#include <iostream>
#include "../game.h"
#include "../configManager.h"

static std::random_device rd;
static std::mt19937 rng(rd());

Cannon::Cannon(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage){
    speed = CANNON_SPEED;
    life = CANNON_LIFE;
    score = CANNON_SCORE;
    damage = CANNON_DAMAGE;

    waitingFirstAttackTimeCounter = 0.f;
    waitingSecondAttackTimeCounter = 0.f;

    facingRight = false;
    isPlayerRight = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Dracula Mask AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);
    animationManager->addAnimation(idleCannon, this->idleFrames);
    animationManager->addAnimation(chargingCannon, this->chargingFrames);

    this->animationManager = animationManager;
    currentAnimation = idleCannon;
    animationManager->playAnimation(idleCannon);
}

// Reset cannon to initial state
void Cannon::resetPosition(){
    Enemy::resetPosition();

    speed = {0.0f, 0.0f};
    life = CANNON_LIFE;

    sprite->setScale({1.0f, 1.0f});

    waitingFirstAttackTimeCounter = 0.f;
    waitingSecondAttackTimeCounter = 0.f;

    facingRight = false;
    isPlayerRight = false;

    animationManager->playAnimation(idleCannon);
}

// Render cannon and debug info
void Cannon::draw(sf::RenderWindow &window){
    if(sprite && isActive)
    {
        Enemy::draw(window);
    }
    for(auto& projectile : projectiles){
        if (projectile && projectile->sprite && projectile->getActive())
        {
            projectile->draw(window);
        }
    }
}

// Update cannon logic (spawn, etc.)
void Cannon::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
            const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds, const sf::FloatRect &mapBounds){
    
    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    for (const auto &hitbox : hitboxes)
    {
        if (playerActivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideActivationZone = true;
        }
        if (playerDeactivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideDeactivationZone = true;
        }
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

    // ACTIVE ENEMY LOGIC
    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }
        

        // STATE MACHINE
        switch(currentState){
            case State::WAITING_FIRST_ATTACK:

                break;

            case State::CHARGING:

                break;

            case State::FIRST_ATTACK:

                break;

            case State::WAITING_SECOND_ATTACK:

                break;

            case State::SECOND_ATTACK:

                break;

            default:
                break;
        }

    updateProjectiles(deltaTime, mapBounds);
    updateAnimation(deltaTime);
    }     
}

// Handle collisions
void Cannon::onCollision(Entity &other, Game &game){
    if (!isActive || !sprite)
    return;

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

void Cannon::updateProjectiles(float deltaTime, const sf::FloatRect& mapBounds){
    for(auto& projectile : projectiles){
        if(projectile && projectile->sprite && projectile->getActive()){
            projectile->update(deltaTime, mapBounds);
        }
    }
}


// Update animation frame
void Cannon::updateAnimation(float deltaTime){
    if (!isActive || !sprite) return;

    switch(currentState){
        case State::WAITING_FIRST_ATTACK:
            if(!animationManager->isPlaying(idleCannon)){
                animationManager->playAnimation(idleCannon);
            }
            break;

        case State::CHARGING:
            if(!animationManager->isPlaying(chargingCannon)){
                animationManager->playAnimation(chargingCannon);
            }
            break;

        case State::FIRST_ATTACK:
            if(!animationManager->isPlaying(idleCannon)){
                animationManager->playAnimation(idleCannon);
            }
            break;

        case State::WAITING_SECOND_ATTACK:
            if(!animationManager->isPlaying(idleCannon)){
                animationManager->playAnimation(idleCannon);
            }
            break;

        case State::SECOND_ATTACK:
            if(!animationManager->isPlaying(idleCannon)){
                animationManager->playAnimation(idleCannon);
            }
            break;

        default:
            if(!animationManager->isPlaying(noAnimation)){
                animationManager->playAnimation(noAnimation);
            }
            break;
    }
}

float Cannon::randomWaitTime(){
    std::uniform_real_distribution<float> dis(MIN_WAITING_FIRST_ATTACK_TIME, MAX_WAITING_FIRST_ATTACK_TIME);

    return dis(rng);
}

void Cannon::hello() const {
    std::cout << "Soy Cannon" << std::endl;
}