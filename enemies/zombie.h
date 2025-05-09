#pragma once

#include "enemy.h"

class Zombie : public Enemy
{
private:
    // Movement constant
    const sf::Vector2f ZOMBIE_SPEED = {-50.0f, 0.0f};

    // Combat stats
    const float ZOMBIE_LIFE = 1.0f;
    const float ZOMBIE_SCORE = 100.0f;
    const float ZOMBIE_DAMAGE = 2.0f;

    std::vector<AnimationManager::Frame> zombieWalkFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 28), sf::Vector2(16, 32)), 0.2f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(18, 28), sf::Vector2(16, 32)), 0.2f},
    };
    std::vector<AnimationManager::Frame> emptyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 28), sf::Vector2(0, 0)), 0.1f},
    };

public:
    Zombie() = default;
    Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    // Reset zombie to initial state
    void resetPosition() override;

    // Position for spawning
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, float dist, bool fromRight);
    void setDirection(float dir);

    // Main update loop
    void update(float deltaTime);

    // Handle collisions
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};