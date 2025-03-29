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
    TileSprite();
    ~TileSprite() = default;
};

// -------------------------------------------------

class SolidTile : public Tile
{
public:
    sf::FloatRect hitbox;           // Hitbox of the tile. In global coord.
    bool isVisible = false;         // If <true>, the tile is visible.

    SolidTile();
    ~SolidTile() = default;
};


