#pragma once
#include "entity.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Game; // forward declaration

class CollisionGrid {
public:
    CollisionGrid();
    CollisionGrid(Game* game);
    CollisionGrid(Game* game, int cellsPerRow, int cellsPerColumn);

    void setRows(const int& newRows);
    void setColumns(const int& newColumns);

    int getRows();
    int getColumns();

    void addEntity(Entity* entity, const sf::Vector2f& viewPosition);
    void addStaticEntity(Entity* entity, const sf::Vector2f& viewPosition);
    void addDynamicEntity(Entity* entity, const sf::Vector2f& viewPosition);
    
    void clear();
    void checkCollisions(std::vector<Entity*>& staticEntities, std::vector<Entity*>& dynamicEntities, const sf::Vector2f& viewPosition);
    void drawCells(sf::RenderWindow& window, const sf::Vector2f& upperLeftCorner);
    
private:
    Game* gameRef = nullptr;
    int cellsPerRow, cellsPerColumn;

    std::unordered_map<int, std::vector<Entity*>> cellsWithStatics;
    std::unordered_map<int, std::vector<Entity*>> cellsWithDynamics;


    int getCellKeyFromCoords(int x, int y) const;
    std::unordered_set<int> getCellKeysContainingEntity(const Entity& entity, const sf::Vector2f& viewPosition);
};


bool checkIntersections(const Entity& a, const Entity& b);
bool isIntersecting(const sf::FloatRect& a, const sf::FloatRect& b);
