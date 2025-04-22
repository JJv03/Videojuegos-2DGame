#include <memory>
#include <unordered_map>
#include <vector>
#include "tile.h"
#include "game.h"


std::unordered_map<MiscTileType, sf::IntRect, MiscTileTypeHash> miscTileTextureRects;
std::vector<sf::Texture> miscTileAtlas;
std::unordered_map<MiscTileType, int> miscTileTypeToAtlasIndex;


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

// -------------------------- MISCELLANEOUS TILE --------------------------

MiscellaneousTile::MiscellaneousTile() : TileSprite() {

}


bool loadMiscTextures()
{
    // If they're already loaded, return true
    if (!miscTileTextureRects.empty())
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
    miscTileAtlas.emplace_back(std::move(tex1)); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    // auto firepitTexture = std::make_shared<sf::Texture>();
    // if (!firepitTexture->loadFromImage(itemsObjectsImage, false, sf::IntRect({175, 2}, {16, 31})))
    // {
    //     std::cout << "Error cargando textura Firepit." << std::endl;
    //     return false;
    // }
    miscTileTextureRects[MiscTileType::FIREPIT] = sf::IntRect({175, 2}, {16, 31});
    miscTileTypeToAtlasIndex[MiscTileType::FIREPIT] = 0;

    miscTileTextureRects[MiscTileType::CANDELABRUM] = sf::IntRect({157, 1}, {8, 16});
    miscTileTypeToAtlasIndex[MiscTileType::CANDELABRUM] = 0;


    //////////////////////////////////////
    // Load the second texture to the atlas
    //////////////////////////////////////
    sf::Image tilesetLvl1Image;
    if (!tilesetLvl1Image.loadFromFile("./assets/tilesets/tileset_1.png")) return false;
    tilesetLvl1Image.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture tex2;
    if (!tex2.loadFromImage(tilesetLvl1Image)) return false;
    tex2.setSmooth(false);
    miscTileAtlas.emplace_back(std::move(tex2)); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscTileTextureRects[MiscTileType::WALL_1SQUARE] = sf::IntRect({18, 342}, {16, 16});
    miscTileTypeToAtlasIndex[MiscTileType::WALL_1SQUARE] = 1;

    miscTileTextureRects[MiscTileType::WALL_4SQUARES] = sf::IntRect({162, 342}, {16, 16});
    miscTileTypeToAtlasIndex[MiscTileType::WALL_4SQUARES] = 1;

    miscTileTextureRects[MiscTileType::WALL_3SQUARES] = sf::IntRect({146, 326}, {16, 16});
    miscTileTypeToAtlasIndex[MiscTileType::WALL_3SQUARES] = 1;

    miscTileTextureRects[MiscTileType::DROP_TRIGGER] = sf::IntRect({120, 56}, {1, 1});
    miscTileTypeToAtlasIndex[MiscTileType::DROP_TRIGGER] = 1;

    return true;
}


void MiscellaneousTile::update(const float& deltaTime) {
    if (this->m_animationManager != nullptr) {
        m_animationManager->update(deltaTime);
    }
}


bool MiscellaneousTile::hasAnimation(MiscTileType type) const {
    return (type == MiscTileType::FIREPIT || type == MiscTileType::CANDELABRUM);
}


const std::vector<AnimationManager::Frame>& getMiscAnimationFrames(MiscTileType type) {
    using Frame = AnimationManager::Frame;

    static const std::unordered_map<MiscTileType, std::vector<Frame>, MiscTileTypeHash> miscTileTypeToAnimation = {
        { MiscTileType::FIREPIT, {
            { miscTileTextureRects.at(MiscTileType::FIREPIT), 0.2f },
            { sf::IntRect({miscTileTextureRects.at(MiscTileType::FIREPIT).position.x
                            + 1 + miscTileTextureRects.at(MiscTileType::FIREPIT).size.x,
                           miscTileTextureRects.at(MiscTileType::FIREPIT).position.y},
                          {miscTileTextureRects.at(MiscTileType::FIREPIT).size.x,
                           miscTileTextureRects.at(MiscTileType::FIREPIT).size.y}), 0.2f },
        }},
        { MiscTileType::CANDELABRUM, {
            { miscTileTextureRects.at(MiscTileType::CANDELABRUM), 0.2f },
            { sf::IntRect({miscTileTextureRects.at(MiscTileType::CANDELABRUM).position.x
                            + 1 + miscTileTextureRects.at(MiscTileType::CANDELABRUM).size.x,
                           miscTileTextureRects.at(MiscTileType::CANDELABRUM).position.y},
                          {miscTileTextureRects.at(MiscTileType::CANDELABRUM).size.x,
                           miscTileTextureRects.at(MiscTileType::CANDELABRUM).size.y}), 0.2f },
        }},
    };

    static const std::vector<Frame> empty;

    auto it = miscTileTypeToAnimation.find(type);
    return (it != miscTileTypeToAnimation.end()) ? it->second : empty;
}


std::shared_ptr<sf::Sprite> getMiscTileSprite(MiscTileType type) {
    auto iterator = miscTileTextureRects.find(type);
    if (iterator == miscTileTextureRects.end()) {
        std::cerr << "[ERROR] Texture for miscellaneous type \"" << static_cast<int>(type) << "\" not found." << std::endl;
        return nullptr;
    }

    sf::IntRect textureRect = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(miscTileAtlas[miscTileTypeToAtlasIndex[type]]);
    sprite->setTextureRect(textureRect);

    return sprite;
}

std::shared_ptr<MiscellaneousTile> getMiscTile(const MiscTileType type, const sf::FloatRect& hitbox,
                                const bool isBreakable, const DropType dropType, const int dropItem_x,
                                const int dropItem_y) {
    std::shared_ptr<MiscellaneousTile> tile = std::make_shared<MiscellaneousTile>();
    tile->type = static_cast<MiscTileType>(type);
    tile->hitboxes.push_back(hitbox);
    tile->isBreakable = isBreakable;
    tile->dropType = dropType;
    tile->dropItem_position.x = dropItem_x;
    tile->dropItem_position.y = dropItem_y;

    tile->sprite = getMiscTileSprite(tile->type);
    tile->sprite->setPosition(hitbox.position);

    if (tile->hasAnimation(tile->type)) {
        tile->m_animationManager = std::make_unique<AnimationManager>(*tile->sprite);
        tile->m_animation.id = notRelevant;
        tile->m_animation.frames = getMiscAnimationFrames(tile->type);
        tile->m_animation.loop = true;
        tile->m_animationManager->playAnimation(tile->m_animation);
    } else {
        tile->m_animationManager = nullptr;
    }

    return tile;
}

bool MiscellaneousTile::isCollidable() const {
    if (this->type != MiscTileType::CANDELABRUM && this->type != MiscTileType::FIREPIT) {
        return true;
    }
    return false;
}

void MiscellaneousTile::onCollision(Entity& other, Game& game){
    if (this->isDestroyed) return;  // The tile is destroyed --> "doesn't exist" anymore

    sf::FloatRect tileBounds = this->hitboxes[0];   // Misc tiles only have 1 hitbox
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

void MiscellaneousTile::onCollision_Whip(Game& game){
    if (this->isBreakable) {
        this->isDestroyed = true;
        game.createDropItem(this->dropType, this->sprite->getPosition());
    }
}

void MiscellaneousTile::onCollision_Player(Entity& other, Game& game) {
    sf::FloatRect tileBounds = this->hitboxes[0];   // Misc tiles only have 1 hitbox

    if (this->type != MiscTileType::DROP_TRIGGER) {
        bool hasCollided = false;
        game.computePlayerTileIntersection(hasCollided, tileBounds);
    }
    else {
        game.createDropItem(this->dropType,
                            sf::Vector2f{static_cast<float>(this->dropItem_position.x),
                                         static_cast<float>(this->dropItem_position.y)});
        this->isDestroyed = true;
    }
}

void MiscellaneousTile::hello() const {
    std::cout << "Soy MiscellaneousTile" << std::endl;
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

