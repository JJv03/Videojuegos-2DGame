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
    bool attacking = false;
    bool moving = false;

    bool up = true;
    bool right = false;

    bool enhancedActivated = false;
    float enhancedTimer = 0.f;
    const float enhancedInterval = 1.5f;
    const float enhancedSpeed = 0.5f;
    bool triedAI = false;

public:
    int level; // Current game level
    int stage; // Current stage within level
    sf::Vector2f position;
    sf::FloatRect mapDims;

    // Up to 5 scythes at screen
    std::array<std::shared_ptr<Scythe>, 5> scythes;
    bool generated = false;

    Death() = default;
    Death(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
               const int &level, const int &stage, const sf::FloatRect &mapDims);

    // Render phantomBat and debug info
    void draw(sf::RenderWindow &window) override;

    // Update phantomBat logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void randomObjective();

    void getLinelSpeed(float timeToMove);

    void getDoubleSpeed();

    // Reset phantomBat to initial state
    void resetPosition() override;

    void selectObjective();

    void enhancedAI(bool isOn, const int playerDir, const sf::FloatRect &playerBounds);

    void generateScythes(const sf::Vector2f &playerPos, const sf::FloatRect &mapDims);

    void hello() const override;
};
