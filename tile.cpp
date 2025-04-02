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

sf::FloatRect SolidTile::getBounds() const {
    return sf::FloatRect();
}

void SolidTile::onCollision(Entity& other){

}
// -------------------------- DOOR TILE --------------------------

DoorTile::DoorTile() : Tile() {
    
}

sf::FloatRect DoorTile::getBounds() const {
    return sf::FloatRect();
}

void DoorTile::onCollision(Entity& other){

}

// -------------------------- BREAKABLE TILE --------------------------

BreakableTile::BreakableTile() : TileSprite() {

}

sf::FloatRect BreakableTile::getBounds() const {
    return sf::FloatRect();
}

void BreakableTile::onCollision(Entity& other){

}