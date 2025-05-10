#pragma once

#include "enemy.h"

class Crow : public Enemy
{
private:
    // Movement constants
    const sf::Vector2f CROW_SPEED = {0.0f, 0.0f};
    const float CROW_JUMP_SPEED = 300.0f;

    // Combat stats
    const float CROW_LIFE = 1.0f;
    const float CROW_SCORE = 200.0f;
    const float CROW_DAMAGE = 2.0f;

    // Vision and AI
    const float VISION_RANGE = 75.0f;
    sf::FloatRect visionField;
    bool playerDetected = false;
    bool hasRedirected = false;
    sf::Vector2f playerPosition;
    bool isPlayerRight = false;

    std::vector<AnimationManager::Frame> idleCrowFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(277, 75), sf::Vector2(16, 16)), 0.1f}
    };
    std::vector<AnimationManager::Frame> flyCrowFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(294, 75), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(311, 75), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(328, 75), sf::Vector2(16, 16)), 0.1f}
    };

    enum class State
    {
        MOVING,
        WAITING,
        POSITIONING,
        ATTACK
    };
    bool startingMoving = false;
    bool startingPositioning = false;
    bool attackPositioning = false;

    bool origDirRight = false;

    sf::Vector2f goal;
    sf::Vector2f position;

    float timerMove = 0.f;
    float timerWait = 0.f;
    float timerPositioning = 0.f;
    const float interval = 2.0f;
    const float waitInterval = 1.5f;
    
public:
    int level; // Current game level
    int stage; // Current stage

    State currentState = State::MOVING;
    State prevState = State::MOVING;

    Crow() = default;
    Crow(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset crow to initial state
    void resetPosition() override;

    // Render crow and debug info
    void draw(sf::RenderWindow &window) override;

    // Update crow logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds, const sf::FloatRect &mapBounds);
    
    void getLinelSpeed(float timeToMove);

    void getRandomGoal(const sf::Vector2f &playerPos);

    void getPlayerGoal(const sf::Vector2f &playerPos);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Update detection area
    void updateVisionField();

    void hello() const override;
};