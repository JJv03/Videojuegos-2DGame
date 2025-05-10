#pragma once

#include "enemy.h"

class RedSkeleton : public Enemy
{
private:

    // Combat stats
    const sf::Vector2f RED_SKELETON_SPEED = {60.0f, 0.0f};
    const float RED_SKELETON_LIFE = 1.0f;
    const float RED_SKELETON_SCORE = 400.0f;
    const float RED_SKELETON_DAMAGE = 3.0f;

    const float DISTANCE_TO_PLAYER = 3 * 32;
    
    enum class State
    {
        DEAD,
        RESPAWNING,
        IDLE,
        WALKING,
        DESPAWNING,
    };


    const float WALK_TIME = 0.5f;
    float walkTimeCounter;

    const float IDLE_TIME = 1.0f;
    float waitingIdleTimeCounter;

    const float DEAD_TIME = 2.0f;
    float waitingDeadTimeCounter;

    const float SPAWN_TIME = 0.5f;
    float waitingRespawnTimeCounter;
    float waitingDespawnTimeCounter;

    bool moveRight;
    bool atTheEdge;
    
    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };

    std::vector<AnimationManager::Frame> deadFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(157, 122), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> spawnFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(174, 122), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(191, 122), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> walkingFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(191, 122), sf::Vector2(16, 32)), 0.2f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(208, 122), sf::Vector2(16, 32)), 0.3f}
    };

public:
    int level; // Current game level
    int stage; // Current stage

    State currentState = State::DEAD;

    RedSkeleton() = default;
    RedSkeleton(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset cannon to initial state
    void resetPosition() override;

    // Render cannon and debug info
    void draw(sf::RenderWindow &window) override;

    // Update cannon logic (spawn, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    std::vector<sf::FloatRect> getBounds() const override;

    void hello() const override;
};