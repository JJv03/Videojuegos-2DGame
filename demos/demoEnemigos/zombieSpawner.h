#pragma once

#include "zombie.h"
#include <vector>
#include <random>
#include <memory>

class ZombieSpawner
{
private:
    std::vector<Zombie> zombies;
    sf::FloatRect spawnZone;
    sf::Vector2f spawnPosition;

    bool allZombiesInactive{true};
    bool playerWasInZone{false};

    std::mt19937 rng;
    std::uniform_int_distribution<int> zombieCountDist;

    std::vector<bool> zombiesToSpawn;
    std::vector<float> zombieSpawnTimers;

public:
    ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize);

    void init();

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);

    void checkCollisions(const std::vector<sf::FloatRect> &boundsList);

    void checkVampireKillerCollision(const sf::FloatRect &weaponBounds);

    void draw(sf::RenderWindow &window, bool debugDraw = false);
};