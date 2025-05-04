#pragma once

#include "boss.h"
#include "../enemies/createProjectile.h"


class DraculaSpirit : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f DRACULASPIRIT_SPEED = {0.0f, 0.0f};
    const float DRACULASPIRIT_LIFE = 48.0f;
    const float DRACULASPIRIT_SCORE = 0.0f;
    const float DRACULASPIRIT_DAMAGE = 4.0f;
    bool reproduced = false;


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
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 148), sf::Vector2(48, 64)), 0.1f}
    };
    std::vector<AnimationManager::Frame> landDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(246, 148), sf::Vector2(48, 48)), 0.1f}
    };
    std::vector<AnimationManager::Frame> fireDraculaSpiritFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(295, 148), sf::Vector2(48, 80)), 1.5f}
    };

public:
    int level; // Current game level
    int stage; // Current stage within level
    sf::Vector2f position;
    sf::FloatRect mapDims;

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
    float waitingInFly = 0.05f;

    float waitingIdleCounter = 0.f;
    float waitingInReadyCounter = 0.f;
    float waitingInSteadyCounter = 0.f;
    float waitingInFlyCounter = 0.f;

    float facingRight = 1;
    float directionFlying = 1;
    bool flyDone = false;

    float fireCounter = 0.f;
    float fireTimer = 1.0f;
    // Time wait next fire
    bool hasFired = false;
    float fireNextCounter = 0.f;
    float fireNextTimer = 5.0f;

    // Init
    bool hasInitJump = false;

    // Projectile
    std::shared_ptr<Projectile> projectile;
    bool hasProjectile{false};

    std::shared_ptr<Projectile> projectile2;
    bool hasProjectile2{false};

    std::shared_ptr<Projectile> projectile3;
    bool hasProjectile3{false};


    // Projectile IA
    bool firstTimeDetected = false;

    //Dodging IA
    bool alreadyChanged = false;
    bool alreadyFlying = false;
    DraculeSpiritState currentState = DraculeSpiritState::ASLEEP;

    DraculaSpirit() = default;
    DraculaSpirit(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position, const int &level, const int &stage, const sf::FloatRect &mapDims);

    // Render draculaSpirit and debug info
    void draw(sf::RenderWindow &window) override;

    // Update draculaSpirit logic (spawn, movement, etc.)
    void update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    std::shared_ptr<Projectile> getProjectile() const { return projectile; }
    std::shared_ptr<Projectile> getProjectile2() const { return projectile2; }
    std::shared_ptr<Projectile> getProjectile3() const { return projectile3; }
    void hello() const override;
};
