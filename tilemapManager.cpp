#include "tilemapManager.h"

TilemapManager::TilemapManager() {

}

TilemapManager::~TilemapManager() {

}


void TilemapManager::loadLevel(int level) {
    tilemaps.clear();
    tilemaps.push_back(TileMap());
    //tilemaps[0].load("assets/tileset.png", "assets/level1.txt");
}
    