#include "ZombieSpawner.h"
#include <iostream>
#include <ctime>

// Initialize spawner with position, zone, and RNG
ZombieSpawner::ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const int &level, const int &stage, std::mt19937 &rngReference)
    : spawnPosition(position), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize),
      rng(rngReference), zombieCountDist(1, 3), level(level), stage(stage)
{
    zombiesToSpawn.resize(3, false);

    init();
}

// Create initial zombie pool
void ZombieSpawner::init()
{
    try
    {
        zombies.clear();

        for (int i = 0; i < 3; i++)
        {
            zombies.push_back(createZombie(spawnPosition));
            zombies.back()->isActive = false; // Start inactive
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing zombies in spawner: " << e.what() << std::endl;
    }
}

// Main update - handles spawning and zombie management
void ZombieSpawner::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone)
{
    // Check if player is in spawn zone
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    // Check if all zombies are inactive
    allZombiesInactive = true;

    for (const auto &zombie : zombies)
    {
        if (zombie->isActive)
        {
            allZombiesInactive = false;
            break;
        }
    }

    // Check if spawner is currently active
    spawnerActive = false;

    for (size_t i = 0; i < zombies.size(); i++)
    {
        if (zombiesToSpawn[i])
        {
            spawnerActive = true;
            break;
        }
    }

    // Activate spawn process when conditions met
    if (playerInZone && allZombiesInactive && !spawnerActive)
    {
        int zombiesToSpawnCount = zombieCountDist(rng); // Random count (1-3)

        std::fill(zombiesToSpawn.begin(), zombiesToSpawn.end(), false);

        zombieSpawnTimers = 1.5f; // Spawn delay
        dist = 0.0;               // Reset position offset

        // Queue zombies for spawning
        for (int i = 0; i < zombiesToSpawnCount; i++)
        {
            zombiesToSpawn[i] = true;
        }
    }

    // Process spawn queue
    for (size_t i = 0; i < zombies.size(); i++)
    {
        if (zombiesToSpawn[i])
        {
            zombieSpawnTimers -= deltaTime;

            if (zombieSpawnTimers <= 0.0f)
            {
                // Position and activate zombie
                zombies[i]->movePositionToBorder(playerActivationZone, dist);
                zombies[i]->isActive = true;
                zombiesToSpawn[i] = false;
                dist += 20; // Offset next zombie
            }
        }
    }

    // Update active zombies
    for (auto &zombie : zombies)
    {
        if (zombie->isActive)
        {
            bool zombieInsideDeactivationZone = false;

            for (const auto &hitbox : zombie->hitboxes)
            {
                if (playerDeactivationZone.findIntersection(hitbox).has_value())
                {
                    zombieInsideDeactivationZone = true;
                    break;
                }
            }

            if (!zombieInsideDeactivationZone)
            {
                zombie->isActive = false;
                zombie->resetPosition();
            }
            else
            {
                zombie->update(deltaTime);
            }
        }
    }
}

// Handle collisions for all active zombies
void ZombieSpawner::checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage)
{
    for (auto &zombie : zombies)
    {
        if (zombie->isActive)
        {
            zombie->checkCollisions(weaponBounds, tileMap, playerIsAtacking, playerDamage);
        }
    }
}

// Render active zombies and debug visuals
void ZombieSpawner::draw(sf::RenderWindow &window, bool debugDraw)
{
    for (auto &zombie : zombies)
    {
        if (zombie->sprite && zombie->isActive)
        {
            zombie->draw(window, debugDraw);
        }
    }

    if (debugDraw)
    {
        sf::RectangleShape zoneShape({spawnZone.size.x, spawnZone.size.y});
        zoneShape.setPosition({spawnZone.position.x, spawnZone.position.y});
        zoneShape.setFillColor(sf::Color(255, 255, 0, 50));
        zoneShape.setOutlineColor(sf::Color::Yellow);
        zoneShape.setOutlineThickness(1.0f);
        window.draw(zoneShape);
    }
}