#include "collisionGrid.h"
#include <cmath>

CollisionGrid::CollisionGrid(float cellSize, int width, int height) 
    : cellSize(cellSize), width(width), height(height) {}

void CollisionGrid::addEntity(Entity* entity) {
    for (int key : getKeysForEntity(*entity)) {
        cells[key].push_back(entity);
    }
}

void CollisionGrid::clear() {
    cells.clear();
}

int CollisionGrid::computeCellKey(int x, int y) const {
    return x + y * width;
}

void CollisionGrid::checkCollisions() {
    for (auto& [key, entities] : cells) {
        if (entities.size() < 2) continue;

        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                // TODO: detectar la colision entre los bounds de las entidades
                //if (entities[i]->getBounds().intersects(entities[j]->getBounds())) { // IF COLLISION
                //    entities[i]->onCollision(*entities[j]);
                //    entities[j]->onCollision(*entities[i]);
                //}
            }
        }
    }
}
