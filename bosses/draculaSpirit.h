#pragma once

#include "boss.h"

class DraculaSpirit : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f DRACULASPIRIT_SPEED = {0.0f, 0.0f};
    const float DRACULASPIRIT_LIFE = 48.0f;
    const float DRACULASPIRIT_SCORE = 0.0f;
    const float DRACULASPIRIT_DAMAGE = 4.0f;


    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };
    std::vector<AnimationManager::Frame> idleDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 148), sf::Vector2(48, 64)), 0.1f}
    };
    std::vector<AnimationManager::Frame> readyDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 148), sf::Vector2(48, 64)), 0.1f}
    };
    std::vector<AnimationManager::Frame> steadyDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(99, 148), sf::Vector2(48, 64)), 0.1f}
    };
    std::vector<AnimationManager::Frame> goDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(148, 148), sf::Vector2(48, 80)), 0.1f}
    };
    std::vector<AnimationManager::Frame> flyingDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 148), sf::Vector2(48, 64)), 0.2f}
    };
    std::vector<AnimationManager::Frame> landDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(246, 148), sf::Vector2(48, 48)), 0.1f}
    };
    std::vector<AnimationManager::Frame> fireDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(295, 148), sf::Vector2(48, 80)), 0.1f}
    };

public:
    int level; // Current game level
    int stage; // Current stage within level

    enum class DraculeSpiritState
    {
        ASLEEP,
        IDLE,
        READY,
        STEADY,
        GO,
        FLYING,
        LANDING,
        FIRE
    };

    float waitingIdle = 0.5f;
    float waitingInReady = 0.5f;
    float waitingInSteady = 0.5f;
    float waitingInFly = 0.2f;

    float waitingIdleCounter = 0.f;
    float waitingInReadyCounter = 0.f;
    float waitingInSteadyCounter = 0.f;
    float waitingInFlyCounter = 0.f;

    float facingRight = 1;
    float directionFlying = 1;
    bool flyDone = false;

    DraculeSpiritState currentState = DraculeSpiritState::ASLEEP;

    DraculaSpirit() = default;
    DraculaSpirit(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position, const int &level, const int &stage);

    // Render draculaSpirit and debug info
    void draw(sf::RenderWindow &window) override;

    // Update draculaSpirit logic (spawn, movement, etc.)
    void update(float deltaTime, const Player &player, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};
