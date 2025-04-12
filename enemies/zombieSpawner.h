#pragma once

#include "createEnemies.h"
#include <vector>
#include <random>
#include <memory>

class ZombieSpawner
{
private:
    // Spawn zone properties
    sf::Vector2f spawnPosition;
    sf::FloatRect spawnZone;

    // Spawner state flags
    bool allZombiesInactive{true};
    bool spawnerActive{false};

    // Spawn positioning and timing
    float dist = 0.0f;
    float zombieSpawnTimers = 0.0f;

    // Random number generation
    std::mt19937 &rng;
    std::uniform_int_distribution<int> zombieCountDist;

    // Spawn queue
    std::vector<bool> zombiesToSpawn;

public:
    std::vector<Zombie*> zombies; // Managed zombie instances

    size_t level; // Current game level
    size_t stage; // Current stage

    ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage, std::mt19937 &rngReference);

    // Initialize zombies
    void init();

    // Main update with spawn logic
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);

    // Handle collisions for all active zombies
    void checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);

    // Render zombies and debug info
    void draw(sf::RenderWindow &window, bool debugDraw = false);
};