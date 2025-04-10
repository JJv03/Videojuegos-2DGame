#include "tile.h"

// -------------------------- TILE (virtual) --------------------------
Tile::Tile(){
    
}

// -------------------------- TILE SPRITE (virtual) --------------------------

TileSprite::TileSprite(){
    
}

// -------------------------- SOLID TILE --------------------------

SolidTile::SolidTile() : Tile() {
    
}

void SolidTile::onCollision(Entity& other){

}

// -------------------------- DOOR TILE --------------------------

DoorTile::DoorTile() : Tile() {
    
}


void DoorTile::onCollision(Entity& other){

}

// -------------------------- BREAKABLE TILE --------------------------

BreakableTile::BreakableTile() : TileSprite() {

}

void BreakableTile::onCollision(Entity& other){

}