#include "enemyManager.h"
#include <iostream>

// Initialize with player reference and RNG
EnemyManager::EnemyManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
}

// Update all active enemies in current level/stage
void EnemyManager::update(float deltaTime, const size_t currentLevel, const size_t currentStage)
{

    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getPosition());
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getGlobalBounds());
        }
    }
}

// Handle collisions for all enemies in current level/stage
void EnemyManager::checkCollisions(const size_t currentLevel, const size_t currentStage, const TilemapManager tilemaps)
{
    sf::FloatRect playerBounds = playerPtr->sprite->getGlobalBounds();
    sf::FloatRect whipBounds = playerPtr->whip.sprite->getGlobalBounds();

    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.checkCollisions(whipBounds, tilemaps[currentStage], playerPtr->isAttacking, playerPtr->damage);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->checkCollisions(playerBounds, whipBounds, tilemaps[currentStage], playerPtr->isAttacking, playerPtr->damage);
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->checkCollisions(playerBounds, whipBounds, playerPtr->isAttacking, playerPtr->damage);
        }
    }
}

// Render all enemies in current level/stage with debug visuals
void EnemyManager::draw(sf::RenderWindow &window, const size_t currentLevel, const size_t currentStage)
{
    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.draw(window, true);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->draw(window, true);
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->draw(window, true);
        }
    }
}

// Load enemy layout for specified level from tilemap data
void EnemyManager::loadEnemiesFromLevel(int level, const TilemapManager &tilemaps)
{
    // Clear existing enemies
    zombiesSpawner.clear();
    leopard.clear();
    bat.clear();

    switch (level)
    {
    case 1:

        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create enemies from tilemap data
            for (const auto &enemyData : tilemap.m_enemyData)
            {
                switch (enemyData.type)
                {
                case 0: // Zombie
                    zombiesSpawner.push_back(ZombieSpawner(
                        enemyData.position,
                        sf::Vector2f(enemyData.width > 0 ? enemyData.width : 50.f,
                                     enemyData.height > 0 ? enemyData.height : 50.f),
                        level, currentStage, globalRng));
                    break;

                case 1: // Leopard
                    leopard.push_back(createLeopard(enemyData.position, level, currentStage));
                    break;

                case 2: // Bat
                    bat.push_back(createBatSpawner(
                        enemyData.position,
                        sf::Vector2f(enemyData.width > 0 ? enemyData.width : 50.f,
                                     enemyData.height > 0 ? enemyData.height : 50.f),
                        level, currentStage));
                    break;

                default:
                    std::cerr << "Unknown enemy type: " << enemyData.type << std::endl;
                    break;
                }
            }
        }
        break;

    case 2:
        break;

    default:
        std::cerr << "Unknown level for enemy loading: " << level << std::endl;
        break;
    }
}

std::vector<Enemy *> EnemyManager::getEnemies() const
{
    std::vector<Enemy *> allEnemies;

    for (auto &spawner : zombiesSpawner)
    {
        for (auto &zombie : spawner.zombies)
        {
            allEnemies.push_back(zombie);
        }
    }

    for (auto &l : leopard)
    {
        allEnemies.push_back(l);
    }
    for (auto &b : bat)
    {
        allEnemies.push_back(b);
    }

    return allEnemies;
}
