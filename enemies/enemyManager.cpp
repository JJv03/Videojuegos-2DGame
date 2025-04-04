#include "enemyManager.h"
#include <iostream>

EnemyManager::EnemyManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
}

void EnemyManager::init()
{
    // LEVEL 1 - STAGE 2
    zombiesSpawner.push_back(ZombieSpawner({200.f, 176.f}, {50.f, 50.f}, 1, 2, globalRng));
    leopard.push_back(createLeopard({696.f, 112.f}, 1, 2));
    leopard.push_back(createLeopard({888.f, 80.f}, 1, 2));
    leopard.push_back(createLeopard({950.f, 112.f}, 1, 2));
    zombiesSpawner.push_back(ZombieSpawner({1300.f, 176.f}, {50.f, 50.f}, 1, 2, globalRng));

    // LEVEL 1 - STAGE 3
    bat.push_back(createBatSpawner({136.f, 112.f}, {50.f, 50.f}, 1, 3));
    bat.push_back(createBatSpawner({395.f, 112.f}, {50.f, 50.f}, 1, 3));

    // LEVEL 1 - STAGE 5
    zombiesSpawner.push_back(ZombieSpawner({203.f, 176.f}, {50.f, 50.f}, 1, 5, globalRng));
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