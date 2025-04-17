#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include "animationManager.h"
#include "entity.h"
#include "item.h"



class Game;
//class Player;


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

// Types of breakable tiles. Only in this scope
enum class BreakableType {
    FIREPIT = 0,
    CANDELABRUM = 1,
    BREAKABLE_WALL_1SQUARE = 2,
    BREAKABLE_WALL_4SQUARES = 3,
    BREAKABLE_WALL_3SQUARES = 4,
};

// Hash function for the breakable tile type
struct BreakableTypeHash
{
    std::size_t operator()(const BreakableType &t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};


// Dicctionary with the texture rectangles of the breakable tiles
extern std::unordered_map<BreakableType, sf::IntRect, BreakableTypeHash> breakableTextureRects;

// Atlas with the textures of all the breakable tiles
extern std::vector<sf::Texture> breakableAtlas;

// Map with the breakable tile type and its corresponding index in the atlas
extern std::unordered_map<BreakableType, int> breakableTypeToAtlasIndex;

class BreakableTile : public TileSprite
{
public:
    BreakableType type;                      // Breakable type
    bool isBreakable = true;        // Enabling the tile to be destroyed
    bool isDestroyed = false;       // If the tile is destroyed
    bool generatesItem = false;     // If the tile generates an item when destroyed
    
    DropType dropType = DropType::NONE;          // If the tile drops an item

    BreakableTile();
    ~BreakableTile() = default;

    //////////////////////////////
    // Entity functions
    //////////////////////////////

    void onCollision(Entity& other, Game& game) override;
    void hello() const override;

    //////////////////////////////
    // Breakable functions
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
    bool hasAnimation(BreakableType type) const;

    // Updates the animation of the tile
    void update(const float& deltaTime);
};

// Function that loads the textures of the breakable tiles
bool loadBreakableTextures();

// Returns the animation frames of a breakable tile based on its type
const std::vector<AnimationManager::Frame>& getBreakableAnimationFrames(BreakableType type);

// Returns the sprite of a breakable tile based on its type
std::shared_ptr<sf::Sprite> getBreakableTileSprite(BreakableType type);

// Returns the instance of a breakable tile based on its type, hitbox, isBreakable and dropType
std::shared_ptr<BreakableTile> getBreakableTile(const BreakableType type, const sf::FloatRect& hitbox,
                                                const bool isBreakable, const DropType dropType);

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


