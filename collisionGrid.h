#pragma once
#include "entity.h"
#include <vector>
#include <unordered_map>

class CollisionGrid {
public:
    CollisionGrid();
    CollisionGrid(int cellsPerRow, int cellsPerColumn);

    void setRows(const int& newRows);
    void setColumns(const int& newColumns);

    int getRows();
    int getColumns();

    void addEntity(Entity* entity, const sf::View& view);
    void clear();
    void checkCollisions(std::vector<Entity*>& allEntities, const sf::View& view);
    void drawCells(sf::RenderWindow& window, const sf::View& view, const sf::Vector2f& upperLeftCorner);
    
private:
    int cellsPerRow, cellsPerColumn;

    std::unordered_map<int, std::vector<Entity*>> cells;

    int getCellKeyFromCoords(int x, int y) const;
    std::vector<int> getCellKeysContainingEntity(const Entity& entity, const sf::View& view) const;
};
