#include "collisionGrid.h"
#include <cmath>
#include <iostream>
#include "globals.h"

CollisionGrid::CollisionGrid() 
    : cellsPerRow(9), cellsPerColumn(9) {}

CollisionGrid::CollisionGrid(int cellsPerRow, int cellsPerColumn) 
    : cellsPerRow(cellsPerRow), cellsPerColumn(cellsPerColumn) {}


void CollisionGrid::setRows(const int& newRows) { cellsPerRow = newRows; }
void CollisionGrid::setColumns(const int& newColumns) { cellsPerColumn = newColumns; }

int CollisionGrid::getRows() { return cellsPerRow; }
int CollisionGrid::getColumns() { return cellsPerColumn; }

void CollisionGrid::addEntity(Entity* entity, const sf::Vector2f& viewPosition) {
    for (int key : getCellKeysContainingEntity(*entity, viewPosition)) {
        cells[key].push_back(entity);
    }
}

std::vector<int> CollisionGrid::getCellKeysContainingEntity(const Entity& entity, const sf::Vector2f& viewPosition) {
    std::vector<int> keys;

    sf::FloatRect bounds = entity.getBounds();
    //std::cout << bounds.size.x << ", " << bounds.size.y << std::endl;
    if (bounds.size.x == 0.f || bounds.size.y == 0.f){ return {}; }

    float cellWidth = gGameGS_size_x / static_cast<float>(cellsPerRow);
    float cellHeight = gGameGS_size_y / static_cast<float>(cellsPerColumn);

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

void CollisionGrid::checkCollisions(std::vector<Entity*>& allEntities, const sf::Vector2f& viewPosition) {
    cells.clear();

    for(auto& entity : allEntities){
        addEntity(entity, viewPosition);
    }

    //std::cout << "All Entities Size: " << allEntities.size() << std::endl;

    for (auto& [key, entities] : cells) {
        //std::cout << "Cell " << key << " - Size: " << entities.size() << std::endl;

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

void CollisionGrid::drawCells(sf::RenderWindow& window, const sf::Vector2f& viewPosition) {
    
    float cellWidth = gGameGS_size_x / static_cast<float>(cellsPerRow);
    float cellHeight = gGameGS_size_y / static_cast<float>(cellsPerColumn);
    
    //std::cout << "Cell size: x = " << cellWidth << ", y = " << cellHeight << std::endl;
    
    for (int x = 0; x < cellsPerRow; ++x) {
        for (int y = 0; y < cellsPerColumn; ++y) {
            sf::RectangleShape cellShape(sf::Vector2f(cellWidth, cellHeight));
            cellShape.setPosition({viewPosition.x + x * cellWidth, viewPosition.y + y * cellHeight});

            // Colorea la celda si tiene entidades
            int cellKey = getCellKeyFromCoords(x, y);
            if (!cells[cellKey].empty()) {
                cellShape.setFillColor(sf::Color(0, 255, 0, 100)); // Verde semi-transparente
            } else {
                cellShape.setFillColor(sf::Color(0, 0, 0, 0)); // Transparente
            }

            cellShape.setOutlineColor(sf::Color::White);
            cellShape.setOutlineThickness(1.f);

            window.draw(cellShape);
        }
    }
}