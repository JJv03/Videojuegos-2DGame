#include "tilemapManager.h"

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
        tilemaps[0].load("assets/tilesets/tileset_1.png", "assets/tilemaps/tilemap_1_1.txt");

        tilemaps.push_back(TileMap());
        tilemaps[1].load("assets/tilesets/tileset_1.png", "assets/tilemaps/tilemap_1_2.txt");

        //tilemaps.push_back(TileMap());
        //tilemaps[2].load("assets/tilesets/tileset_1.png", "assets/tilemaps/tilemap_1_3.txt");

        //tilemaps.push_back(TileMap());
        //tilemaps[3].load("assets/tilesets/tileset_1.png", "assets/tilemaps/tilemap_1_4.txt");

        //tilemaps.push_back(TileMap());
        //tilemaps[4].load("assets/tilesets/tileset_1.png", "assets/tilemaps/tilemap_1_5.txt");
        
        // ------------------------
        doors.clear();

        doors.push_back({1, 2, ENTER_CASTLE});  // Door 1
        doors.push_back({2, 3, DOOR});          // Door 2
        doors.push_back({3, 4, STAIRS});        // Door 3
        doors.push_back({4, 3, STAIRS});        // Door 4
        doors.push_back({3, 5, DOOR});          // Door 5
        doors.push_back({5, 100, DOOR});        // Door 6

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
    

int TilemapManager::startStage(int stage){
    if(stage > tilemaps.size()){
        return -1;
    }

    return stage;
}


int TilemapManager::goToStage(int from, int to){

}
