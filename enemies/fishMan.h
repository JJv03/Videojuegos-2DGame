#pragma once

#include "enemy.h"

class FishMan : public Enemy
{
private:
    // Movement and combat constants
    const sf::Vector2f FISHMAN_SPEED = {-50.0f, 0.0f};
    const float FISHMAN_LIFE = 1.0f;
    const float FISHMAN_SCORE = 300.0f;
    const float FISHMAN_DAMAGE = 2.0f;

    // Animation settings
    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    // Spawner variables
    sf::FloatRect spawnZone;
    bool spawnerActive{false};
    bool fishManToSpawn{false};
    float spawnTime = 0.0f;
    float fishManSpawnTimers = 0.0f;

    // Animation tracking
    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    size_t level; // Current game level
    size_t stage; // Current stage within level

    FishMan() = default;
    FishMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
            const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage);

    // Reset fishman to initial state
    void resetPosition() override;

    // Move fishman to spawn position
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds);

    // Render fishman and debug info
    void draw(sf::RenderWindow &window, bool debugDraw) override;

    // Update fishman logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone,
                const sf::FloatRect &playerDeactivationZone, const sf::FloatRect &playerBounds);

    // Handle collisions
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const bool playerIsAtacking, const float playerDamage);

    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};