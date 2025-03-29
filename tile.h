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
    bool isVisible = false;         // If <true>, the tile is visible.

    SolidTile();
    ~SolidTile() = default;
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
};
