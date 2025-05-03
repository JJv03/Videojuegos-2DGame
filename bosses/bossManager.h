#pragma once

#include "createBosses.h"
#include "../player.h"
#include "../tilemapManager.h"
#include <random>
#include <vector>

enum bossID{
    phantomBatID,
    mummyMan1ID,
    mummyMan2ID,
    deathID,
    draculaID,
    draculaSpiritID,
};

class BossManager
{
private:
    // Bosses
    PhantomBat *phantomBat = nullptr;
    Dracula *dracula = nullptr;
    DraculaSpirit *draculaSpirit = nullptr;
    Death *death = nullptr;
    MummyMan *mummyMan1 = nullptr;
    MummyMan *mummyMan2 = nullptr;
    // Reference to player
    Player *playerPtr;

    // Random number generator
    std::mt19937 globalRng;

public:
    BossManager(Player *player); // Initialize with player reference

    // Core game loop functions
    void update(float deltaTime, const int currentLevel, const int currentStage, const int phase, const sf::FloatRect &mapBounds);
    void draw(sf::RenderWindow &window, const int currentLevel, const int currentStage);

    std::vector<Entity *> getBosses(int currentLevel, int currentStage) const; // Get all bosses

    // Level loading system
    void loadBossesFromLevel(int level, const TilemapManager &tilemaps);

    void killBoss(bossID boss);

    void restartBosses(int currentLevel, int currentStage);
};
