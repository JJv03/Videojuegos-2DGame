#include "collisionGrid.h"
#include <cmath>
#include <iostream>

CollisionGrid::CollisionGrid() 
    : cellsPerRow(10), cellsPerColumn(10) {}

CollisionGrid::CollisionGrid(int cellsPerRow, int cellsPerColumn) 
    : cellsPerRow(cellsPerRow), cellsPerColumn(cellsPerColumn) {}


void CollisionGrid::setRows(const int& newRows) { cellsPerRow = newRows; }
void CollisionGrid::setColumns(const int& newColumns) { cellsPerColumn = newColumns; }

int CollisionGrid::getRows() { return cellsPerRow; }
int CollisionGrid::getColumns() { return cellsPerColumn; }

void CollisionGrid::addEntity(Entity* entity, const sf::View& view) {
    for (int key : getCellKeysContainingEntity(*entity, view)) {
        cells[key].push_back(entity);
    }
}

std::vector<int> CollisionGrid::getCellKeysContainingEntity(const Entity& entity, const sf::View& view) const {
    std::vector<int> keys;

    sf::Vector2f viewPosition = view.getCenter() - (view.getSize() / 2.f);

    sf::FloatRect bounds = entity.getBounds();

    float cellWidth = view.getSize().x / cellsPerRow;
    float cellHeight = view.getSize().y / cellsPerColumn;


    int startX = static_cast<int>(std::floor((bounds.position.x - viewPosition.x) / cellWidth));
    int startY = static_cast<int>(std::floor((bounds.position.y - viewPosition.y) / cellHeight));
    int endX = static_cast<int>(std::floor((bounds.position.x + bounds.size.x - viewPosition.x) / cellWidth));
    int endY = static_cast<int>(std::floor((bounds.position.y + bounds.size.y - viewPosition.y) / cellHeight));

    // Iterar sobre todas las celdas cubiertas por la entidad
    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            // Ignorar celdas fuera del grid visible
            if (x < 0 || x >= cellsPerRow || y < 0 || y >= cellsPerColumn)
                continue;
            
            keys.push_back(getCellKeyFromCoords(x, y));
        }
    }

    return keys;
}

void CollisionGrid::clear() {
    cells.clear();
}

int CollisionGrid::getCellKeyFromCoords(int x, int y) const {
    return x + y * cellsPerRow;
}

void CollisionGrid::checkCollisions(std::vector<Entity*>& allEntities, const sf::View& view) {
    for(auto& entity : allEntities){
        addEntity(entity, view);
    }

    for (auto& [key, entities] : cells) {
        if (entities.size() < 2) continue;

        // Check collisions without repeating
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

