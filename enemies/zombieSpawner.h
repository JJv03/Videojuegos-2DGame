#pragma once

#include "createEnemies.h"
#include <vector>
#include <random>
#include <memory>

class ZombieSpawner
{
private:
    sf::Vector2f spawnPosition;
    sf::FloatRect spawnZone;

    bool allZombiesInactive{true};
    bool spawnerActive{false};

    float dist = 0.0f;
    float zombieSpawnTimers = 0.0f;

    std::mt19937 &rng;
    std::uniform_int_distribution<int> zombieCountDist;

    std::vector<bool> zombiesToSpawn;

public:
    std::vector<Zombie> zombies;

    size_t level;
    size_t stage;

    ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage, std::mt19937 &rngReference);

    void init();

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);

    int checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);

    void draw(sf::RenderWindow &window, bool debugDraw = false);
};