#pragma once
#include "entity.h"
#include <vector>
#include <unordered_map>

class CollisionGrid {
public:
    CollisionGrid(float cellSize, int width, int height);

    void addEntity(Entity* entity);
    void clear();
    void checkCollisions();

private:

    float cellSize;
    int width, height;

    std::unordered_map<int, std::vector<Entity*>> cells;

    int computeCellKey(int x, int y) const;
    std::vector<int> getKeysForEntity(const Entity& entity) const;
};
