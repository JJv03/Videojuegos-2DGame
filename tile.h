#pragma once
#include "entity.h"


class Tile : public Entity
{
public:
    Tile();
    ~Tile() = default;
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
    void onCollision(Entity& other) override;
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

    Type type;                      // Door type
    sf::Vector2f playerAparition;           // Hitbox of the tile. In global coord.

    // Entity functions
    void onCollision(Entity& other) override;
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

    enum class DropType {       // Types of items that can drop from breakable tiles. Only in this scope
        NONE = 0,               // No item drops
        DEFAULT = 1,            // Default item drop (whip, secondary weapon, etc). Should be handled in game logic
        CROWN = 2,
        MEAT = 3,
    };

    Type type;                      // Breakable type
    bool isBreakable = true;        // Enabling the tile to be destroyed
    bool isDestroyed = false;       // If the tile is destroyed
    DropType dropItem = DropType::NONE;          // If the tile drops an item

    BreakableTile();
    ~BreakableTile() = default;

    // Entity functions
    void onCollision(Entity& other) override;
};


