#include "dracula.h"
#include "../game.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include "../configManager.h"

static std::random_device rd;       // we only want 1 instance of random_device
static std::mt19937 rng(rd());

DraculaBody::DraculaBody(std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes,
                            const float _damage, sf::Vector2f _position)
                            : EntitySprite(_draculaSprite, _draculaHitboxes), damage(_damage), position(_position) {}


void DraculaBody::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
    auto mode = dracula->configManager.getDifficulty();

    if(mode.hard_mode){
        if (Whip *whip = dynamic_cast<Whip *>(&other))
        {
            if (!whip->collisionedEntities.contains(this))
            {
                dracula->hasBeenHurtThisTime = true;
                if(dracula->applyDamage(whip->whipDmg, game.player)){
                    sprite->setColor(sf::Color::White);
                    dracula->sprite->setColor(sf::Color::White);
                    dracula->currentState = Dracula::DraculaState::DEAD_MASK_OFF;
                }
            }
        }
        else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
        {
            if (!subWeapon->collisionedEntities.contains(this))
            {
                dracula->hasBeenHurtThisTime = true;
                if(dracula->applyDamage(subWeapon->subDamage, game.player)){
                    sprite->setColor(sf::Color::White);
                    dracula->sprite->setColor(sf::Color::White);
                    dracula->currentState = Dracula::DraculaState::DEAD_MASK_OFF;
                }
            }
        }
    }
}

void DraculaBody::hello() const {
    std::cout << "Soy DraculaBody" << std::endl;
} 

std::vector<sf::FloatRect> DraculaBody::getBounds() const{
    if(animationManager->isPlaying(noAnimation)){
        return std::vector<sf::FloatRect>({sf::FloatRect()});
    }
    return std::vector<sf::FloatRect>({this->sprite->getGlobalBounds()});
}


// -------------------------- DRACULA MASK --------------------------

// Constructor: Initialize dracula with sprite, hitboxes, position, and game level/stage
Dracula::Dracula(std::shared_ptr<sf::Sprite> _maskSprite, std::vector<sf::FloatRect> &_maskHitboxes,
                 std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes,
                 const int &level, const int &stage, sf::Vector2f bodyPosition) 
                 : Boss(_maskSprite, _maskHitboxes), level(level), stage(stage)
{

    speed = DRACULA_SPEED;
    life = DRACULA_LIFE;
    score = DRACULA_SCORE;
    damage = DRACULA_DAMAGE;

    gKilledBoss = false;
    isDead = false;

    asleepTimeCounter = 0.f;
    maskWaitTimeCounter = 0.f;
    appearTimeCounter = 0.f;
    disappearTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;
    deadTimeCounter = 0.f;

    maskVerticalSpeed = 0.f;

    facingRight = false;
    isPlayerRight = false;
    adjustHead = false;

    timesPlayerHasNotSwitchedSides = 0;

    attacksThisTime = 0;
    attacksLastTime = 0;
    hasBeenHurtThisTime = false;
    hasBeenHurtLastTime = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Dracula Mask AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(noAnimation, this->noAnimationFrames);

    auto skins = configManager.getSkins();

    if(skins.activated){
        animationManager->addAnimation(draculaMask, this->customMaskDraculaFrames);
    } else {
        animationManager->addAnimation(draculaMask, this->maskDraculaFrames);
    }

    this->animationManager = animationManager;
    currentAnimation = noAnimation;

    // -------------------------
    draculaBody = new DraculaBody(_draculaSprite, _draculaHitboxes, damage, bodyPosition);
    AnimationManager *bodyAnimationManager = new AnimationManager(*draculaBody->sprite, draculaBody);
    if (!bodyAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Dracula Body AnimationManager!" << std::endl;
    }


    bodyAnimationManager->addAnimation(noAnimation, this->noAnimationFrames);
    if(skins.activated){
        bodyAnimationManager->addAnimation(draculaIdle, draculaBody->customIdleDraculaFrames);
        bodyAnimationManager->addAnimation(draculaAttack, draculaBody->customAttackDraculaFrames);
    } else {
        bodyAnimationManager->addAnimation(draculaIdle, draculaBody->idleDraculaFrames);
        bodyAnimationManager->addAnimation(draculaAttack, draculaBody->attackDraculaFrames);
    }
    

    draculaBody->animationManager = bodyAnimationManager;
    weights[0] = 0.f;
    weights[1] = 0.f;
    currentState = DraculaState::ASLEEP;
}

// Update dracula logic: handle spawning, movement, and deactivation
void Dracula::update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds)
{
    if(!draculaBody->dracula || draculaBody->dracula != this){
        draculaBody->dracula = this;
    }

    // SPAWN LOGIC
    if (!isActive && phase == 1)
    {
        life = DRACULA_LIFE;
        maxLife = life;
        isActive = true;
    }

    // MOVEMENT LOGIC
    if (isActive)
    {
        auto mode = configManager.getDifficulty();
        if(mode.hard_mode){
            updateHardMode(deltaTime, player, mapBounds);
        } else {
            updateNormalMode(deltaTime, player, mapBounds);
        }
    }

    currentBossLife = life;
}

// Update dracula logic: handle spawning, movement, and deactivation
void Dracula::updateNormalMode(float deltaTime, const Player &player, const sf::FloatRect &mapBounds)
{
    switch(currentState){
        case DraculaState::ASLEEP:
            asleepTimeCounter += deltaTime;

            if(asleepTimeCounter >= ASLEEP_TIME){
                currentState = DraculaState::MASK_APPEAR;
            }
            break;

        case DraculaState::MASK_APPEAR:

            maskWaitTimeCounter += deltaTime;

            if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                maskWaitTimeCounter = 0.f;
                currentState = DraculaState::MASK_ELEVATE;
            }
            break;

        case DraculaState::MASK_ELEVATE:
            if(sprite->getPosition().y > MASK_MAX_HEIGHT){
                if(sprite->getPosition().y - MASK_MAX_HEIGHT < 1.f){
                    float elevation = sprite->getPosition().y - MASK_MAX_HEIGHT;
                    sprite->move({0.f, -elevation});
                    draculaBody->sprite->move({0.f, -elevation});
                } else {
                    float elevation = MASK_ELEVATE_SPEED * deltaTime;
                    sprite->move({0.f, -elevation});
                    draculaBody->sprite->move({0.f, -elevation});
                }
            } else { // Mask in position
                maskWaitTimeCounter += deltaTime;

                if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                    appearTimeCounter = 0.f;
                    currentState = DraculaState::BODY_APPEAR;
                }
            }
            
            break;

        case DraculaState::BODY_APPEAR:
            if(appearBody(deltaTime)){
                idleTimeCounter = 0.f;
                WAIT_IDLE_TIME = randomIdleTime(player);
                currentState = DraculaState::BATTLE_IDLE;
            }
            break;

        case DraculaState::BATTLE_IDLE:
            idleTimeCounter += deltaTime;

            if(idleTimeCounter >= WAIT_IDLE_TIME){
                attackTimeCounter = 0.f;
                generateProjectiles(player.isJumping);
                currentState = DraculaState::BATTLE_ATTACK;
            }
            break;


        case DraculaState::BATTLE_ATTACK:
            attackTimeCounter += deltaTime;

            if(attackTimeCounter >= WAIT_ATTACK_TIME){
                disappearTimeCounter = 0.f;
                currentState = DraculaState::BATTLE_DISAPPEAR_ATTACK;
            }
            break;

        case DraculaState::BATTLE_DISAPPEAR_ATTACK:
            if(disappear(deltaTime)){
                awayTimeCounter = 0.f;
                currentState = DraculaState::BATTLE_AWAY;
            }
            break;

        case DraculaState::BATTLE_AWAY:
            awayTimeCounter += deltaTime;

            if(awayTimeCounter >= AWAY_TIME){
                appearTimeCounter = 0.f;
                moveTo(randomPosition(player.getBounds()[0].position.x));
                isPlayerRight = sprite->getGlobalBounds().position.x <= player.getBounds()[0].position.x;
                adjustHead = true;
                currentState = DraculaState::BATTLE_APPEAR;
            }
            break;

        case DraculaState::BATTLE_APPEAR:
            if(appear(deltaTime)){
                idleTimeCounter = 0.f;
                WAIT_IDLE_TIME = randomIdleTime(player);
                currentState = DraculaState::BATTLE_IDLE;
            }
            break;

        case DraculaState::DEAD_MASK_OFF:
            if(!isDead){
                isDead = true;
                maskVerticalSpeed = -MASK_VERTICAL_SPEED;
                isPlayerRight = sprite->getGlobalBounds().position.x <= player.getBounds()[0].position.x;
            }

            if(deadMaskAnimation(deltaTime, mapBounds)){
                currentState = DraculaState::DEAD_WAIT;
            }
            break;
        
        case DraculaState::DEAD_WAIT:
            deadTimeCounter += deltaTime;

            if(deadTimeCounter >= DEAD_TIME){
                deadTimeCounter = 0.f;
                gKilledBoss = true;
            }
        default:
            break;
    }

    updateProjectiles(deltaTime, mapBounds);
    updateAnimation(deltaTime);
}

// Update dracula logic: handle spawning, movement, and deactivation
void Dracula::updateHardMode(float deltaTime, const Player &player, const sf::FloatRect &mapBounds)
{
    switch(currentState){
        case DraculaState::ASLEEP:
            asleepTimeCounter += deltaTime;

            if(asleepTimeCounter >= ASLEEP_TIME){
                currentState = DraculaState::MASK_APPEAR;
            }
            break;

        case DraculaState::MASK_APPEAR:

            maskWaitTimeCounter += deltaTime;

            if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                maskWaitTimeCounter = 0.f;
                currentState = DraculaState::MASK_ELEVATE;
            }
            break;

        case DraculaState::MASK_ELEVATE:
            if(sprite->getPosition().y > MASK_MAX_HEIGHT){
                if(sprite->getPosition().y - MASK_MAX_HEIGHT < 1.f){
                    float elevation = sprite->getPosition().y - MASK_MAX_HEIGHT;
                    sprite->move({0.f, -elevation});
                    draculaBody->sprite->move({0.f, -elevation});
                } else {
                    float elevation = MASK_ELEVATE_SPEED * deltaTime;
                    sprite->move({0.f, -elevation});
                    draculaBody->sprite->move({0.f, -elevation});
                }
            } else { // Mask in position
                maskWaitTimeCounter += deltaTime;

                if(maskWaitTimeCounter >= WAIT_MASK_TIME){
                    appearTimeCounter = 0.f;
                    currentState = DraculaState::BODY_APPEAR;
                }
            }
            
            break;

        case DraculaState::BODY_APPEAR:
            if(appearBody(deltaTime)){
                idleTimeCounter = 0.f;
                WAIT_IDLE_TIME = randomIdleTime(player);
                currentState = DraculaState::BATTLE_IDLE;
            }
            break;

        case DraculaState::BATTLE_IDLE:
            idleTimeCounter += deltaTime;

            if(idleTimeCounter >= WAIT_IDLE_TIME){
                bool attack = shouldAttack(player);
                if(attack){ // Whether to attack or not
                    attacksThisTime++;
                    attackTimeCounter = 0.f;
                    generateProjectiles(player.isJumping);
                    currentState = DraculaState::BATTLE_ATTACK;
                } else {
                    attacksLastTime = attacksThisTime; // Reset attack counter
                    attacksThisTime = 0;
                    disappearTimeCounter = 0.f;
                    currentState = DraculaState::BATTLE_DISAPPEAR_IDLE;
                }
            }
            break;


        case DraculaState::BATTLE_ATTACK:
            attackTimeCounter += deltaTime;
            if(attackTimeCounter >= WAIT_ATTACK_TIME){
                bool startAttack = shouldStartAnotherAttack(player);
                if(startAttack){
                    idleTimeCounter = 0.f;
                    currentState = DraculaState::BATTLE_IDLE;
                } else {
                    attacksLastTime = attacksThisTime;
                    attacksThisTime = 0;
                    disappearTimeCounter = 0.f;
                    currentState = DraculaState::BATTLE_DISAPPEAR_ATTACK;
                }
            }
            break;
    

        case DraculaState::BATTLE_DISAPPEAR_IDLE:
            if(disappear(deltaTime)){
                awayTimeCounter = 0.f;
                currentState = DraculaState::BATTLE_AWAY;
            }
            break;

        case DraculaState::BATTLE_DISAPPEAR_ATTACK:
            if(disappear(deltaTime)){
                awayTimeCounter = 0.f;
                currentState = DraculaState::BATTLE_AWAY;
            }
            break;


        case DraculaState::BATTLE_AWAY:
            awayTimeCounter += deltaTime;

            if(awayTimeCounter >= AWAY_TIME){
                appearTimeCounter = 0.f;
                moveDraculaHardMode(player);
                bool wasPlayerRight = isPlayerRight;
                isPlayerRight = sprite->getGlobalBounds().position.x <= player.getBounds()[0].position.x;

                if(isPlayerRight == wasPlayerRight){
                    timesPlayerHasNotSwitchedSides++;
                } else {
                    timesPlayerHasNotSwitchedSides = 0;
                }

                hasBeenHurtLastTime = hasBeenHurtThisTime;
                hasBeenHurtThisTime = false;
                adjustHead = true;
                currentState = DraculaState::BATTLE_APPEAR;
            }
            break;

        case DraculaState::BATTLE_APPEAR:
            if(appear(deltaTime)){
                idleTimeCounter = 0.f;
                WAIT_IDLE_TIME = randomIdleTime(player);
                currentState = DraculaState::BATTLE_IDLE;
            }
            break;

        case DraculaState::DEAD_MASK_OFF:
            if(!isDead){
                isDead = true;
                maskVerticalSpeed = -MASK_VERTICAL_SPEED;
                isPlayerRight = sprite->getGlobalBounds().position.x <= player.getBounds()[0].position.x;
            }

            if(deadMaskAnimation(deltaTime, mapBounds)){
                currentState = DraculaState::DEAD_WAIT;
            }
            break;
        
        case DraculaState::DEAD_WAIT:
            deadTimeCounter += deltaTime;

            if(deadTimeCounter >= DEAD_TIME){
                deadTimeCounter = 0.f;
                gKilledBoss = true;
            }
        default:
            break;
    }

    updateWeights();
    updateProjectiles(deltaTime, mapBounds);
    updateAnimation(deltaTime);
}

void Dracula::moveDraculaHardMode(const Player& player){
    bool switchSide = shouldSwitchSide();

    if(switchSide){
        if(isPlayerRight){
            moveTo(randomPositionRight(player.getBounds()[0].position.x));
        } else {
            moveTo(randomPositionLeft(player.getBounds()[0].position.x));
        }
    } else {
        moveTo(randomPosition(player.getBounds()[0].position.x));
    }
}

bool Dracula::shouldSwitchSide() {
    if (timesPlayerHasNotSwitchedSides < MAX_TIMES_SWITCH_SIDE) {
        return false;
    }

    int overLimit = timesPlayerHasNotSwitchedSides - MAX_TIMES_SWITCH_SIDE;

    // Initial probability when timesPlayerHasNotSwitchedSides = MAX_TIMES_SWITCH_SIDE is 0.5 (50%)
    // and every extra time over MAX_TIMES_SWITCH_SIDE increases % by half (50%, 75%, 87.5%, 93.75%, ...)
    float probability = 1.0f - (0.5f / std::pow(2, overLimit));

    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(rng) < probability;
}

bool Dracula::shouldAttack(const Player& player) {

    if(player.isBeingHurt || player.isInvulnerable){
        if(attacksLastTime > 0) weights[1] -= 1;
        else weights[1] += 1;
    } else {
        if(attacksLastTime > 0) weights[1] += 1;
        else weights[1] -= 1;
    }

    if(player.isBeingHurt || player.isInvulnerable){
        if(attacksLastTime > 0) weights[1] -= 1;
        else weights[1] += 1;
    } else {
        if(attacksLastTime > 0) weights[1] += 1;
        else weights[1] -= 1;
    }


    if (attacksLastTime == 0 && attacksThisTime == 0) { // No-attack can't happen twice in a row
        //std::cout << "ShouldAttack: No-attack can't happen twice in a row" << std::endl;
        return true;
    }

    if (attacksLastTime == 2 && attacksThisTime == 1) { // Double-attack can't happen twice in a row
        //std::cout << "ShouldAttack: Double-attack can't happen twice in a row" << std::endl;
        return false;
    }

    if(std::abs(player.sprite->getPosition().x - sprite->getPosition().x) < DISTANCE_TOO_CLOSE){    // If too close, maybe attack
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float prob = dis(rng);
        prob = prob + (weights[0] * 0.1f);
        prob = std::clamp(prob, 0.0f, 1.0f);
        //std::cout << "ShouldAttack: Too close" << std::endl;
        return prob < PROB_MID;
    }

    if(std::abs(player.sprite->getPosition().x - sprite->getPosition().x) > DISTANCE_TOO_FAR){      // If too far, attack
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float prob = dis(rng);
        prob = prob + (weights[0] * 0.1f);
        prob = std::clamp(prob, 0.0f, 1.0f);
        //std::cout << "ShouldAttack: Too far" << std::endl;
        return prob < PROB_HIGH;
    }

    if (attacksThisTime == 0){
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float prob = dis(rng);
        prob = prob + (weights[0] * 0.1f);
        prob = std::clamp(prob, 0.0f, 1.0f);
        //std::cout << "ShouldAttack: Didn't attack this time" << std::endl;
        return prob < PROB_HIGH;
    }

    if (attacksThisTime == 1){
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float prob = dis(rng);
        prob = prob + (weights[0] * 0.1f);
        prob = std::clamp(prob, 0.0f, 1.0f);
        //std::cout << "ShouldAttack: Already attacked this time" << std::endl;
        return prob < PROB_MID;
    }

    if (attacksThisTime == 2){
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float prob = dis(rng);
        prob = prob + (weights[0] * 0.1f);
        prob = std::clamp(prob, 0.0f, 1.0f);
        //std::cout << "ShouldAttack: Already attacked this time" << std::endl;
        return prob < PROB_MID;
    }
    
    return true;
}

bool Dracula::shouldStartAnotherAttack(const Player& player) {

    // Second attack only happens when Dracula is low in hp
    float lifeRatio = static_cast<float>(life) / static_cast<float>(DRACULA_LIFE);
    if(lifeRatio > 0.6) {
        return false;
    }

    if (attacksThisTime >= 2){  // Triple-or-more-attack can't happen if dracula is not almost dead
        //std::cout << "shouldStartAnotherAttack: Triple-or-more-attack can't happen" << std::endl;
        if(lifeRatio < 0.3) return true;
        else return false;
    }

    if(player.isInvulnerable || player.isBeingHurt){
        //std::cout << "shouldStartAnotherAttack: Invulnerable or hurt" << std::endl;
        return false;
    }

    //std::cout << "shouldStartAnotherAttack: Yes" << std::endl;
    return true;
}


void Dracula::generateProjectiles(bool isPlayerJumping){
    float verticalSpeed;

    if(isPlayerJumping) verticalSpeed = -20.f;
                   else verticalSpeed = -10.f; 

    if(isPlayerRight){
        for(auto& projectile : projectiles){
            projectile = createProjectile(
                {   sprite->getPosition().x - 8.f,
                    sprite->getPosition().y + 16.f},
                {PROJECTILE_SPEED, verticalSpeed},
                ProjectileType::FISHMAN,
                PROJECTILE_DAMAGE);
            projectile->sprite->setScale({1.f, 1.f});
            projectile->setActive(true);
            verticalSpeed += 10.f;
        }
    } else {
        for(auto& projectile : projectiles){
            projectile = createProjectile(
                {   sprite->getPosition().x + 8.f,
                    sprite->getPosition().y + 16.f},
                {-PROJECTILE_SPEED, verticalSpeed},
                ProjectileType::FISHMAN,
                PROJECTILE_DAMAGE);
            projectile->sprite->setScale({-1.f, 1.f});
            projectile->setActive(true);
            verticalSpeed += 10.f;
        }
    }
}


void Dracula::updateProjectiles(float deltaTime, const sf::FloatRect& mapBounds){
    for(auto& projectile : projectiles){
        if(projectile && projectile->sprite && projectile->getActive()){
            projectile->update(deltaTime, mapBounds);
        }
    }
}

void Dracula::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this))
        {
            hasBeenHurtThisTime = true;
            if(applyDamage(whip->whipDmg, game.player)){
                sprite->setColor(sf::Color::White);
                currentState = DraculaState::DEAD_MASK_OFF;
                reproduced = true;
            }
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this))
        {
            hasBeenHurtThisTime = true;
            if(applyDamage(subWeapon->subDamage, game.player)){
                sprite->setColor(sf::Color::White);
                currentState = DraculaState::DEAD_MASK_OFF;
                reproduced = true;
            }
        }
    }
}

// Render dracula and debug info
void Dracula::draw(sf::RenderWindow &window)
{
    if(draculaBody && draculaBody->sprite && isActive)
    {
        drawBody(window);
    }
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
    for(auto& projectile : projectiles){
        if (projectile && projectile->sprite && projectile->getActive())
        {
            //std::cout << "P1 " << projectile->sprite->getPosition().x  << "  " << projectile->sprite->getPosition().y << std::endl;
            projectile->draw(window);
        }
    }
}

void Dracula::drawBody(sf::RenderWindow &window)
{
    window.draw(*draculaBody->sprite); // Draw the boss sprite

    if (gDrawHitboxes) // Only draw hitboxes in debug mode
    {
        for (const auto &hitbox : draculaBody->getBounds())
        {
            sf::RectangleShape hitboxShape({hitbox.size.x, hitbox.size.y});
            hitboxShape.setPosition({hitbox.position.x, hitbox.position.y});
            hitboxShape.setFillColor(sf::Color::Transparent);
            hitboxShape.setOutlineColor(sf::Color::Red);
            hitboxShape.setOutlineThickness(1.0f);
            window.draw(hitboxShape);
        }
    }
}

void Dracula::resetPosition()
{
    Boss::resetPosition();

    life = DRACULA_LIFE;

    asleepTimeCounter = 0.f;
    maskWaitTimeCounter = 0.f;
    appearTimeCounter = 0.f;
    disappearTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;
    awayTimeCounter = 0.f;
    deadTimeCounter = 0.f;

    if(sprite){
        sprite->setColor(sf::Color::White);
        sprite->setScale({1.f, 1.f});
    }

    if (draculaBody && draculaBody->sprite) {
        draculaBody->sprite->setPosition(draculaBody->position);
        draculaBody->sprite->setColor(sf::Color::White);
        draculaBody->sprite->setScale({1.f, 1.f});
    }
    
    isPlayerRight = false;
    facingRight = false;
    adjustHead = false;

    timesPlayerHasNotSwitchedSides = 0;

    attacksThisTime = 0;
    attacksLastTime = 0;
    hasBeenHurtThisTime = false;
    hasBeenHurtLastTime = false;
    
    for(auto& p : projectiles){
        if(p) p->reset();
    }

    weights[0] = 0.f;
    weights[1] = 0.f;

    currentState = DraculaState::ASLEEP;

    applyMaskBodyAnimation(noAnimation);
}


// Update animation frame and flip sprite based on direction
void Dracula::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    switch(currentState){
        case DraculaState::ASLEEP:
            applyMaskBodyAnimation(noAnimation);
            break;

        case DraculaState::MASK_APPEAR:
            applyMaskBodyAnimation(draculaMask);
            break;

        case DraculaState::MASK_ELEVATE:
            applyMaskBodyAnimation(draculaMask);
            break;
        
        case DraculaState::BODY_APPEAR:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_IDLE:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_APPEAR:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_ATTACK:
            applyMaskBodyAnimation(draculaAttack);
            break;

        case DraculaState::BATTLE_DISAPPEAR_IDLE:
            applyMaskBodyAnimation(draculaIdle);
            break;

        case DraculaState::BATTLE_DISAPPEAR_ATTACK:
            applyMaskBodyAnimation(draculaAttack);
            break;

        case DraculaState::BATTLE_AWAY:
            applyMaskBodyAnimation(noAnimation);
            break;

        case DraculaState::DEAD_MASK_OFF:
            applyMaskBodyAnimation(draculaIdle);
            break;
            
        case DraculaState::DEAD_WAIT:
            applyMaskBodyAnimation(draculaIdle);
            break;

        default:
            break;
    }
}

void Dracula::applyMaskBodyAnimation(animationID id){
    switch(id){
        case noAnimation:
            if(!this->animationManager->isPlaying(noAnimation)){
                this->animationManager->playAnimation(noAnimation);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;
            
        case draculaMask:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;

        case draculaIdle:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(draculaIdle)){
                draculaBody->animationManager->playAnimation(draculaIdle);
            }

            if(isPlayerRight && !facingRight){
                sprite->move({16.f, 0.f});
            } else if(!isPlayerRight && facingRight){
                sprite->move({-16.f, 0.f});
            }

            if(isPlayerRight && adjustHead){
                facingRight = true;
                adjustHead = false;
                sprite->move({-6.f, 0.f});
                sprite->setScale({-1.0f, 1.0f});
                draculaBody->sprite->setScale({-1.0f, 1.0f});
            } else if (!isPlayerRight && adjustHead){
                facingRight = false;
                adjustHead = false;
                sprite->move({6.f, 0.f});
                sprite->setScale({1.0f, 1.0f});
                draculaBody->sprite->setScale({1.0f, 1.0f});
            }
            break;

        case draculaAttack:
            if(!this->animationManager->isPlaying(draculaMask)){
                this->animationManager->playAnimation(draculaMask);
            }
            if(!draculaBody->animationManager->isPlaying(draculaAttack)){
                draculaBody->animationManager->playAnimation(draculaAttack);
            }
            break;
            
        default:
            if(!this->animationManager->isPlaying(noAnimation)){
                this->animationManager->playAnimation(noAnimation);
            }
            if(!draculaBody->animationManager->isPlaying(noAnimation)){
                draculaBody->animationManager->playAnimation(noAnimation);
            }
            break;
    }
}

bool Dracula::deadMaskAnimation(float deltaTime, const sf::FloatRect &mapBounds){
    if (mapBounds.findIntersection(sprite->getGlobalBounds()).has_value())
    { 
        maskVerticalSpeed += gPlayerGravity * deltaTime * 2.5f;
        if(isPlayerRight) sprite->move({-MASK_HORIZONTAL_SPEED * deltaTime, maskVerticalSpeed * deltaTime});
        else sprite->move({MASK_HORIZONTAL_SPEED * deltaTime, maskVerticalSpeed * deltaTime});
        return false;    
    } else { // Mask exits screen
        return true;
    }
}

std::vector<sf::FloatRect> Dracula::getBounds() const{
    if(animationManager->isPlaying(noAnimation) ||
        currentState == DraculaState::MASK_APPEAR ||
        currentState == DraculaState::MASK_ELEVATE ||
        currentState == DraculaState::BODY_APPEAR ||
        currentState == DraculaState::DEAD_MASK_OFF || 
        currentState == DraculaState::DEAD_WAIT){
        return std::vector<sf::FloatRect>({sf::FloatRect()});
    }
    return std::vector<sf::FloatRect>({sprite->getGlobalBounds()});
}

bool Dracula::appear(float deltaTime){
    if(appearTimeCounter < APPEAR_TIME){
        appearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 128));
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else {
        appearTimeCounter += deltaTime;
        sprite->setColor(sf::Color::White);
        draculaBody->sprite->setColor(sf::Color::White);
        return true;
    }
    return true;
}

bool Dracula::disappear(float deltaTime){
    if(disappearTimeCounter < DISAPPEAR_TIME){
        disappearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 128));
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else {
        disappearTimeCounter += deltaTime;
        sprite->setColor(sf::Color(255, 255, 255, 0));
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 0));
        return true;
    }
    return true;
}

bool Dracula::appearBody(float deltaTime){
    if(appearTimeCounter < APPEAR_TIME){
        appearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else {
        appearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color::White);
        return true;
    }
    return true;
}

bool Dracula::disappearBody(float deltaTime){
    if(disappearTimeCounter < DISAPPEAR_TIME){
        disappearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 128));
        return false;
    } else {
        disappearTimeCounter += deltaTime;
        draculaBody->sprite->setColor(sf::Color(255, 255, 255, 0));
        return true;
    }
    return true;
}

void Dracula::moveTo(float position){
    if(isPlayerRight){
        sprite->move({position - sprite->getGlobalBounds().position.x, 0.f});
        draculaBody->sprite->move({position - draculaBody->sprite->getGlobalBounds().position.x, 0.f});
    } else {
        sprite->move({position - sprite->getGlobalBounds().position.x, 0.f});
        draculaBody->sprite->move({position - draculaBody->sprite->getGlobalBounds().position.x, 0.f});
    }
}

void Dracula::hello() const
{
    std::cout << "Soy Dracula" << std::endl;
} 

float clamp(float v, float lo, float hi) {
    return std::max(lo, std::min(v, hi));
}

void Dracula::updateWeights() {

    // Keep weights between values
    // Weight[0] between -5 and +5
    weights[0] = clamp(weights[0], -5.f, 5.f);
    weights[1] = clamp(weights[1], -5.f, 5.f);
}


float Dracula::randomIdleTime(const Player &player){
    // If dracula has been hurt, switch weight (if high idle time, lower. if low idle time, higher)
    // Otherwise, reward weights (if high idle time, higher. if low idle time, lower)
    if(hasBeenHurtLastTime){
        if(weights[0] <= -2) weights[0] += 2;
        else if (weights[0] == -1) weights[0] = 0;
        else weights[0] -= 2;
    } else {
        if(weights[0] <= 0) weights[0] -= 1;
        else weights[0] += 1;
    }

    std::uniform_real_distribution<float> dis(0.75f, 0.85f);

    float idleTime = dis(rng) + (weights[0] / 10.f);

    // Time between 0.25 and 1.35
    return idleTime;
}

float randomPosition(float playerPos, float margin){
    std::uniform_real_distribution<float> dis(32.f, 224.f);

    float pos;
    do {
        pos = dis(rng);
    } while (std::abs(pos - playerPos) <= margin);

    return pos;
}

float randomPositionLeft(float playerPos, float margin){
    std::uniform_real_distribution<float> dis(8.f, 16.f);

    return dis(rng);
}

float randomPositionRight(float playerPos, float margin){
    std::uniform_real_distribution<float> dis(240.f, 248.f);

    return dis(rng);
}
