#pragma once
#include "entity.h"
#include "item.h"

class Game;
void createDropItemAfterBrokenTile(sf::Vector2f itemPosition, DropType dropType);

class Tile : public Entity
{
public:
    Tile();
    ~Tile() = default;
    void hello() const override;
};

class TileSprite : public EntitySprite
{
public:
    TileSprite();
    ~TileSprite() = default;
};

// -------------------------------------------------

class SolidTile : public Tile
{
public:
    SolidTile();
    ~SolidTile() = default;

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

// -------------------------------------------------

class DoorTile : public Tile {
public:
    enum class Type{   // Types of door tiles. Only in this scope
        CASTLE_ENTRANCE = 0,
        DOOR = 1,
        STAIRS = 2,
    };

    DoorTile();
    ~DoorTile() = default;

    int doorId;
    Type type;                      // Door type
    sf::Vector2f playerAparition;           // Hitbox of the tile. In global coord.

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

// -------------------------------------------------

class BreakableTile : public TileSprite
{
public:
    enum class Type {   // Types of breakable tiles. Only in this scope
        FIREPIT = 0,
        CANDELABRUM = 1,
        BREAKABLE_WALL = 2,
    };

    Type type;                      // Breakable type
    bool isBreakable = true;        // Enabling the tile to be destroyed
    bool isDestroyed = false;       // If the tile is destroyed
    bool generatesItem = false;     // If the tile generates an item when destroyed
    
    DropType dropType = DropType::NONE;          // If the tile drops an item

    BreakableTile();
    ~BreakableTile() = default;

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void onCollision_Whip(Game& game);
    void hello() const override;
};

// -------------------------------------------------

class StairTile : public Tile
{
public:
    enum class Type {   // Types of stair tiles. Only in this scope
        NONE,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
    };

    Type type;                      // Stair type
    
    StairTile();
    ~StairTile() = default;

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

bool getStairType(int level, int tileNumber, StairTile::Type& stairType);


