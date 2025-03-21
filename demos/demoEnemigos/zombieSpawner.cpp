#include "ZombieSpawner.h"
#include <iostream>
#include <ctime>

ZombieSpawner::ZombieSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize)
    : spawnPosition(position), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize),
      rng(static_cast<unsigned int>(std::time(nullptr))), zombieCountDist(1, 3)
{
    zombiesToSpawn.resize(3, false);
    zombieSpawnTimers.resize(3, 0.0f);

    init();
}

void ZombieSpawner::init()
{
    try
    {
        zombies.clear();

        for (int i = 0; i < 3; i++)
        {
            zombies.push_back(Zombie::createZombie(spawnPosition));
            zombies.back().isActive = false;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing zombies in spawner: " << e.what() << std::endl;
    }
}

void ZombieSpawner::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone)
{
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    // Comprueba que todos los zombis esten inactivos
    allZombiesInactive = true;

    for (const auto &zombie : zombies)
    {
        if (zombie.isActive)
        {
            allZombiesInactive = false;
            break;
        }
    }

    // Comprueba que el spawner este activo
    spawnerActive = false;

    for (int i = 0; i < zombies.size(); i++)
    {
        if (zombiesToSpawn[i])
        {
            spawnerActive = true;
            break;
        }
    }

    // Activación del proceso de spawn de la horda
    if (playerInZone && allZombiesInactive && !spawnerActive)
    {
        int zombiesToSpawnCount = zombieCountDist(rng);

        std::fill(zombiesToSpawn.begin(), zombiesToSpawn.end(), false);

        float spawnTime = 1.5f;

        for (int i = 0; i < zombiesToSpawnCount; i++)
        {
            zombiesToSpawn[i] = true;
            zombieSpawnTimers[i] = spawnTime;
            spawnTime += 0.3f;
        }
    }

    // Spawn de los zombies de la orda según el timer y el tamaño de la horda
    for (size_t i = 0; i < zombies.size(); i++)
    {
        if (zombiesToSpawn[i])
        {
            zombieSpawnTimers[i] -= deltaTime;

            if (zombieSpawnTimers[i] <= 0.0f)
            {
                zombies[i].isActive = true;
                zombies[i].resetPosition();
                zombiesToSpawn[i] = false;
            }
        }
    }

    // Actualizar los zombies activos
    for (auto &zombie : zombies)
    {
        if (zombie.isActive)
        {
            bool zombieInsideDeactivationZone = false;

            for (const auto &hitbox : zombie.hitboxes)
            {
                if (playerDeactivationZone.findIntersection(hitbox).has_value())
                {
                    zombieInsideDeactivationZone = true;
                    break;
                }
            }

            if (!zombieInsideDeactivationZone)
            {
                zombie.isActive = false;
                zombie.resetPosition();
            }
            else
            {
                zombie.update(deltaTime);
            }
        }
    }
}

void ZombieSpawner::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                                    const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking)
{
    for (auto &zombie : zombies)
    {
        if (zombie.isActive)
        {
            zombie.checkCollisions(simonBounds, weaponBounds, boundsList, playerIsAtacking);
        }
    }
}

void ZombieSpawner::draw(sf::RenderWindow &window, bool debugDraw)
{
    for (auto &zombie : zombies)
    {
        if (zombie.sprite && zombie.isActive)
        {
            zombie.draw(window, debugDraw);
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