#include "enemyManager.h"
#include <iostream>

// Initialize with player reference and RNG
EnemyManager::EnemyManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
    // Load and process enemy texture
    sf::Image enemyImage;
    if (!enemyImage.loadFromFile("./assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen de enemigos" << std::endl;
        throw std::runtime_error("Error cargando la imagen de enemy");
    }
    enemyImage.createMaskFromColor(gColorKeyGrey);
    enemyImage.createMaskFromColor(gColorKeyGreen);

    sf::Texture *enemyTexture = new sf::Texture();
    if (!enemyTexture->loadFromImage(enemyImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete enemyTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    gTextures["enemy"] = *enemyTexture;
}

// Update all active enemies in current level/stage
void EnemyManager::update(float deltaTime, const int currentLevel, const int currentStage, const sf::FloatRect &mapBounds)
{
    PlayerPosition playerPos = playerPtr->getPlayerOffsetPosition();

    if (playerPtr->isStopWatchActive || playerPtr->upgradeWhip)
    {
        return;
    }

    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.update(deltaTime, playerPtr->gPlayerActivationZone,
                                 playerPtr->gPlayerDeactivationZone, playerPos, mapBounds);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), playerPtr->getBounds(), mapBounds);
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                        playerPtr->sprite->getScale().x, playerPtr->sprite->getGlobalBounds(), playerPos, mapBounds);
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getGlobalBounds(), mapBounds);
        }
    }
    for (auto &ghost : ghost)
    {
        if (ghost->level == currentLevel && ghost->stage == currentStage)
        {
            ghost->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                          playerPtr->sprite->getPosition(), mapBounds);
        }
    }
    for (auto &medusa : medusa)
    {
        if (medusa->level == currentLevel && medusa->stage == currentStage)
        {
            medusa->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                           playerPtr->sprite->getScale().x, playerPtr->sprite->getGlobalBounds(), playerPos, mapBounds);
        }
    }
    for (auto &cannon : cannon)
    {
        if (cannon->level == currentLevel && cannon->stage == currentStage)
        {
            cannon->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), mapBounds);
        }
    }
    for (auto &axeman : axeman)
    {
        if (axeman->level == currentLevel && axeman->stage == currentStage)
        {
            axeman->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), mapBounds);
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
    for (auto &ghost : ghost)
    {
        if (ghost->level == currentLevel && ghost->stage == currentStage)
        {
            ghost->draw(window);
        }
    }
    for (auto &medusa : medusa)
    {
        if (medusa->level == currentLevel && medusa->stage == currentStage)
        {
            medusa->draw(window);
        }
    }
    for (auto &cannon : cannon)
    {
        if (cannon->level == currentLevel && cannon->stage == currentStage)
        {
            cannon->draw(window);
        }
    }
    for (auto &axeman : axeman)
    {
        if (axeman->level == currentLevel && axeman->stage == currentStage)
        {
            axeman->draw(window);
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
    ghost.clear();
    medusa.clear();

    switch (level)
    {
    case 1: // LEVEL 1

        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create enemies from tilemap data
            for (const auto &enemyData : tilemap.m_enemyData)
            {
                if (enemyData.type >= 100)
                    continue; // Ignore bosses

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

    case 3: // LEVEL 3

        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create enemies from tilemap data
            for (const auto &enemyData : tilemap.m_enemyData)
            {
                if (enemyData.type >= 100)
                    continue; // Ignore bosses

                switch (enemyData.type)
                {

                case 4: // Ghost
                    ghost.push_back(createGhost(enemyData.position, level, currentStage));
                    break;

                case 5: // Medusa
                    medusa.push_back(createMedusaSpawner(
                        enemyData.position,
                        sf::Vector2f(enemyData.width > 0 ? enemyData.width : 50.f,
                                     enemyData.height > 0 ? enemyData.height : 50.f),
                        level, currentStage));
                    break;

                case 6: // Cannon
                    cannon.push_back(createCannon(
                        enemyData.position, level, currentStage));
                    break;


                default:
                    std::cerr << "Unknown enemy type: " << enemyData.type << std::endl;
                    break;
                }
            }
        }
        break;

    case 5: // LEVEL 5
        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create enemies from tilemap data
            for (const auto &enemyData : tilemap.m_enemyData)
            {
                if (enemyData.type >= 100)
                    continue; // Ignore bosses

                switch (enemyData.type)
                {

                case 5: // Medusa
                    medusa.push_back(createMedusaSpawner(
                        enemyData.position,
                        sf::Vector2f(enemyData.width > 0 ? enemyData.width : 50.f,
                                     enemyData.height > 0 ? enemyData.height : 50.f),
                        level, currentStage));
                    break;

                case 7: // AxeMan
                    axeman.push_back(createAxeMan(
                        enemyData.position, level, currentStage));
                    break;

                default:
                    std::cerr << "Unknown enemy type: " << enemyData.type << std::endl;
                    break;
                }
            }
        }
        break;

    case 7: // LEVEL 7
        break;

    default:
        std::cerr << "Unknown level for enemy loading: " << level << std::endl;
        break;
    }
}

std::vector<Entity *> EnemyManager::getEnemies(int currentLevel, int currentStage) const
{
    std::vector<Entity *> allEnemies;

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
            if (f->getProjectile())
            {
                allEnemies.push_back(f->getProjectile().get());
            }
            allEnemies.push_back(f);
        }
    }
    for (auto &g : ghost)
    {
        if (g->level == currentLevel && g->stage == currentStage && g->isActive)
        {
            allEnemies.push_back(g);
        }
    }
    for (auto &m : medusa)
    {
        if (m->level == currentLevel && m->stage == currentStage && m->isActive)
        {
            allEnemies.push_back(m);
        }
    }
    for (auto &c : cannon)
    {
        if (c->level == currentLevel && c->stage == currentStage && c->isActive)
        {
            allEnemies.push_back(c);

            for(auto &p : c->projectiles)
            {
                if (p && p->getActive())
                {
                    allEnemies.push_back(p.get());
                }
            }
        }
    }
    for (auto &ax : axeman)
    {
        if (ax->level == currentLevel && ax->stage == currentStage && ax->isActive)
        {
            allEnemies.push_back(ax);
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
            zombieSpawner.resetPosition();
            zombieSpawner.resetSpawnState();
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
            bat->resetSpawnState();
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->isActive = false;
            fishman->resetPosition();
            if (fishman->getProjectile().get())
            {
                fishman->getProjectile().get()->reset();
            }
            fishman->resetSpawnState();
        }
    }
    for (auto &ghost : ghost)
    {
        if (ghost->level == currentLevel && ghost->stage == currentStage)
        {
            ghost->isActive = false;
            ghost->resetPosition();
        }
    }
    for (auto &medusa : medusa)
    {
        if (medusa->level == currentLevel && medusa->stage == currentStage)
        {
            medusa->isActive = false;
            medusa->resetPosition();
            medusa->resetSpawnState();
        }
    }
    for (auto &cannon : cannon)
    {
        if (cannon->level == currentLevel && cannon->stage == currentStage)
        {
            cannon->isActive = false;
            cannon->resetPosition();
        }
    }
    for (auto &axeman : axeman)
    {
        if (axeman->level == currentLevel && axeman->stage == currentStage)
        {
            axeman->isActive = false;
            axeman->resetPosition();
        }
    }
}
