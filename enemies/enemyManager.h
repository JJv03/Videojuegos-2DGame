#pragma once

#include "createEnemies.h"
#include "../player.h"
#include "../tilemapManager.h"
#include "projectile.h"
#include <random>
#include <vector>

class EnemyManager
{
private:
    // Enemies vector
    std::vector<ZombieSpawner> zombiesSpawner;  // 0
    std::vector<Leopard *> leopard;             // 1
    std::vector<Bat *> bat;                     // 2
    std::vector<FishMan *> fishman;             // 3
    std::vector<Ghost *> ghost;                 // 4
    std::vector<Medusa *> medusa;               // 5
    std::vector<Cannon *> cannon;               // 6
    std::vector<AxeMan *> axeman;               // 7
    std::vector<RedSkeleton *> redSkeleton;     // 8
    std::vector<Crow *> crow;                   // 9
    std::vector<WhiteSkeleton *> whiteSkeleton; // 10
    std::vector<Hunchback *> hunchback;     // 11

    // Reference to player
    Player *playerPtr;

    // Random number generator
    std::mt19937 globalRng;

public:
    EnemyManager(Player *player); // Initialize with player reference

    // Core game loop functions
    void update(float deltaTime, const int currentLevel, const int currentStage, const sf::FloatRect &mapBounds);
    void draw(sf::RenderWindow &window, const int currentLevel, const int currentStage);

    std::vector<Entity *> getEnemies(int currentLevel, int currentStage) const; // Get all enemies

    // Level loading system
    void loadEnemiesFromLevel(int level, const TilemapManager &tilemaps);

    void restartEnemies(int currentLevel, int currentStage);

    void turnOffAutoSpawn(int currentLevel, int currentStage);
};