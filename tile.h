#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include "animationManager.h"
#include "entity.h"
#include "item.h"



class Game;


class Tile : public Entity
{
public:
    Tile();
    ~Tile() = default;
    void hello() const override;
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
    SolidTile();
    ~SolidTile() = default;

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

// -------------------------------------------------

class DoorTile : public Tile {
public:
    enum class Type{   // Types of door tiles. Only in this scope
        CASTLE_ENTRANCE = 0,
        DOOR = 1,
        STAIRS = 2,
    };

    DoorTile();
    ~DoorTile() = default;

    int doorId;
    Type type;                      // Door type
    sf::Vector2f playerAparition;           // Hitbox of the tile. In global coord.

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

// -------------------------------------------------

// Types of miscellaneous tiles. Only in this scope
enum class MiscTileType {
    FIREPIT = 0,
    CANDELABRUM = 1,
    WALL_1SQUARE = 2,
    WALL_4SQUARES = 3,
    WALL_3SQUARES = 4,
    DROP_TRIGGER = 5,       // Zone in which a player collision triggers an item drop
    
    WALL_1SQUARE_BROKE = 6,     // Slight modification that has a crack
    WALL_4SQUARES_BROKE = 7,    // Slight modification that has a crack
    WALL_3SQUARES_BROKE = 8,    // Slight modification that has a crack
};

// Hash function for the miscellaneous tile type
struct MiscTileTypeHash
{
    std::size_t operator()(const MiscTileType &t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};


// Dicctionary with the texture rectangles of the miscellaneous tiles
extern std::unordered_map<MiscTileType, sf::IntRect, MiscTileTypeHash> miscType_to_TextureRects;

// Map with the miscellaneous tile type and its corresponding key in the texture atlas
extern std::unordered_map<MiscTileType, std::string> miscType_to_TextureAtlasKey;

class MiscellaneousTile : public TileSprite
{
public:
    MiscTileType type;             // Misc type
    bool isBreakable = true;        // Enabling the tile to be destroyed
    bool isDestroyed = false;       // If the tile is destroyed
    sf::Vector2i dropItem_position; // Position of the item that drops from the tile
    
    DropType dropType = DropType::NONE;          // If the tile drops an item

    MiscellaneousTile();
    ~MiscellaneousTile() = default;

    //////////////////////////////
    // Entity functions
    //////////////////////////////

    void onCollision(Entity& other, Game& game) override;
    void hello() const override;

    //////////////////////////////
    // Misc functions
    //////////////////////////////

    // Returns whether this tile is collidable or not
    bool isCollidable() const;

    // Collision functions
    void onCollision_Whip(Game& game);
    void onCollision_Player(Entity& other, Game& game);

public:
    std::unique_ptr<AnimationManager> m_animationManager{nullptr};
    AnimationManager::Animation m_animation;        // This tiles's animation
    
    // Returns whether this tile has an animation or not
    bool hasAnimation(MiscTileType type) const;

    // Updates the animation of the tile
    void update(const float& deltaTime);
};

// Function that loads the textures of the miscellaneous tiles
bool loadMiscTextures();

// Returns 'true' if the tile is a soft-block kind of tile. It just so happens that these
// tiles are also the ones that the player can't collide with.
// E.g: Candelabrum, Firepit
bool isSoftBlock(MiscTileType type);

// Returns 'true' if the player can collide with the 'type' tile
bool isCollidable(MiscTileType type);

// Returns the animation frames of a miscellaneous tile based on its type
const std::vector<AnimationManager::Frame>& getMiscAnimationFrames(MiscTileType type);

// Returns the sprite of a miscellaneous tile based on its type
std::shared_ptr<sf::Sprite> getMiscTileSprite(MiscTileType type);

// Returns the instance of a miscellaneous tile based on its type, hitbox, isBreakable and dropType
std::shared_ptr<MiscellaneousTile> getMiscTile(const MiscTileType type, const sf::FloatRect& hitbox,
                                                const bool isBreakable, const DropType dropType,
                                                const int dropItem_x, const int dropItem_y);

// -------------------------------------------------

class StairTile : public Tile
{
public:
    enum class Type {   // Types of stair tiles. Only in this scope
        NONE,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
    };

    Type type;                      // Stair type
    
    StairTile();
    ~StairTile() = default;

    // Entity functions
    void onCollision(Entity& other, Game& game) override;
    void hello() const override;
};

bool getStairType(int level, int tileNumber, StairTile::Type& stairType);


