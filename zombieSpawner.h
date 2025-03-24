#pragma once

#include "zombie.h"
#include <vector>
#include <random>
#include <memory>

class ZombieSpawner
{
private:
    sf::Vector2f spawnPosition;
    sf::FloatRect spawnZone;
    std::vector<Zombie> zombies;

    bool allZombiesInactive{true};
    bool spawnerActive{false};

    float dist = 0.0f;
    float zombieSpawnTimers = 0.0f;

    std::mt19937 rng;
    std::uniform_int_distribution<int> zombieCountDist;

    std::vector<bool> zombiesToSpawn;

public:
    ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize);

    void init();

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);

    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);

    void draw(sf::RenderWindow &window, bool debugDraw = false);
};