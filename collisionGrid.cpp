#include "collisionGrid.h"
#include <cmath>
#include <iostream>
#include "globals.h"
#include "tile.h"

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
        if(dynamic_cast<Tile*>(entity)){ // Tile = Static entity
            cells[key].statics.push_back(entity);
        } else {
            cells[key].dynamics.push_back(entity);
        }
    }
}

std::unordered_set<int> CollisionGrid::getCellKeysContainingEntity(const Entity& entity, const sf::Vector2f& viewPosition) {
    std::unordered_set<int> keys;
    float cellWidth = gGameGS_size_x / static_cast<float>(cellsPerRow);
    float cellHeight = gGameGS_size_y / static_cast<float>(cellsPerColumn);

    for(sf::FloatRect bounds : entity.getBounds()){
        //std::cout << bounds.size.x << ", " << bounds.size.y << std::endl;
        if (bounds.size.x == 0.f || bounds.size.y == 0.f){ return {}; }

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

                keys.insert(getCellKeyFromCoords(x, y));
            }
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

    // 1. Check collisions between static and dynamic entities
    cells.clear();
    for(auto& entity : allEntities){
        addEntity(entity, viewPosition);
    }

    for (auto& [key, cellEntities] : cells) {
        if (cellEntities.statics.size() + cellEntities.dynamics.size() < 2) continue;

        for (Entity* d : cellEntities.dynamics) {
            for (Entity* s : cellEntities.statics) {
                if (checkIntersections(*d, *s)) {
                    d->onCollision(*s);
                    s->onCollision(*d);
                }
            }
        }
    }



    // 2. Check collisions between dynamic entities
    cells.clear();
    for(auto& entity : allEntities){
        addEntity(entity, viewPosition);
    }

    //std::cout << "Cell " << key << " - Size: " << entities.size() << std::endl;
    for (auto& [key, cellEntities] : cells) {
        if (cellEntities.dynamics.size() < 2) continue;

        for (size_t i = 0; i < cellEntities.dynamics.size(); ++i) {
            for (size_t j = i + 1; j < cellEntities.dynamics.size(); ++j) {
                if (checkIntersections(*cellEntities.dynamics[i], *cellEntities.dynamics[j])) {
                    cellEntities.dynamics[i]->onCollision(*cellEntities.dynamics[j]);
                    cellEntities.dynamics[j]->onCollision(*cellEntities.dynamics[i]);
                }
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
            if (!cells[cellKey].dynamics.empty() || !cells[cellKey].statics.empty()) {
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


bool checkIntersections(const Entity& entityA, const Entity& entityB) {
    
    for(sf::FloatRect a : entityA.getBounds()){
        for(sf::FloatRect b : entityB.getBounds()){
            if(isIntersecting(a, b)) return true;
        }
    }
    return false;
}

bool isIntersecting(const sf::FloatRect& a, const sf::FloatRect& b){
    return !(a.position.x + a.size.x < b.position.x ||
             a.position.x > b.position.x + b.size.x ||
             a.position.y + a.size.y < b.position.y ||
             a.position.y > b.position.y + b.size.y);
}