#include "tilemapManager.h"
#include <iostream>

TilemapManager::TilemapManager() {
}

TilemapManager::~TilemapManager() {

}


void TilemapManager::loadLevel(int level) {
    switch (level)
    {
    case 1:
        tilemaps.clear();

        tilemaps.push_back(TileMap());
        tilemaps[0].load(1, 1);

        tilemaps.push_back(TileMap());
        tilemaps[1].load(1, 2);

        tilemaps.push_back(TileMap());
        tilemaps[2].load(1, 3);

        tilemaps.push_back(TileMap());
        tilemaps[3].load(1, 4);

        tilemaps.push_back(TileMap());
        tilemaps[4].load(1, 5);
        
        // ------------------------
        doors.clear();

        doors[1] = {1, 2, DoorTile::Type::CASTLE_ENTRANCE};  // Door 1
        doors[2] = {2, 3, DoorTile::Type::DOOR};          // Door 2
        doors[3] = {3, 4, DoorTile::Type::STAIRS};        // Door 3
        doors[4] = {4, 3, DoorTile::Type::STAIRS};        // Door 4
        doors[5] = {3, 5, DoorTile::Type::DOOR};          // Door 5
        doors[6] = {5, 100, DoorTile::Type::DOOR};        // Door 6

        break;
    
    case 2:
        tilemaps.clear();
        doors.clear();
        break;

    
    case 3:
        tilemaps.clear();
        doors.clear();        
        break;

        
    case 4:
        tilemaps.clear();
        doors.clear();    
        break;

       
    case 5:
        tilemaps.clear();
        doors.clear();
        break;

        
    case 6:
        tilemaps.clear();
        doors.clear();
        break;

    
    case 7:
        tilemaps.clear();
        doors.clear();
        break;

    default:
        break;
    }
}

void TilemapManager::restartBreakableTiles(){
    for(auto& tilemap : tilemaps)
    {
        tilemap.m_items.clear();

        for(auto& breakableTile: tilemap.m_breakableTiles)
        {
            breakableTile.isBreakable = true;
            breakableTile.isDestroyed = false;
        }
    }
}
