#include <memory>
#include <unordered_map>
#include <vector>
#include "tile.h"
#include "game.h"
#include "globals.h"


std::unordered_map<MiscTileType, sf::IntRect, MiscTileTypeHash> miscType_to_TextureRects;
std::unordered_map<MiscTileType, std::string> miscType_to_TextureAtlasKey;


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

void SolidTile::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect){

}

void SolidTile::hello() const {
    std::cout << "Soy SolidTile" << std::endl;
}

// -------------------------- DOOR TILE --------------------------

DoorTile::DoorTile() : Tile() {
    
}


void DoorTile::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect){

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
    if (!miscType_to_TextureRects.empty())
    {
        return true;
    }

    //////////////////////////////////////
    // Load the lvl1 miscTiles first texture to the atlas
    //////////////////////////////////////
    sf::Image itemsObjectsImage;
    if (!itemsObjectsImage.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    itemsObjectsImage.createMaskFromColor(gColorKeyGrey);

    sf::Texture tex1;
    if (!tex1.loadFromImage(itemsObjectsImage)) return false;
    tex1.setSmooth(false);
    gTextures["miscTile_1"] = std::move(tex1); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscType_to_TextureRects[MiscTileType::FIREPIT] = sf::IntRect({175, 2}, {16, 31});
    miscType_to_TextureAtlasKey[MiscTileType::FIREPIT] = "miscTile_1";

    miscType_to_TextureRects[MiscTileType::CANDELABRUM] = sf::IntRect({157, 1}, {8, 16});
    miscType_to_TextureAtlasKey[MiscTileType::CANDELABRUM] = "miscTile_1";


    //////////////////////////////////////
    // Load the lvl1 miscTiles second texture to the atlas
    //////////////////////////////////////
    sf::Image tilesetLvl1Image;
    if (!tilesetLvl1Image.loadFromFile("./assets/tilesets/tileset_1.png")) return false;
    tilesetLvl1Image.createMaskFromColor(gColorKeyGrey);

    sf::Texture tex2;
    if (!tex2.loadFromImage(tilesetLvl1Image)) return false;
    tex2.setSmooth(false);
    gTextures["miscTile_2"] = std::move(tex2); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscType_to_TextureRects[MiscTileType::LVL1_WALL_1SQUARE] = sf::IntRect({18, 342}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_1SQUARE] = "miscTile_2";

    miscType_to_TextureRects[MiscTileType::LVL1_WALL_4SQUARES] = sf::IntRect({162, 342}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_4SQUARES] = "miscTile_2";

    miscType_to_TextureRects[MiscTileType::LVL1_WALL_3SQUARES] = sf::IntRect({146, 326}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_3SQUARES] = "miscTile_2";

    miscType_to_TextureRects[MiscTileType::DROP_TRIGGER] = sf::IntRect({120, 56}, {1, 1});
    miscType_to_TextureAtlasKey[MiscTileType::DROP_TRIGGER] = "miscTile_2";


    //////////////////////////////////////
    // Load the lvl3 texture to the atlas
    //////////////////////////////////////
    sf::Image tilesetLvl3Image;
    if (!tilesetLvl3Image.loadFromFile("./assets/tilesets/tileset_3.png")) return false;
    tilesetLvl3Image.createMaskFromColor(gColorKeyGrey);
    sf::Texture tex4;
    if (!tex4.loadFromImage(tilesetLvl3Image)) return false;
    tex4.setSmooth(false);
    gTextures["miscTile_3"] = std::move(tex4);

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscType_to_TextureRects[MiscTileType::LVL3_WALL_1SQUARE] = sf::IntRect({2, 182}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL3_WALL_1SQUARE] = "miscTile_3";


    //////////////////////////////////////
    // Load the lvl5 texture to the atlas
    //////////////////////////////////////
    sf::Image tilesetLvl5Image;
    if (!tilesetLvl5Image.loadFromFile("./assets/tilesets/tileset_5.png")) return false;
    tilesetLvl5Image.createMaskFromColor(gColorKeyGrey);
    sf::Texture tex5;
    if (!tex5.loadFromImage(tilesetLvl5Image)) return false;
    tex5.setSmooth(false);
    gTextures["miscTile_5"] = std::move(tex5);

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscType_to_TextureRects[MiscTileType::LVL5_WALL_1SQUARE] = sf::IntRect({74, 74}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL5_WALL_1SQUARE] = "miscTile_5";


    //////////////////////////////////////
    // Load the custom textures to the atlas
    //////////////////////////////////////
    sf::Image tilesetMiscTilesCustom;
    if (!tilesetMiscTilesCustom.loadFromFile("./assets/tilesets/miscTiles.png")) return false;
    tilesetMiscTilesCustom.createMaskFromColor(gColorKeyGreen);

    sf::Texture tex3;
    if (!tex3.loadFromImage(tilesetMiscTilesCustom)) return false;
    tex3.setSmooth(false);
    gTextures["miscTileCustom"] = std::move(tex3); 

    ////////////////////////////////////////////////////////////////////////////
    // Load the texture rectangles for each custom miscellaneous tile of that texture atlas
    ////////////////////////////////////////////////////////////////////////////
    miscType_to_TextureRects[MiscTileType::LVL1_WALL_1SQUARE_BROKE] = sf::IntRect({10, 19}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_1SQUARE_BROKE] = "miscTileCustom";

    miscType_to_TextureRects[MiscTileType::LVL1_WALL_4SQUARES_BROKE] = sf::IntRect({44, 19}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_4SQUARES_BROKE] = "miscTileCustom";

    miscType_to_TextureRects[MiscTileType::LVL1_WALL_3SQUARES_BROKE] = sf::IntRect({27, 19}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL1_WALL_3SQUARES_BROKE] = "miscTileCustom";

    miscType_to_TextureRects[MiscTileType::LVL3_WALL_1SQUARE_BROKE] = sf::IntRect({74, 19}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL3_WALL_1SQUARE_BROKE] = "miscTileCustom";

    miscType_to_TextureRects[MiscTileType::LVL5_WALL_1SQUARE_BROKE] = sf::IntRect({104, 19}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::LVL5_WALL_1SQUARE_BROKE] = "miscTileCustom";

    miscType_to_TextureRects[MiscTileType::INVISIBLE_WALL] = sf::IntRect({44, 2}, {16, 16});
    miscType_to_TextureAtlasKey[MiscTileType::INVISIBLE_WALL] = "miscTileCustom";


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


bool MiscellaneousTile::isCollidable() const {
    return ::isCollidable(this->type);
}

bool isSoftBlock(MiscTileType type) {
    if (type == MiscTileType::CANDELABRUM || type == MiscTileType::FIREPIT) {
        return true;
    }
    return false;
}

bool isCollidable(MiscTileType type) {
    if (type != MiscTileType::CANDELABRUM && type != MiscTileType::FIREPIT) {
        return true;
    }
    return false;
}

const std::vector<AnimationManager::Frame>& getMiscAnimationFrames(MiscTileType type) {
    using Frame = AnimationManager::Frame;

    static const std::unordered_map<MiscTileType, std::vector<Frame>, MiscTileTypeHash> miscTileTypeToAnimation = {
        { MiscTileType::FIREPIT, {
            { miscType_to_TextureRects.at(MiscTileType::FIREPIT), 0.2f },
            { sf::IntRect({miscType_to_TextureRects.at(MiscTileType::FIREPIT).position.x
                            + 1 + miscType_to_TextureRects.at(MiscTileType::FIREPIT).size.x,
                           miscType_to_TextureRects.at(MiscTileType::FIREPIT).position.y},
                          {miscType_to_TextureRects.at(MiscTileType::FIREPIT).size.x,
                           miscType_to_TextureRects.at(MiscTileType::FIREPIT).size.y}), 0.2f },
        }},
        { MiscTileType::CANDELABRUM, {
            { miscType_to_TextureRects.at(MiscTileType::CANDELABRUM), 0.2f },
            { sf::IntRect({miscType_to_TextureRects.at(MiscTileType::CANDELABRUM).position.x
                            + 1 + miscType_to_TextureRects.at(MiscTileType::CANDELABRUM).size.x,
                           miscType_to_TextureRects.at(MiscTileType::CANDELABRUM).position.y},
                          {miscType_to_TextureRects.at(MiscTileType::CANDELABRUM).size.x,
                           miscType_to_TextureRects.at(MiscTileType::CANDELABRUM).size.y}), 0.2f },
        }},
    };

    static const std::vector<Frame> empty;

    auto it = miscTileTypeToAnimation.find(type);
    return (it != miscTileTypeToAnimation.end()) ? it->second : empty;
}


std::shared_ptr<sf::Sprite> getMiscTileSprite(MiscTileType type) {
    auto iterator = miscType_to_TextureRects.find(type);
    if (iterator == miscType_to_TextureRects.end()) {
        std::cerr << "[ERROR] Texture for miscellaneous type \"" << static_cast<int>(type) << "\" not found." << std::endl;
        return nullptr;
    }

    sf::IntRect textureRect = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(gTextures.at(miscType_to_TextureAtlasKey[type]));
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

void MiscellaneousTile::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect){
    if (this->isDestroyed) return;  // The tile is destroyed --> "doesn't exist" anymore

    sf::FloatRect tileBounds = this->hitboxes[0];   // Misc tiles only have 1 hitbox
    if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) return;  // No hitbox

    if (dynamic_cast<Whip*>(&other)) {
        this->onCollision_Whip(game);
    }
    else if (dynamic_cast<SubWeapon*>(&other) && isSoftBlock(this->type)) {
        this->onCollision_Whip(game);
    }
    else if (dynamic_cast<Player*>(&other)) {
        if (this->isCollidable()) {
            this->onCollision_Player(other, game);
        }
    }
}

void MiscellaneousTile::onCollision_Whip(Game& game){
    if (this->isBreakable) {
        this->isDestroyed = true;
        game.createDropItem(this->dropType, this->sprite->getPosition());
        
        if (this->isCollidable()) {
            game.particleSystem.spawnBreakBlockParticle(this->sprite->getPosition());
        } else {
            game.particleSystem.spawnFireParticle(this->sprite->getPosition());
        }
    }
}

void MiscellaneousTile::onCollision_Player(Entity& other, Game& game) {
    sf::FloatRect *tileBounds = &this->hitboxes[0];   // Misc tiles only have 1 hitbox

    if (this->type != MiscTileType::DROP_TRIGGER) {
        bool hasCollided = false;
        game.computePlayerTileIntersection(hasCollided, *tileBounds);
    }
    else if (Player* player = dynamic_cast<Player*>(&other); player && player->isDucking) {  // It's MiscTileType::DROP_TRIGGER
        auto audio = game.configManager.getAudio();
        gameSoundManager.playSound("secret_treasure", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        
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


void StairTile::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect){

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


// -------------------------- WATER ZONE --------------------------

WaterZone::WaterZone() : Tile() {
    
}


void WaterZone::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect){
    if (game.currentLevel != 1 || game.currentStage != 4) return;

    if (this->collisionedEntities.find(&other) != this->collisionedEntities.end()) {
        return;  // Already collided with this entity
    }
    
    if (dynamic_cast<Player*>(&other)) {
        this->collisionedEntities.insert(&other);
        auto audio = game.configManager.getAudio();
        gameSoundManager.playSound("water_surface_hit", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        game.particleSystem.spawnWaterSplashParticle(intersectionRect.position + intersectionRect.size * 0.5f);
    }
    else if (dynamic_cast<Enemy*>(&other)) {
        this->collisionedEntities.insert(&other);
        auto audio = game.configManager.getAudio();
        gameSoundManager.playSound("water_surface_hit", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        game.particleSystem.spawnWaterSplashParticle(intersectionRect.position + intersectionRect.size * 0.5f);
    }
}

void WaterZone::clearEntity(const Entity& entity) {
    this->collisionedEntities.erase(&entity);
}

void WaterZone::hello() const {
    std::cout << "Soy WaterZone" << std::endl;
}