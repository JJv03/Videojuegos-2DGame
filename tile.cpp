#include <memory>
#include <unordered_map>
#include <vector>
#include "tile.h"
#include "game.h"


std::unordered_map<BreakableType, sf::IntRect, BreakableTypeHash> breakableTextureRects;
std::vector<sf::Texture> breakableAtlas;
std::unordered_map<BreakableType, int> breakableTypeToAtlasIndex;


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


bool loadBreakableTextures()
{
    // If they're already loaded, return true
    if (!breakableTextureRects.empty())
    {
        return true;
    }

    //////////////////////////////////////
    // Load the first texture to the atlas
    //////////////////////////////////////
    sf::Image itemsObjectsImage;
    if (!itemsObjectsImage.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    itemsObjectsImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture tex1;
    if (!tex1.loadFromImage(itemsObjectsImage)) return false;
    tex1.setSmooth(false);
    breakableAtlas.emplace_back(std::move(tex1)); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each breakable tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    // auto firepitTexture = std::make_shared<sf::Texture>();
    // if (!firepitTexture->loadFromImage(itemsObjectsImage, false, sf::IntRect({175, 2}, {16, 31})))
    // {
    //     std::cout << "Error cargando textura Firepit." << std::endl;
    //     return false;
    // }
    breakableTextureRects[BreakableType::FIREPIT] = sf::IntRect({175, 2}, {16, 31});
    breakableTypeToAtlasIndex[BreakableType::FIREPIT] = 0;

    breakableTextureRects[BreakableType::CANDELABRUM] = sf::IntRect({157, 1}, {8, 16});
    breakableTypeToAtlasIndex[BreakableType::CANDELABRUM] = 0;


    //////////////////////////////////////
    // Load the second texture to the atlas
    //////////////////////////////////////
    sf::Image tilesetLvl1Image;
    if (!tilesetLvl1Image.loadFromFile("./assets/tilesets/tileset_1.png")) return false;
    tilesetLvl1Image.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture tex2;
    if (!tex2.loadFromImage(tilesetLvl1Image)) return false;
    tex2.setSmooth(false);
    breakableAtlas.emplace_back(std::move(tex2)); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each breakable tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    breakableTextureRects[BreakableType::BREAKABLE_WALL_1SQUARE] = sf::IntRect({18, 342}, {16, 16});
    breakableTypeToAtlasIndex[BreakableType::BREAKABLE_WALL_1SQUARE] = 1;

    breakableTextureRects[BreakableType::BREAKABLE_WALL_4SQUARES] = sf::IntRect({162, 342}, {16, 16});
    breakableTypeToAtlasIndex[BreakableType::BREAKABLE_WALL_4SQUARES] = 1;

    breakableTextureRects[BreakableType::BREAKABLE_WALL_3SQUARES] = sf::IntRect({146, 326}, {16, 16});
    breakableTypeToAtlasIndex[BreakableType::BREAKABLE_WALL_3SQUARES] = 1;

    return true;
}


void BreakableTile::update(const float& deltaTime) {
    if (this->m_animationManager != nullptr) {
        m_animationManager->update(deltaTime);
    }
}


bool BreakableTile::hasAnimation(BreakableType type) const {
    return (type == BreakableType::FIREPIT || type == BreakableType::CANDELABRUM);
}


const std::vector<AnimationManager::Frame>& getBreakableAnimationFrames(BreakableType type) {
    using Frame = AnimationManager::Frame;

    static const std::unordered_map<BreakableType, std::vector<Frame>, BreakableTypeHash> breakableTypeToAnimation = {
        { BreakableType::FIREPIT, {
            { breakableTextureRects.at(BreakableType::FIREPIT), 0.2f },
            { sf::IntRect({breakableTextureRects.at(BreakableType::FIREPIT).position.x
                            + 1 + breakableTextureRects.at(BreakableType::FIREPIT).size.x,
                           breakableTextureRects.at(BreakableType::FIREPIT).position.y},
                          {breakableTextureRects.at(BreakableType::FIREPIT).size.x,
                           breakableTextureRects.at(BreakableType::FIREPIT).size.y}), 0.2f },
        }},
        { BreakableType::CANDELABRUM, {
            { breakableTextureRects.at(BreakableType::CANDELABRUM), 0.2f },
            { sf::IntRect({breakableTextureRects.at(BreakableType::CANDELABRUM).position.x
                            + 1 + breakableTextureRects.at(BreakableType::CANDELABRUM).size.x,
                           breakableTextureRects.at(BreakableType::CANDELABRUM).position.y},
                          {breakableTextureRects.at(BreakableType::CANDELABRUM).size.x,
                           breakableTextureRects.at(BreakableType::CANDELABRUM).size.y}), 0.2f },
        }},
    };

    static const std::vector<Frame> empty;

    auto it = breakableTypeToAnimation.find(type);
    return (it != breakableTypeToAnimation.end()) ? it->second : empty;
}


std::shared_ptr<sf::Sprite> getBreakableTileSprite(BreakableType type) {
    auto iterator = breakableTextureRects.find(type);
    if (iterator == breakableTextureRects.end()) {
        std::cerr << "[ERROR] Texture for breakable type \"" << static_cast<int>(type) << "\" not found." << std::endl;
        return nullptr;
    }

    sf::IntRect textureRect = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(breakableAtlas[breakableTypeToAtlasIndex[type]]);
    sprite->setTextureRect(textureRect);

    return sprite;
}

std::shared_ptr<BreakableTile> getBreakableTile(const BreakableType type, const sf::FloatRect& hitbox,
                                const bool isBreakable, const DropType dropType) {
    std::shared_ptr<BreakableTile> tile = std::make_shared<BreakableTile>();
    tile->type = static_cast<BreakableType>(type);
    tile->hitboxes.push_back(hitbox);
    tile->isBreakable = isBreakable;
    tile->dropType = dropType;

    tile->sprite = getBreakableTileSprite(tile->type);
    tile->sprite->setPosition(hitbox.position);

    if (tile->hasAnimation(tile->type)) {
        tile->m_animationManager = std::make_unique<AnimationManager>(*tile->sprite);
        tile->m_animation.id = notRelevant;
        tile->m_animation.frames = getBreakableAnimationFrames(tile->type);
        tile->m_animation.loop = true;
        tile->m_animationManager->playAnimation(tile->m_animation);
    } else {
        tile->m_animationManager = nullptr;
    }

    return tile;
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

