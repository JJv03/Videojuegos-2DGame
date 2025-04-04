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
    std::vector<Leopard> leopard;
    std::vector<Bat> bat;

    // Player pointer
    Player *playerPtr;

    // Random number generator
    std::mt19937 globalRng;

    // Stores in <n-1> the tilemaps of the stages n in the current level
    std::vector<TileMap> tilemaps;

public:
    EnemyManager(Player *player);

    void update(float deltaTime, const size_t currentLevel, const size_t currentStage);
    void checkCollisions(const size_t currentLevel, const size_t currentStage, const TilemapManager tilemaps);
    void draw(sf::RenderWindow &window, const size_t currentLevel, const size_t currentStage);

    void loadEnemiesFromLevel(int level, const TilemapManager &tilemaps);
};