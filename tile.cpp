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

bool BreakableTile::isCollidable() const {
    if (this->type != BreakableType::CANDELABRUM && this->type != BreakableType::FIREPIT) {
        return true;
    }
    return false;
}

void BreakableTile::onCollision(Entity& other, Game& game){
    if (this->isDestroyed) return;  // The tile is destroyed --> "doesn't exist" anymore

    sf::FloatRect tileBounds = this->hitboxes[0];   // Breakable tiles only have 1 hitbox
    if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) return;  // No hitbox

    if (dynamic_cast<Whip*>(&other)) {
        //std::cout << "Es un SolidTile\n";
        this->onCollision_Whip(game);
    }
    else if (dynamic_cast<Player*>(&other)) {
        //std::cout << "Es un Player\n";
        if (isCollidable()) {
            this->onCollision_Player(other, game);
        }
    }
}

void BreakableTile::onCollision_Whip(Game& game){
    if (this->isBreakable) {
        this->isDestroyed = true;
        this->generatesItem = true;
        game.createDropItem(this->sprite->getPosition(), this->dropType);
    }
}

void BreakableTile::onCollision_Player(Entity& other, Game& game) {
    sf::FloatRect tileBounds = this->hitboxes[0];   // Breakable tiles only have 1 hitbox

    bool hasCollided = false;
    game.computePlayerTileIntersection(hasCollided, tileBounds);
    // Falta ponerlo en el suelo ?
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

