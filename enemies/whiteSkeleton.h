#pragma once

#include "enemy.h"

class WhiteSkeleton : public Enemy
{
private:
    // Combat stats
    const sf::Vector2f WHITE_SKELETON_SPEED = {120.0f, 0.0f};
    const float WHITE_SKELETON_LIFE = 1.0f;
    const float WHITE_SKELETON_SCORE = 400.0f;
    const float WHITE_SKELETON_DAMAGE = 3.0f;

    const float DISTANCE_TO_PLAYER = 3 * 32;

    enum class State
    {
        IDLE,
        ATTACKING,
        WALKINGCLOSE,
        WALKINGAWAY,
    };

    const float WALK_TIME = 0.25f;
    float walkTimeCounter;

    const float IDLE_TIME = 0.05f;
    float idleTimeCounter;

    const float ATTACK_TIME = 0.25f;
    float attackTimeCounter;

    bool atTheEdge;
    bool isPlayerRight;


    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };

    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(349, 75), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> walkAttackFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(349, 75), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(366, 75), sf::Vector2(16, 32)), 0.1f}
    };


public:


    int level; // Current game level
    int stage; // Current stage

    State currentState = State::IDLE;

    WhiteSkeleton() = default;
    WhiteSkeleton(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);
    
    // Reset skeleton to initial state
    void resetPosition() override;

    // Render skeleton and debug info
    void draw(sf::RenderWindow &window) override;

    // Update skeleton logic (spawn, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};