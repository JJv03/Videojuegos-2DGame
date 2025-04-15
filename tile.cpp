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
    if (dynamic_cast<Whip*>(&other)) {
        //std::cout << "Es un SolidTile\n";
        this->onCollision_Whip(game);
    }
}

void BreakableTile::onCollision_Whip(Game& game){
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


// -------------------------- STAIR TILE --------------------------

StairTile::StairTile() : Tile() {
    
}


void StairTile::onCollision(Entity& other, Game& game){

}

void StairTile::hello() const {
    std::cout << "Soy StairTile" << std::endl;
}

bool getStairType(int level, int tileNumber, StairTile::Type& stairType){
    switch(level){
        case 1:
            switch(tileNumber){
                case 13:
                    stairType = StairTile::Type::BOTTOM_LEFT;
                    return true;

                case 22:
                case 66:
                    stairType = StairTile::Type::BOTTOM_RIGHT;
                    return true;
                    
                case 6:
                case 23:
                    stairType = StairTile::Type::TOP_LEFT;
                    return true;
                
                case 2:
                    stairType = StairTile::Type::TOP_RIGHT;
                    return true;
            }

        default:
            return false;
    }
}

