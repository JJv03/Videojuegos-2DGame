#pragma once

#include "enemy.h"

class Medusa : public Enemy
{
private:
    // Movement and combat constants
    const sf::Vector2f MEDUSA_SPEED = {-85.0f, 0.0f};
    const float MEDUSA_LIFE = 1.0f;
    const float MEDUSA_SCORE = 300.0f;
    const float MEDUSA_DAMAGE = 2.0f;

    // Oscillation (up-down movement) settings
    const float OSCILLATION_SPEED = 3.0f;
    const float OSCILLATION_AMPLITUDE = 95.0f;

    // Spawner variables
    sf::FloatRect spawnZone;
    bool spawnerActive{false};
    bool medusaToSpawn{false};
    float spawnTime = 0.0f;
    float medusaSpawnTimers = 0.0f;

public:
    int level; // Current game level
    int stage; // Current stage within level

    Medusa() = default;
    Medusa(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
           const sf::Vector2f &zoneSize, const int &level, const int &stage);

    // Reset medusa to initial state
    void resetPosition() override;

    // Move medusa to spawn position
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds, bool spawnFromRight);

    // Render medusa and debug info
    void draw(sf::RenderWindow &window) override;

    // Update medusa logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const float playerDir, const sf::FloatRect &playerBounds, const PlayerPosition playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Reset de spawn process
    void resetSpawnState();

    void hello() const override;
};