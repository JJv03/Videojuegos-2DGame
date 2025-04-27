#include "bossManager.h"
#include <iostream>

// Initialize with player reference and RNG
BossManager::BossManager(Player *player) : playerPtr(player), globalRng(std::random_device{}())
{
    // Load and process boss texture
    sf::Image bossImage;
    if (!bossImage.loadFromFile("./assets/sprites/enemies/bosses.png"))
    {
        std::cerr << "Error cargando la imagen de los bosses" << std::endl;
        throw std::runtime_error("Error cargando la imagen de boss");
    }
    bossImage.createMaskFromColor(gColorKeyGrey);
    bossImage.createMaskFromColor(gColorKeyGreen);

    sf::Texture *bossTexture = new sf::Texture();
    if (!bossTexture->loadFromImage(bossImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete bossTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    gTextures["boss"] = *bossTexture;
}

// Update all active bosses in current level/stage
void BossManager::update(float deltaTime, const int currentLevel, const int currentStage, const int phase, const sf::FloatRect &mapBounds)
{
    if (playerPtr->upgradeWhip)
    {
        return;
    }
    if (phantomBat)
    {
        if (phantomBat->level == currentLevel && phantomBat->stage == currentStage)
        {
            phantomBat->update(deltaTime, playerPtr->gPlayerActivationZone, playerPtr->gPlayerDeactivationZone,
                               playerPtr->sprite->getScale().x, playerPtr->sprite->getGlobalBounds(), mapBounds);
        }
    }
    if (dracula)
    {
        if (dracula->level == currentLevel && dracula->stage == currentStage)
        {
            dracula->update(deltaTime, phase, *playerPtr, mapBounds);
        }
    }
    if (draculaSpirit)
    {
        if (draculaSpirit->level == currentLevel && draculaSpirit->stage == currentStage)
        {
            draculaSpirit->update(deltaTime, phase, *playerPtr, mapBounds);
        }
    }
}

// Render all bosses in current level/stage with debug visuals
void BossManager::draw(sf::RenderWindow &window, const int currentLevel, const int currentStage)
{
    if (phantomBat)
    {
        if (phantomBat->level == currentLevel && phantomBat->stage == currentStage)
        {
            phantomBat->draw(window);
        }
    }
    if (dracula)
    {
        if (dracula->level == currentLevel && dracula->stage == currentStage)
        {
            dracula->draw(window);
        }
    }
    if (draculaSpirit)
    {
        if (draculaSpirit->level == currentLevel && draculaSpirit->stage == currentStage)
        {
            draculaSpirit->draw(window);
        }
    }
}

// Load boss layout for specified level from tilemap data
void BossManager::loadBossesFromLevel(int level, const TilemapManager &tilemaps)
{
    // Clear existing bosses
    if (phantomBat != nullptr)
    {
        delete phantomBat;
        phantomBat = nullptr;
    }
    if (dracula != nullptr)
    {
        delete dracula;
        dracula = nullptr;
    }
    if (draculaSpirit != nullptr)
    {
        delete draculaSpirit;
        draculaSpirit = nullptr;
    }

    switch (level)
    {
    case 1:

        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create bosses from tilemap data
            for (const auto &bossData : tilemap.m_enemyData)
            {
                if (bossData.type < 100)
                    continue; // Ignore enemies

                switch (bossData.type)
                {
                case 100: // PhantomBat
                    phantomBat = createPhantomBat(bossData.position, level, currentStage, tilemap.getMapBoundsBossFight());
                    break;
                default:
                    std::cerr << "Unknown boss type: " << bossData.type << std::endl;
                    break;
                }
            }
        }
        break;

    case 7:

        for (size_t stageIndex = 0; stageIndex < tilemaps.tilemaps.size(); ++stageIndex)
        {
            const TileMap &tilemap = tilemaps.tilemaps[stageIndex];
            int currentStage = static_cast<int>(stageIndex) + 1;

            // Create bosses from tilemap data
            for (const auto &bossData : tilemap.m_enemyData)
            {
                if (bossData.type < 100)
                    continue; // Ignore enemies

                switch (bossData.type)
                {
                case 101: // Dracula
                    dracula = createDracula(bossData.position, level, currentStage);
                    break;
                case 102: // Dracula Spirit
                    draculaSpirit = createDraculaSpirit(bossData.position, level, currentStage, tilemap.getMapBoundsBossFight());
                    break;
                default:
                    std::cerr << "Unknown boss type: " << bossData.type << std::endl;
                    break;
                }
            }
        }
        break;

    default:
        std::cerr << "Unknown level for boss loading: " << level << std::endl;
        break;
    }
}

std::vector<Entity *> BossManager::getBosses(int currentLevel, int currentStage) const
{
    std::vector<Entity *> allBosses;

    if (phantomBat)
    {
        if (phantomBat->level == currentLevel && phantomBat->stage == currentStage && phantomBat->isActive)
        {
            allBosses.push_back(phantomBat);
        }
    }
    if (dracula)
    {
        if (dracula->level == currentLevel && dracula->stage == currentStage && dracula->isActive)
        {
            allBosses.push_back(dracula);
            allBosses.push_back(dracula->draculaBody);
            for(auto& projectile : dracula->projectiles){
                if (projectile)
                {
                    allBosses.push_back(projectile.get());
                }
            }
        }
    }
    if (draculaSpirit)
    {
        if (draculaSpirit->level == currentLevel && draculaSpirit->stage == currentStage && draculaSpirit->isActive)
        {
            if (draculaSpirit->getProjectile())
            {
                allBosses.push_back(draculaSpirit->getProjectile().get());
            }
            if (draculaSpirit->getProjectile2())
            {
                allBosses.push_back(draculaSpirit->getProjectile2().get());
            }
            if (draculaSpirit->getProjectile3())
            {
                allBosses.push_back(draculaSpirit->getProjectile3().get());
            }
            allBosses.push_back(draculaSpirit);
        }
    }

    return allBosses;
}

void BossManager::killBoss(bossID boss){
    if (phantomBat != nullptr && boss == phantomBatID)
    {
        //delete phantomBat;
        //phantomBat = nullptr;
        phantomBat->isActive = false;
    }
    if (dracula != nullptr && boss == draculaID)
    {
        //delete dracula;
        //dracula = nullptr;
        float deadPosition = dracula->draculaBody->sprite->getGlobalBounds().position.x;
        dracula->isActive = false;
        for(auto& p : dracula->projectiles){
            if(p) p->setActive(false);
        }
        if(draculaSpirit) draculaSpirit->sprite->move({dracula->draculaBody->sprite->getGlobalBounds().position.x - draculaSpirit->sprite->getPosition().x, 0.f});
    }
    if (draculaSpirit != nullptr && boss == draculaSpiritID)
    {
        //delete draculaSpirit;
        //draculaSpirit = nullptr;
        draculaSpirit->isActive = false;
        if(draculaSpirit->projectile) draculaSpirit->projectile->setActive(false);
        if(draculaSpirit->projectile2) draculaSpirit->projectile2->setActive(false);
        if(draculaSpirit->projectile3) draculaSpirit->projectile3->setActive(false);
    }
}

void BossManager::restartBosses(int currentLevel, int currentStage)
{
    if(phantomBat)
    {
        if (phantomBat->level == currentLevel && phantomBat->stage == currentStage)
        {
            phantomBat->isActive = false;
            phantomBat->resetPosition();
        }
    }
    if(dracula)
    {
        if (dracula->level == currentLevel && dracula->stage == currentStage)
        {
            dracula->isActive = false;
            dracula->resetPosition();
        }
    }
    if(draculaSpirit)
    {
        if (draculaSpirit->level == currentLevel && draculaSpirit->stage == currentStage)
        {
            draculaSpirit->isActive = false;
            draculaSpirit->resetPosition();
        }
    }
}