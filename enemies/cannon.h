#pragma once

#include "enemy.h"

class Cannon : public Enemy
{
private:

    // Combat stats
    const float CANNON_LIFE = 1.0f;
    const float CANNON_SCORE = 200.0f;
    const float CANNON_DAMAGE = 2.0f;


public:
    int level; // Current game level
    int stage; // Current stage

    Cannon() = default;
    Cannon(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset cannon to initial state
    void resetPosition() override;

    // Render cannon and debug info
    void draw(sf::RenderWindow &window) override;

    // Update cannon logic (spawn, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Update detection area
    void updateVisionField();

    void hello() const override;
};