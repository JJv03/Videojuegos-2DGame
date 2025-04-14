#pragma once

#include "enemy.h"
#include <random>

class FishMan : public Enemy
{
private:
    // Movement and combat constants
    const sf::Vector2f FISHMAN_SPEED = {-50.0f, 0.0f};
    const float FISHMAN_LIFE = 1.0f;
    const float FISHMAN_SCORE = 300.0f;
    const float FISHMAN_DAMAGE = 2.0f;
    const float JUMP_VELOCITY = 425.0f;

    // Animation settings
    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    // Attack settings
    const float ATTACK_INTERVAL = 2.5f;
    const float ATTACK_PAUSE_TIME = 1.0f;

    // State management
    enum class State
    {
        SPAWNING,
        JUMPING,
        WALKING,
        PAUSED_FOR_ATTACK
    };

    State currentState = State::SPAWNING;

    // Spawner variables
    sf::FloatRect spawnZone;
    bool spawnerActive{false};
    bool fishManToSpawn{false};
    float spawnTime = 0.0f;
    float fishManSpawnTimers = 0.0f;

    // Attack and movement timing
    float attackTimer = 0.0f;
    float pauseTimer = 0.0f;

    // Animation tracking
    float animTimer = 0.0f;
    int currentFrame = 0;

    // Generador de números aleatorios
    std::mt19937 &rng;

public:
    int level; // Current game level
    int stage; // Current stage within level

    FishMan() = default;
    FishMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
            const sf::Vector2f &zoneSize, const int &level, const int &stage, std::mt19937 &rngReference);

    // Reset fishman to initial state
    void resetPosition() override;

    // Move fishman to spawn position - updated to use random position
    void moveToSpawnPosition(const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerBounds);

    // Render fishman and debug info
    void draw(sf::RenderWindow &window, bool debugDraw) override;

    // Update fishman logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone,
                const sf::FloatRect &playerDeactivationZone, const sf::FloatRect &playerBounds);

    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const bool playerIsAtacking, const float playerDamage);

    // Handle collisions - only with ground
    void onCollision(Entity &other, Game &game) override;

    // Face the player direction
    void facePlayer(const sf::FloatRect &playerBounds);

    // Enter attack mode
    void startAttackPause();

    // Initiate the jump when spawning
    void startJump();

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};
