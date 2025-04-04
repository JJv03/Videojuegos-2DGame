#include "enemyManager.h"
#include <iostream>

EnemyManager::EnemyManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
}

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
        if (leopard.level == currentLevel && leopard.stage == currentStage)
        {
            leopard.update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone, playerPtr->sprite->getPosition());
        }
    }
    for (auto &bat : bat)
    {
        if (bat.level == currentLevel && bat.stage == currentStage)
        {
            bat.update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone);
        }
    }
}

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
        if (leopard.level == currentLevel && leopard.stage == currentStage)
        {
            leopard.checkCollisions(playerBounds, whipBounds, tilemaps[currentStage], playerPtr->isAttacking, playerPtr->damage);
        }
    }
    for (auto &bat : bat)
    {
        if (bat.level == currentLevel && bat.stage == currentStage)
        {
            bat.checkCollisions(playerBounds, whipBounds, playerPtr->isAttacking, playerPtr->damage);
        }
    }
}

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
        if (leopard.level == currentLevel && leopard.stage == currentStage)
        {
            leopard.draw(window, true);
        }
    }
    for (auto &bat : bat)
    {
        if (bat.level == currentLevel && bat.stage == currentStage)
        {
            bat.draw(window, true);
        }
    }
}

void EnemyManager::loadEnemiesFromLevel(int level, const TilemapManager &tilemaps)
{

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