#include "tile.h"
#include "game.h"

// -------------------------- TILE (virtual) --------------------------
Tile::Tile(){
    
}

void Tile::hello() const {
    std::cout << "Soy Tile" << std::endl;
}

// -------------------------- TILE SPRITE (virtual) --------------------------

TileSprite::TileSprite(){
    
}

// -------------------------- SOLID TILE --------------------------

SolidTile::SolidTile() : Tile() {
    
}

void SolidTile::onCollision(Entity& other, Game& game){

}

void SolidTile::hello() const {
    std::cout << "Soy SolidTile" << std::endl;
}

// -------------------------- DOOR TILE --------------------------

DoorTile::DoorTile() : Tile() {
    
}


void DoorTile::onCollision(Entity& other, Game& game){

}

void DoorTile::hello() const {
    std::cout << "Soy DoorTile" << std::endl;
}

// -------------------------- BREAKABLE TILE --------------------------

BreakableTile::BreakableTile() : TileSprite() {

}

void BreakableTile::onCollision(Entity& other, Game& game){
    if (Whip* whip = dynamic_cast<Whip*>(&other)) {
        //std::cout << "Es un SolidTile\n";
        this->onCollision_Whip(*whip, game);
    }
}

void BreakableTile::onCollision_Whip(Whip& whip, Game& game){
    if (this->isDestroyed) return;

    sf::FloatRect tileBounds = this->hitboxes[0];   // Breakable tiles only have 1 hitbox

    // Ignore if empty hitbox
    if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) return;
   
    if (this->isBreakable) {
        this->isDestroyed = true;
        this->generatesItem = true;
        game.createDropItem(this->sprite->getPosition(), this->dropType);
    }
}

void BreakableTile::hello() const {
    std::cout << "Soy BreakableTile" << std::endl;
}