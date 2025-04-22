#pragma once

#include "boss.h"

class PhantomBat : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f PBAT_SPEED = {-75.0f, 0.0f};
    const float PBAT_LIFE = 12.0f;
    const float PBAT_SCORE = 3000.0f;
    const float PBAT_DAMAGE = 2.0f;

    bool starting = true;

    float sleepTimer = 0.f;
    const float sleepInterval = 2.f;

    std::vector<AnimationManager::Frame> idlePhBatFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 11), sf::Vector2(16, 16)), 0.1f}
    };
    std::vector<AnimationManager::Frame> flyPhBatFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(18, 11), sf::Vector2(48, 24)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(67, 11), sf::Vector2(32, 24)), 0.15f}
    };

public:
    int level; // Current game level
    int stage; // Current stage within level

    PhantomBat() = default;
    PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
               const int &level, const int &stage);

    // Render phantomBat and debug info
    void draw(sf::RenderWindow &window) override;

    // Update phantomBat logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};
