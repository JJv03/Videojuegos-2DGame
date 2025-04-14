#pragma once

#include "createEnemies.h"
#include "../player.h"
#include "../tilemapManager.h"
#include <random>
#include <vector>

class EnemyManager
{
private:
    // Enemies vector
    std::vector<ZombieSpawner> zombiesSpawner;
    std::vector<Leopard *> leopard;
    std::vector<Bat *> bat;
    std::vector<FishMan *> fishman;

    // Reference to player
    Player *playerPtr;

    // Random number generator
    std::mt19937 globalRng;

public:
    EnemyManager(Player *player); // Initialize with player reference

    // Core game loop functions
    void update(float deltaTime, const int currentLevel, const int currentStage);
    void checkCollisions(const int currentLevel, const int currentStage, const TilemapManager tilemaps);
    void draw(sf::RenderWindow &window, const int currentLevel, const int currentStage);

    std::vector<Enemy *> getEnemies(int currentLevel, int currentStage) const; // Get all enemies

    // Level loading system
    void loadEnemiesFromLevel(int level, const TilemapManager &tilemaps);

    void restartEnemies(int currentLevel, int currentStage);
};