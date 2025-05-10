#include "collisionGrid.h"
#include <cmath>
#include <iostream>
#include "globals.h"
#include "tile.h"
#include "player.h"
#include <set>
#include <algorithm>
#include <utility>

CollisionGrid::CollisionGrid() 
    : gameRef(nullptr), cellsPerRow(9), cellsPerColumn(9) {}
    

CollisionGrid::CollisionGrid(Game* game) 
    : gameRef(game), cellsPerRow(9), cellsPerColumn(9) {}
    

CollisionGrid::CollisionGrid(Game* game, int cellsPerRow, int cellsPerColumn) 
    : gameRef(game), cellsPerRow(cellsPerRow), cellsPerColumn(cellsPerColumn)  {}


void CollisionGrid::setRows(const int& newRows) { cellsPerRow = newRows; }
void CollisionGrid::setColumns(const int& newColumns) { cellsPerColumn = newColumns; }

int CollisionGrid::getRows() { return cellsPerRow; }
int CollisionGrid::getColumns() { return cellsPerColumn; }

void CollisionGrid::addEntity(Entity* entity, const sf::Vector2f& viewPosition) {
    for (int key : getCellKeysContainingEntity(*entity, viewPosition)) {
        if(dynamic_cast<Tile*>(entity)){ // Tile = Static entity
            cellsWithStatics[key].push_back(entity);
        } else {
            cellsWithDynamics[key].push_back(entity);
        }
    }
}

void CollisionGrid::addStaticEntity(Entity* entity, const sf::Vector2f& viewPosition) {
    for (int key : getCellKeysContainingEntity(*entity, viewPosition)) {
        cellsWithStatics[key].push_back(entity);
    }
}

void CollisionGrid::addDynamicEntity(Entity* entity, const sf::Vector2f& viewPosition) {
    for (int key : getCellKeysContainingEntity(*entity, viewPosition)) {
        cellsWithDynamics[key].push_back(entity);
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
    cellsWithDynamics.clear();
    cellsWithStatics.clear();
}

int CollisionGrid::getCellKeyFromCoords(int x, int y) const {
    return x + y * cellsPerRow;
}


void CollisionGrid::checkCollisions(std::vector<Entity*>& staticEntities, std::vector<Entity*>& dynamicEntities, const sf::Vector2f& viewPosition) {

    std::set<std::pair<const Entity*, const Entity*>> checkedPairs; // So the same entities don't collide twice in different cells

    // 1. Check collisions between static and dynamic entities
    //    Clear all cells
    clear();

    //    Add all entities to the grid
    for(auto& entity : staticEntities){
        addStaticEntity(entity, viewPosition);
    }

    for(auto& entity : dynamicEntities){
        addDynamicEntity(entity, viewPosition);
    }

    //    Check static-dynamic collisions
    for (auto& [key, staticEntities] : cellsWithStatics) {
        auto& dynamicEntities = cellsWithDynamics[key]; // Might be empty

        if (dynamicEntities.empty() || staticEntities.empty()) continue;
        if (dynamicEntities.size() + staticEntities.size() < 2) continue;

        for (Entity* d : dynamicEntities) {
            for (Entity* s : staticEntities) {
                auto orderedPair = std::minmax(d, s); // A,B = B,A
                if (checkedPairs.contains(orderedPair)) continue;
                checkedPairs.insert(orderedPair);
                
                std::optional<sf::FloatRect> intersectionRect = checkIntersections(*d, *s);
                if (intersectionRect) {
                    d->onCollision(*s, *gameRef, *intersectionRect);
                    s->onCollision(*d, *gameRef, *intersectionRect);
                }
                else if (WaterZone* waterZone = dynamic_cast<WaterZone*>(s)) {
                    waterZone->clearEntity(*d);
                }
            }
        }
    }


    // 2. Check collisions between dynamic entities
    //    Update dynamic entities positions
    checkedPairs.clear();
    cellsWithDynamics.clear();
    for(auto& entity : dynamicEntities){
        addDynamicEntity(entity, viewPosition);
    }

    //std::cout << "Cell " << key << " - Size: " << entities.size() << std::endl;
    for (auto& [key, dynamicEntities] : cellsWithDynamics) {
        if (dynamicEntities.size() < 2) continue;

        for (size_t i = 0; i < dynamicEntities.size(); ++i) {
            for (size_t j = i + 1; j < dynamicEntities.size(); ++j) {
                auto orderedPair = std::minmax(dynamicEntities[i], dynamicEntities[j]); // A,B = B,A
                if (checkedPairs.contains(orderedPair)) continue;
                checkedPairs.insert(orderedPair);
                
                std::optional<sf::FloatRect> intersectionRect = checkIntersections(*dynamicEntities[i], *dynamicEntities[j]);
                if (intersectionRect) {
                    dynamicEntities[i]->onCollision(*dynamicEntities[j], *gameRef, *intersectionRect);
                    dynamicEntities[j]->onCollision(*dynamicEntities[i], *gameRef, *intersectionRect);

                    if(Whip* whip = dynamic_cast<Whip*>(dynamicEntities[i])) whip->collisionedEntities.insert(dynamicEntities[j]);
                    if(Whip* whip = dynamic_cast<Whip*>(dynamicEntities[j])) whip->collisionedEntities.insert(dynamicEntities[i]);
                    if(SubWeapon* subweapon = dynamic_cast<SubWeapon*>(dynamicEntities[i])) subweapon->collisionedEntities.insert(dynamicEntities[j]);
                    if(SubWeapon* subweapon = dynamic_cast<SubWeapon*>(dynamicEntities[j])) subweapon->collisionedEntities.insert(dynamicEntities[i]);
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
            if (!cellsWithDynamics[cellKey].empty() || !cellsWithStatics[cellKey].empty()) {
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


std::optional<sf::FloatRect> checkIntersections(const Entity& entityA, const Entity& entityB) {
    for(sf::FloatRect a : entityA.getBounds()){
        for(sf::FloatRect b : entityB.getBounds()){
            if(const std::optional<sf::FloatRect> intersection = a.findIntersection(b))
                return intersection;
        }
    }
    return std::nullopt;
}


bool isIntersecting(const sf::FloatRect& a, const sf::FloatRect& b) {
    return !(a.position.x + a.size.x < b.position.x ||
             a.position.x > b.position.x + b.size.x ||
             a.position.y + a.size.y < b.position.y ||
             a.position.y > b.position.y + b.size.y);
}