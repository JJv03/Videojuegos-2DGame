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

    std::mt19937 rng;
    std::uniform_int_distribution<int> zombieCountDist;

    std::vector<bool> zombiesToSpawn;
    std::vector<float> zombieSpawnTimers;

public:
    ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize);

    void init();

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);

    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking);

    void draw(sf::RenderWindow &window, bool debugDraw = false);
};