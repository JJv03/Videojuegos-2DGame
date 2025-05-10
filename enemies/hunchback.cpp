#include "hunchback.h"
#include <iostream>
#include <cmath>
#include <array>
#include <random>
#include "../game.h"

// static std::random_device rd;
// static std::mt19937 gen(rd());


namespace {     // Only visible in this file
    const float HIGH_SPEED_X = 70.0f;
    const float SMALL_SPEED_X = 125.0f;
    const float HIGH_JUMP_SPEED = 300.0f;
    const float SMALL_JUMP_SPEED = 90.0f;
    const float LIFE = 1.0f;
    const float SCORE = 400.0f;
    const float DAMAGE = 3.0f;
    const float DISTANCE_TO_PLAYER_Y = 14.f;
    const float DISTANCE_TO_PLAYER_X = 32.f;
    const float ON_GROUND_TIME = 0.2f;
    const float INIT_TIME = 2.0f;
    // const float ESCAPE_PROB = 0.25f;

    std::array<sf::IntRect, 2> textureRects {
        sf::IntRect(sf::Vector2(200, 75), sf::Vector2(16, 16)), // Hunchback still
        sf::IntRect(sf::Vector2(217, 75), sf::Vector2(16, 16))  // Hunchback jumping
    };

    std::vector<AnimationManager::Frame> shakeFrames{
        AnimationManager::Frame{textureRects[0], 0.2f},
        AnimationManager::Frame{textureRects[1], 0.2f},
        AnimationManager::Frame{textureRects[0], 0.2f},
        AnimationManager::Frame{textureRects[1], 0.2f},
        AnimationManager::Frame{textureRects[0], 0.2f},
        AnimationManager::Frame{textureRects[1], 0.2f},
        AnimationManager::Frame{textureRects[0], 0.2f},
        AnimationManager::Frame{textureRects[1], 0.2f}
    };
}


Hunchback::Hunchback(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
                     const int &level, const int &stage): Enemy(_sprite, _hitboxes),
                     onGround_TimeCounter(0.f), moveRight(true), level(level), stage(stage)//, atTheEdge(false)
{
    speed = sf::Vector2f{SMALL_SPEED_X, 0.0f};
    life = LIFE;
    score = SCORE;
    damage = DAMAGE;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Hunchback AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(hunchbackShake, shakeFrames, false);
    this->animationManager = animationManager;
    currentAnimation = hunchbackShake;
    animationManager->playAnimation(currentAnimation);

    sprite->setTextureRect(textureRects[0]);
    sprite->setScale({-1.0f, 1.0f});
    currentState = State::INIT;
}


void Hunchback::update(float deltaTime, const sf::FloatRect &playerActivationZone,
                       const sf::FloatRect &playerDeactivationZone, const sf::Vector2f &playerPos,
                       const sf::FloatRect &mapBounds, const Player* player)
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
        //sf::Vector2f position = sprite->getGlobalBounds().position;
        
        if (checkMapBoundaries(mapBounds))
            return;
        
        if(currentState != State::JUMPING && currentState != State::INIT){
            if (this->moveRight) {
                //std::cout << "Move right" << std::endl;
                sprite->setScale({-1.f, 1.f});
            } else {
                //std::cout << "Move left" << std::endl;
                sprite->setScale({1.f, 1.f});
            }
        }
        
        

        // STATE MACHINE
        //std::cout << "State: " << static_cast<int>(currentState) << std::endl;
        switch(currentState) {
            case State::INIT:
                updateAnimation(deltaTime);

                if (animationManager->isAnimationFinished()) {
                    currentState = State::ON_GROUND;
                    currentAnimation = noAnimation;
                    sprite->setTextureRect(textureRects[0]);
                }
                
                break;
            
            case State::ON_GROUND: {     // Decide where to jump
                sf::FloatRect playerBounds = player->getBounds()[0];
                sf::FloatRect enemyBounds = sprite->getGlobalBounds();
                
                // point towards player
                this->moveRight = ((playerPos.x + playerBounds.size.x * 0.5f) >
                                  (enemyBounds.position.x + enemyBounds.size.x * 0.5f));
                
                onGround_TimeCounter += deltaTime;
                if(onGround_TimeCounter >= ON_GROUND_TIME){
                    onGround_TimeCounter = 0.f;
                    currentState = State::JUMPING;
                    sprite->setTextureRect(textureRects[1]);
                    isOnGround = false;
                    
                    if (playerIsLookingAtMe(moveRight, player)) {
                        speed = sf::Vector2f{HIGH_SPEED_X, HIGH_JUMP_SPEED};
                    } else {
                        speed = sf::Vector2f{SMALL_SPEED_X, SMALL_JUMP_SPEED};
                    }
                }
                
                break;
            }
            case State::JUMPING:
                if (speed.y <= 0.f && isOnGround) {
                    currentState = State::ON_GROUND;
                    speed.y = 0.f;
                    sprite->setTextureRect(textureRects[0]);
                }
                else {
                    isOnGround = false;
                    onlyApplyGravity(deltaTime);
                    //float moveX = speed.x * deltaTime;
                    float moveX = (moveRight ? speed.x : -speed.x) * deltaTime;
                    float moveY = -speed.y * deltaTime;
                    
                    //std::cout << "Jumping: " << moveX << ", " << moveY << ", " << moveRight << std::endl;
                    //std::cout << "Scale: " << sprite->getScale().x << std::endl;
                    sprite->move({moveX, moveY});
                    for (auto &hitbox : hitboxes) {
                        hitbox.position.x += moveX;
                        hitbox.position.y += moveY;
                    }
                }
                break;

            default:
                break;
        }

        if(currentState != State::JUMPING) {
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
    }
    
}

bool Hunchback::playerIsLookingAtMe(const bool moveRight, const Player* player) const {
    sf::FloatRect playerBounds = player->getBounds()[0];

    bool lookingAtEachOther = (moveRight && player->dir == PlayerDirection::LEFT) ||
                              (!moveRight && player->dir == PlayerDirection::RIGHT);
    bool playerIsHigherOrSameHeight = (playerBounds.position.y + DISTANCE_TO_PLAYER_Y) <= sprite->getPosition().y;
    bool playerIsInRangeForHighJump = std::abs(playerBounds.position.x + playerBounds.size.x * 0.5f
                                                - sprite->getPosition().x) <= DISTANCE_TO_PLAYER_X;
    
    return lookingAtEachOther && playerIsHigherOrSameHeight && playerIsInRangeForHighJump;
}

void Hunchback::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);
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
    // else if (Player *player = dynamic_cast<Player *>(&other))
    // {

    // }
}

void Hunchback::onCollision_SolidTile(Entity &solidTile)
{
    sf::FloatRect enemyHitbox = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    for (auto tileBound : tileBounds)
    {
        if (tileBound.size.x == 0 || tileBound.size.y == 0)
            continue;

        if (const std::optional<sf::FloatRect> intersection = enemyHitbox.findIntersection(tileBound))
        {
            float overlapX = intersection->size.x;
            float overlapY = intersection->size.y;

            const float minHeightThreshold = 0.25f;

            // Handle vertical collision (feet)
            if (overlapX >= overlapY || overlapY <= minHeightThreshold)
            {
                if (enemyHitbox.position.y < (tileBound.position.y + tileBound.size.y * 0.5f))
                {
                    if(overlapY >= 0.01f && speed.y < 0.f){
                        sprite->move({0.f, -overlapY + 0.01f});
                        for (auto &h : hitboxes){
                            h.position.y -= overlapY;
                            h.position.y += 0.01f;
                        }
                    }
                    
                    isOnGround = true;
                }
            }
        }
    }

    if(speed.y == 0.f){
        isOnGround = true;
    }
}

void Hunchback::resetPosition()
{
    Enemy::resetPosition();

    speed = sf::Vector2f{SMALL_SPEED_X, 0.0f};
    life = LIFE;
    score = SCORE;
    damage = DAMAGE;

    onGround_TimeCounter = 0.f;

    moveRight = true;

    currentAnimation = hunchbackShake;
    currentState = State::INIT;

    sprite->setScale({-1.0f, 1.0f});
}

void Hunchback::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
}

void Hunchback::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    animationManager->update(deltaTime);
}

void Hunchback::hello() const
{
    std::cout << "Soy Hunchback" << std::endl;
}