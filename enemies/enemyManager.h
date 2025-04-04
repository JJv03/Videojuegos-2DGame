#pragma once

#include "createEnemies.h"
#include "../player.h"
#include "../tilemapManager.h"
#include <random>
#include <vector>

class EnemyManager
{
private:
    // Vectores de los enemigos
    std::vector<ZombieSpawner> zombiesSpawner;
    std::vector<Leopard> leopard;
    std::vector<Bat> bat;

    // Puntero al player
    Player *playerPtr;

    // Generador de números aleatorios
    std::mt19937 globalRng;

public:
    EnemyManager(Player *player);

    void init();
    void update(float deltaTime, const size_t currentLevel, const size_t currentStage);
    void checkCollisions(const size_t currentLevel, const size_t currentStage, const TilemapManager tilemaps);
    void draw(sf::RenderWindow &window, const size_t currentLevel, const size_t currentStage);
};