#pragma once
#include "enemy.h"


class Hunchback : public Enemy
{
private:
    enum class State
    {
        INIT,
        ON_GROUND,
        JUMPING,
        ESCAPE,
    };

    float onGround_TimeCounter;     // Time counter for on ground state

    bool moveRight;     // Direction of movement

public:
    int level;  // Current game level
    int stage;  // Current stage

    State currentState = State::INIT;

    Hunchback() = default;
    Hunchback(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset Hunchback to initial state
    void resetPosition() override;

    // Render Hunchback and debug info
    void draw(sf::RenderWindow &window) override;

    // Update Hunchback logic (spawn, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds, const Player* player);

    bool playerIsLookingAtMe(const bool moveRight, const Player* player) const;

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;
    
    // Handle collision with solid tiles
    void onCollision_SolidTile(Entity &solidTile);

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};