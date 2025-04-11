#pragma once
#include "entity.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class CollisionGrid {
public:
    struct CellEntities {
        std::vector<Entity*> statics;
        std::vector<Entity*> dynamics;
    };

    CollisionGrid();
    CollisionGrid(int cellsPerRow, int cellsPerColumn);

    void setRows(const int& newRows);
    void setColumns(const int& newColumns);

    int getRows();
    int getColumns();

    void addEntity(Entity* entity, const sf::Vector2f& viewPosition);
    void clear();
    void checkCollisions(std::vector<Entity*>& allEntities, const sf::Vector2f& viewPosition);
    void drawCells(sf::RenderWindow& window, const sf::Vector2f& upperLeftCorner);
    
private:
    int cellsPerRow, cellsPerColumn;

    std::unordered_map<int, CellEntities> cells;

    int getCellKeyFromCoords(int x, int y) const;
    std::unordered_set<int> getCellKeysContainingEntity(const Entity& entity, const sf::Vector2f& viewPosition);
};


bool checkIntersections(const Entity& a, const Entity& b);
bool isIntersecting(const sf::FloatRect& a, const sf::FloatRect& b);
