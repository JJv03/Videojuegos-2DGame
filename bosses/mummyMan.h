#pragma once

#include "boss.h"
#include "createBandage.h"


class MummyMan : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f MUMMY_SPEED = {30.0f, 0.0f};   // By default static
    const float MUMMY_LIFE = 1.0f;
    const float MUMMY_SCORE = 3000.0f;
    const float MUMMY_DAMAGE = 2.0f;

    

    bool starting = true;
    sf::Vector2f goal = sf::Vector2f(0, 0);

    bool attacking = false;

    sf::Vector2f startPosition;

    bool triedAI = false;

    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };

    std::vector<AnimationManager::Frame> idleMummyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(274, 19), sf::Vector2(16, 40)), 0.1f}
    };
    std::vector<AnimationManager::Frame> walkMummyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(274, 20), sf::Vector2(16, 39)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(291, 19), sf::Vector2(16, 40)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(308, 20), sf::Vector2(16, 39)), 0.15f}
    };

    std::vector<AnimationManager::Frame> attackkMummyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(325, 20), sf::Vector2(16, 39)), 0.65f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(342, 19), sf::Vector2(16, 40)), 0.65f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(359, 20), sf::Vector2(16, 39)), 0.65f}
    };

public:
    int level; // Current game level
    int stage; // Current stage within level
    sf::Vector2f position;
    sf::FloatRect mapDims;

    enum class MummyState
    {
        IDLE,
        WALK,
        ATTACK
    };

    MummyState currentState = MummyState::IDLE;

    float facingRight = 1.f;

    // Timer wait in idle state
    float counterStarting = 0.f;
    float time2Start = 2.f;
    bool started = false;
    bool firstTime = true;

    // Timer to attack
    float attackWaitingCounter = 0.f;
    float attackWaitingTime = 3.5f;


    float actionTimer = 0.f;
    float nextActionTime = 1.f + static_cast<float>(rand() % 1000) / 500.f; // entre 1 y 3 seg
    int currentAction = 0; // 0: caminar, 1: atacar
    bool directionChanged = false;
    // Bandage
    std::shared_ptr<Bandage> bandage;
    bool lanzado = false;

    // Fix death of other mummy
    bool isOtherDead = false;
    bool dead = false;
    
    int id;
    // IA 
    int weights[2] = {1, 1};
    bool playerClose = false;
    bool playerAway = false;
    float speedFire = 80.f;

    MummyMan() = default;
    MummyMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
               const int &level, const int &stage, const sf::FloatRect &mapDims);

    // Render MummyMan and debug info
    void draw(sf::RenderWindow &window) override;

    // Update MummyMan logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds,const Player &player);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Reset phantomBat to initial state
    void resetPosition() override;

    void objectivePlayer(const sf::FloatRect &playerBounds);

    void enhancedAI(bool isOn, const int playerDir, const sf::FloatRect &playerBounds);
    
    void selectNewState();
    
    void updateWeights();
    void hello() const override;
};
