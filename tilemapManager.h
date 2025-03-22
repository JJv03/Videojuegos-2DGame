#pragma once

#include "tilemap.h"
#include <map>

class TilemapManager
{
public:

    struct Door{
        int prev_stage;
        int next_stage;
        TileMap::DoorTile::Type type; // For transitions
    };

    // Stores in its key <n-1> the number of the door/passage/stairs number n, and
    // in its value the previous and next stage the door takes you from/to
    // and the type of tranisiton it has to make between them
    // If a stage has the number *100*, it means the stage belongs to the first
    // stage of the next level (the next level has to be loaded and started)
    std::unordered_map<int, Door> doors;

    // Stores in <n-1> the tilemaps of the stages n in the current level
    std::vector<TileMap> tilemaps;

    TilemapManager();
    ~TilemapManager();

    void loadLevel(int level);

    TileMap& operator[](int i) {
        return tilemaps[i-1];
    }

    const TileMap& operator[](int i) const {
        return tilemaps.at(i-1);
    }
};

