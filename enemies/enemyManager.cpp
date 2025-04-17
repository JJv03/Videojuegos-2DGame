#include "enemyManager.h"
#include <iostream>

// Initialize with player reference and RNG
EnemyManager::EnemyManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
}

// Update all active enemies in current level/stage
void EnemyManager::update(float deltaTime, const int currentLevel, const int currentStage)
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
            leopard->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getPosition(), playerPtr->getBounds());
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getGlobalBounds());
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getGlobalBounds());
        }
    }
}

// Render all enemies in current level/stage with debug visuals
void EnemyManager::draw(sf::RenderWindow &window, const int currentLevel, const int currentStage)
{
    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.draw(window);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->draw(window);
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->draw(window);
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->draw(window);
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
    fishman.clear();

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

                case 3: // Fishman
                    fishman.push_back(createFishManSpawner(
                        enemyData.position,
                        sf::Vector2f(enemyData.width > 0 ? enemyData.width : 50.f,
                                     enemyData.height > 0 ? enemyData.height : 50.f),
                        level, currentStage, globalRng));
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

std::vector<Enemy *> EnemyManager::getEnemies(int currentLevel, int currentStage) const
{
    std::vector<Enemy *> allEnemies;

    for (auto &spawner : zombiesSpawner)
    {
        if (spawner.level == currentLevel && spawner.stage == currentStage)
        {
            for (auto &z : spawner.zombies)
            {
                if (z->isActive)
                {
                    allEnemies.push_back(z);
                }
            }
        }
    }
    for (auto &l : leopard)
    {
        if (l->level == currentLevel && l->stage == currentStage && l->isActive)
        {
            allEnemies.push_back(l);
        }
    }
    for (auto &b : bat)
    {
        if (b->level == currentLevel && b->stage == currentStage && b->isActive)
        {
            allEnemies.push_back(b);
        }
    }
    for (auto &f : fishman)
    {
        if (f->level == currentLevel && f->stage == currentStage && f->isActive)
        {
            allEnemies.push_back(f);
        }
    }

    return allEnemies;
}

void EnemyManager::restartEnemies(int currentLevel, int currentStage)
{
    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            for (auto &zombie : zombieSpawner.zombies)
            {
                zombie->isActive = false;
                zombie->resetPosition();
            }
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->isActive = false;
            leopard->resetPosition();
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->isActive = false;
            bat->resetPosition();
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->isActive = false;
            fishman->resetPosition();
        }
    }
}
