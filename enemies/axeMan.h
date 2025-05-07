#pragma once

#include "enemy.h"

class AxeMan : public Enemy
{
private:
    // Movement constants
    const sf::Vector2f AXEMAN_SPEED = {0.0f, 0.0f};

    // Combat stats
    const float AXEMAN_LIFE = 13.5f;
    const float AXEMAN_SCORE = 1400.0f;
    const float AXEMAN_DAMAGE = 4.0f;

    const float distanceToPlayer = 3 * 32; // Num of tiles * tiles dimensions

    std::vector<AnimationManager::Frame> axeMovementFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(240, 122), sf::Vector2(24, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(265, 122), sf::Vector2(24, 32)), 0.1f}
    };

public:
    int level; // Current game level
    int stage; // Current stage

    AxeMan() = default;
    AxeMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset ghost to initial state
    void resetPosition() override;

    // Render ghost and debug info
    void draw(sf::RenderWindow &window) override;

    // Update ghost logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};