#pragma once

#include "enemy.h"

class Bat : public Enemy
{
private:
    // Movement and combat constants
    const sf::Vector2f BAT_SPEED = {-75.0f, 0.0f};
    const float BAT_LIFE = 1.0f;
    const float BAT_SCORE = 200.0f;
    const float BAT_DAMAGE = 2.0f;

    // Oscillation (up-down movement) settings
    const float OSCILLATION_SPEED = 2.5f;
    const float OSCILLATION_AMPLITUDE = 0.5f;

    // Spawner variables
    sf::FloatRect spawnZone;
    bool spawnerActive{false};
    bool batToSpawn{false};
    float spawnTime = 0.0f;
    float batSpawnTimers = 0.0f;

    std::vector<AnimationManager::Frame> flyBatFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(184, 11), sf::Vector2(16, 16)), 0.125f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(201, 11), sf::Vector2(16, 16)), 0.125f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(218, 11), sf::Vector2(16, 16)), 0.125f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(201, 11), sf::Vector2(16, 16)), 0.125f},
    };
    std::vector<AnimationManager::Frame> emptyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(184, 11), sf::Vector2(0, 0)), 0.1f},
    };

public:
    int level; // Current game level
    int stage; // Current stage within level

    Bat() = default;
    Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
        const sf::Vector2f &zoneSize, const int &level, const int &stage);

    // Reset bat to initial state
    void resetPosition() override;

    // Move bat to spawn position
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds, bool spawnFromRight);

    // Render bat and debug info
    void draw(sf::RenderWindow &window) override;

    // Update bat logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Reset de spawn process
    void resetSpawnState();

    void hello() const override;
};