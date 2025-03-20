#pragma once

#include "tilemap.h"


class TilemapManager
{
public:
    std::vector<TileMap> tilemaps;

    TilemapManager();
    ~TilemapManager();

    void loadLevel(int level);
    void startStage(int stage);
    void goToStage(int from, int to);

    void draw(sf::RenderWindow& window, Camera& camera);
};

