#pragma once

#include "tilemap.h"
#include <map>



class TilemapManager
{
private:
    enum Transition{
        ENTER_CASTLE,
        DOOR,
        STAIRS,
    };

    struct Door{
        int prev_stage;
        int next_stage;
        int transition;
    };

    // Stores in its key <n-1> the number of the door/passage/stairs number n, and
    // in its value the previous and next stage the door takes you from/to
    // and the type of tranisiton it has to make between them
    // If a stage has the number *100*, it means the stage belongs to the first
    // stage of the next level (the next level has to be loaded and started)
    std::vector<Door> doors;
    
public:
    // Stores in <n-1> the tilemaps of the stages n in the current level
    std::vector<TileMap> tilemaps;

    TilemapManager();
    ~TilemapManager();

    void loadLevel(int level);
    int startStage(int stage);
    int goToStage(int from, int to);

    void draw(sf::RenderWindow& window, Camera& camera);
};

