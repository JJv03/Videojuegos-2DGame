#pragma once

#include "enemy.h"
#include "projectile.h"

class Cannon : public Enemy
{
private:

    // Combat stats
    const sf::Vector2f CANNON_SPEED = {0.0f, 0.0f};
    const float CANNON_LIFE = 8.0f;
    const float CANNON_SCORE = 400.0f;
    const float CANNON_DAMAGE = 2.0f;

    const float PROJECTILE_SPEED = 120.0f;
    const float PROJECTILE_DAMAGE = 2.0f;
    
    enum class State
    {
        WAITING_FIRST_ATTACK,
        CHARGING,
        FIRST_ATTACK,
        WAITING_SECOND_ATTACK,
        SECOND_ATTACK,
    };

    const float MIN_WAITING_FIRST_ATTACK_TIME = 1.5f;
    const float MAX_WAITING_FIRST_ATTACK_TIME = 4.0f;
    float WAITING_FIRST_ATTACK_TIME;
    float waitingFirstAttackTimeCounter;

    const float CHARGING_TIME = 0.7f;
    float chargingTimeCounter;

    const float WAITING_SECOND_ATTACK_TIME = 1.0f;
    float waitingSecondAttackTimeCounter;

    bool facingRight;
    bool isPlayerRight;

    
    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };

    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(40, 75), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> chargingFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(57, 75), sf::Vector2(16, 32)), 0.1f}
    };

public:
    int level; // Current game level
    int stage; // Current stage

    State currentState = State::WAITING_FIRST_ATTACK;

    std::array<std::shared_ptr<Projectile>, 2> projectiles;

    Cannon() = default;
    Cannon(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset cannon to initial state
    void resetPosition() override;

    // Render cannon and debug info
    void draw(sf::RenderWindow &window) override;

    // Update cannon logic (spawn, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void generateProjectile(bool secondAttack);
    
    // Update projectile logic
    void updateProjectiles(float deltaTime, const sf::FloatRect& mapBounds);

    float randomWaitTime();

    void hello() const override;
};