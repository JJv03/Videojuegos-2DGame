#pragma once

#include "enemy.h"

class PhantomBat : public Enemy
{
private:
    // Movement and combat constants
    const sf::Vector2f PBAT_SPEED = {-75.0f, 0.0f};
    const float PBAT_LIFE = 1.0f;
    const float PBAT_SCORE = 200.0f;
    const float PBAT_DAMAGE = 2.0f;

    // Animation settings
    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    // Oscillation (up-down movement) settings
    const float OSCILLATION_SPEED = 2.5f;
    const float OSCILLATION_AMPLITUDE = 0.5f;

    // Spawner variables
    sf::FloatRect spawnZone;
    bool spawnerActive{false};
    bool phantomBatToSpawn{false};
    bool phantomBatSpawned{false};
    float spawnTime = 0.0f;
    float batSpawnTimers = 0.0f;

    // Animation tracking
    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    int level; // Current game level
    int stage; // Current stage within level

    PhantomBat() = default;
    PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
        const sf::Vector2f &zoneSize, const int &level, const int &stage);

    // Reset phantomBat to initial state
    void resetPosition() override;

    // Move phantomBat to spawn position
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds);

    // Render phantomBat and debug info
    void draw(sf::RenderWindow &window) override;

    // Update phantomBat logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone,
                const sf::FloatRect &playerDeactivationZone, const sf::FloatRect &playerBounds);

    // Handle collisions
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds, const bool playerIsAtacking, const float playerDamage);
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};