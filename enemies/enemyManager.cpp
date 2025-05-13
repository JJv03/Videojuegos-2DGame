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

    if (playerPtr->isStopWatchActive || playerPtr->upgradeWhip || playerPtr->activateRosario)
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
    for (auto &redSkeleton : redSkeleton)
    {
        if (redSkeleton->level == currentLevel && redSkeleton->stage == currentStage)
        {
            redSkeleton->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), mapBounds);
        }
    }
    for (auto &crow : crow)
    {
        if (crow->level == currentLevel && crow->stage == currentStage)
        {
            crow->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getGlobalBounds().position, playerPtr->getBounds(), mapBounds);
        }
    }
    for (auto &whiteSkeleton : whiteSkeleton)
    {
        if (whiteSkeleton->level == currentLevel && whiteSkeleton->stage == currentStage)
        {
            whiteSkeleton->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), mapBounds);
        }
    }
    for (auto &hunchback : hunchback)
    {
        if (hunchback->level == currentLevel && hunchback->stage == currentStage)
        {
            hunchback->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                            playerPtr->sprite->getPosition(), mapBounds, playerPtr);
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
    for (auto &redSkeleton : redSkeleton)
    {
        if (redSkeleton->level == currentLevel && redSkeleton->stage == currentStage)
        {
            redSkeleton->draw(window);
        }
    }
    for (auto &crow : crow)
    {
        if (crow->level == currentLevel && crow->stage == currentStage)
        {
            crow->draw(window);
        }
    }
    for (auto &whiteSkeleton : whiteSkeleton)
    {
        if (whiteSkeleton->level == currentLevel && whiteSkeleton->stage == currentStage)
        {
            whiteSkeleton->draw(window);
        }
    }
    for (auto &hunchback : hunchback)
    {
        if (hunchback->level == currentLevel && hunchback->stage == currentStage)
        {
            hunchback->draw(window);
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
    cannon.clear();
    axeman.clear();
    redSkeleton.clear();
    crow.clear();
    whiteSkeleton.clear();
    hunchback.clear();


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

                case 9: // Crow
                    crow.push_back(createCrow(
                        enemyData.position, level, currentStage));
                    break;
                
                case 10: // WhiteSkeleton
                    whiteSkeleton.push_back(createWhiteSkeleton(
                        enemyData.position, level, currentStage));
                    break;
                
                case 11: // Hunchback
                    hunchback.push_back(createHunchback(
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
                
                case 6: // Cannon
                    cannon.push_back(createCannon(
                        enemyData.position, level, currentStage));
                    break;

                case 7: // AxeMan
                    axeman.push_back(createAxeMan(
                        enemyData.position, level, currentStage));
                    break;

                case 8: // RedSkeleton
                    redSkeleton.push_back(createRedSkeleton(
                        enemyData.position, level, currentStage));
                    break;
                
                case 10: // WhiteSkeleton
                    whiteSkeleton.push_back(createWhiteSkeleton(
                        enemyData.position, level, currentStage));
                    break;
                
                case 11: // Hunchback
                    hunchback.push_back(createHunchback(
                        enemyData.position, level, currentStage, false));
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
        if (f->level == currentLevel && f->stage == currentStage)
        {
            if (f->isActive) allEnemies.push_back(f);
            if (f->getProjectile() && f->getProjectile()->getActive()) allEnemies.push_back(f->getProjectile().get());
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
        if (c->level == currentLevel && c->stage == currentStage)
        {
            if(c->isActive) allEnemies.push_back(c);

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
        if (ax->level == currentLevel && ax->stage == currentStage)
        {
            if(ax->isActive) allEnemies.push_back(ax);

            for(auto& axe : ax->axes){
                if (axe)
                {
                    allEnemies.push_back(axe.get());
                }
            }
        }
    }
    for (auto &rs : redSkeleton)
    {
        if (rs->level == currentLevel && rs->stage == currentStage && rs->isActive)
        {
            allEnemies.push_back(rs);
        }
    }
    for (auto &c : crow)
    {
        if (c->level == currentLevel && c->stage == currentStage && c->isActive)
        {
            allEnemies.push_back(c);
        }
    }
    for (auto &ws : whiteSkeleton)
    {
        if (ws->level == currentLevel && ws->stage == currentStage)
        {
            if(ws->isActive) allEnemies.push_back(ws);

            for(auto &b : ws->bones)
            {
                if (b && b->getActive())
                {
                    allEnemies.push_back(b.get());
                }
            }
        }
    }
    for (auto &h : hunchback)
    {
        if (h->level == currentLevel && h->stage == currentStage && h->isActive)
        {
            allEnemies.push_back(h);
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
    for (auto &redSkeleton : redSkeleton)
    {
        if (redSkeleton->level == currentLevel && redSkeleton->stage == currentStage)
        {
            redSkeleton->isActive = false;
            redSkeleton->resetPosition();
        }
    }
    for (auto &crow : crow)
    {
        if (crow->level == currentLevel && crow->stage == currentStage)
        {
            crow->isActive = false;
            crow->resetPosition();
        }
    }
    for (auto &whiteSkeleton : whiteSkeleton)
    {
        if (whiteSkeleton->level == currentLevel && whiteSkeleton->stage == currentStage)
        {
            whiteSkeleton->isActive = false;
            whiteSkeleton->resetPosition();
        }
    }
    for (auto &hunchback : hunchback)
    {
        if (hunchback->level == currentLevel && hunchback->stage == currentStage)
        {
            hunchback->isActive = false;
            hunchback->resetPosition();
        }
    }
}


void EnemyManager::turnOffAutoSpawn(int currentLevel, int currentStage)
{
    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            for(auto& zombie : zombieSpawner.zombies)
            {
                zombie->needsPlayerToLeaveZone = true;
            }
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard->level == currentLevel && leopard->stage == currentStage)
        {
            leopard->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &bat : bat)
    {
        if (bat->level == currentLevel && bat->stage == currentStage)
        {
            bat->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &fishman : fishman)
    {
        if (fishman->level == currentLevel && fishman->stage == currentStage)
        {
            fishman->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &ghost : ghost)
    {
        if (ghost->level == currentLevel && ghost->stage == currentStage)
        {
            ghost->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &medusa : medusa)
    {
        if (medusa->level == currentLevel && medusa->stage == currentStage)
        {
            medusa->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &cannon : cannon)
    {
        if (cannon->level == currentLevel && cannon->stage == currentStage)
        {
            cannon->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &axeman : axeman)
    {
        if (axeman->level == currentLevel && axeman->stage == currentStage)
        {
            axeman->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &redSkeleton : redSkeleton)
    {
        if (redSkeleton->level == currentLevel && redSkeleton->stage == currentStage)
        {
            redSkeleton->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &crow : crow)
    {
        if (crow->level == currentLevel && crow->stage == currentStage)
        {
            crow->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &whiteSkeleton : whiteSkeleton)
    {
        if (whiteSkeleton->level == currentLevel && whiteSkeleton->stage == currentStage)
        {
            whiteSkeleton->needsPlayerToLeaveZone = true;
        }
    }
    for (auto &hunchback : hunchback)
    {
        if (hunchback->level == currentLevel && hunchback->stage == currentStage)
        {
            hunchback->needsPlayerToLeaveZone = true;
        }
    }
}
