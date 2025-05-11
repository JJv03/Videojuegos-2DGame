#pragma once

#include "boss.h"
#include "createScythe.h"

class Death : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f DEATH_SPEED = {0.0f, 0.0f};   // By default static
    const float DEATH_LIFE = 48.0f;
    const float DEATH_SCORE = 7000.0f;
    const float DEATH_DAMAGE = 4.0f;

    bool dead = false;

    bool starting = true;
    float timer = 0.f;
    const float sleepInterval = 2.f;

    sf::Vector2f goal = sf::Vector2f(0, 0);
    sf::Vector2f startPosition;
    float arcHeight = 35.f;

    float doubleMoveTimer = 0.f;
    const float moveInterval = 2.5f;
    const float waitInterval = 2.f;
    const float attackInterval = 1.5f;

    bool goingDown = true;

    bool up = true;
    bool right = false;

    // bool enhancedActivated = false;
    // float enhancedTimer = 0.f;
    // const float enhancedInterval = 1.5f;
    // const float enhancedSpeed = 0.5f;
    bool triedAI = false;
    bool startingMove = false;
    bool playerClose = false;
    bool playerAway = false;

public:
    int level; // Current game level
    int stage; // Current stage within level
    sf::Vector2f position;
    sf::FloatRect mapDims;

    enum class State {
        WAITING,
        MOVING,
        ATTACKING
    };

    int weights[3] = {1, 1, 1};

    State currentState = State::WAITING;

    // Up to 5 scythes at screen
    std::array<std::shared_ptr<Scythe>, 5> scythes;
    bool generated = false;

    // Holy water handling
    bool collisionWithHolyWater = false;
    float paralizeCounter = 0.7f;
    const float paralizeTimer = 0.6f;

    Death() = default;
    Death(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
               const int &level, const int &stage, const sf::FloatRect &mapDims);

    // Render phantomBat and debug info
    void draw(sf::RenderWindow &window) override;

    // Update phantomBat logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void randomObjective();

    void getLinelSpeed(float timeToMove);

    void getDoubleSpeed();

    // Reset phantomBat to initial state
    void resetPosition() override;

    void selectObjective();

    sf::Vector2f getRandomScythesPos(const sf::Vector2f &playerPos);

    void generateScythes(const sf::Vector2f &playerPos, const sf::FloatRect &mapDims);

    void selectNewState();
    
    void updateWeights();

    void hello() const override;
};
