#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "entity.h"
#include "animationManager.h"


// All the collectable items in the game
enum class ItemType {
    NONE,

    // Subweapons
    DAGGER,
    AXE,
    FIRE_BOMB,  // Holy water
    BOOMERANG,
    STOPWATCH,


    // Collectables
    MORNING_STAR,

    SMALL_HEART,
    LARGE_HEART,

    RED_MONEY_BAG,
    PURPLE_MONEY_BAG,
    WHITE_MONEY_BAG,
    

    // Special collectables
    ROSARY,
    INVISIBILITY_POTION,

    PORK_CHOP,

    DOUBLE_SHOT,
    TRIPLE_SHOT,

    FLASHING_MONEY_BAG,
    CROWN,
    CHEST,
    MOAI,
    
    MAGIC_CRYSTAL,
    
    ONEUP,
};

enum class DropType {       // Types of items that can drop from breakable tiles.
    NONE = 0,               // No item drops
    DEFAULT = 1,            // Default item drop (whip, secondary weapon, etc).
    WEAPON = 2,             
    CROWN = 3,
    FLASHING_MONEY_BAG = 4,
    CHEST = 5,
    MOAI = 6,
    PORK_CHOP = 7,
    DOUBLE_SHOT = 8,
    TRIPLE_SHOT = 9,
};


// Hash function for the items enum to texture
struct ItemTypeHash {
    std::size_t operator()(const ItemType& t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};

// Texture rects for each item type
extern std::unordered_map<ItemType, sf::IntRect, ItemTypeHash> item_To_TextureRect;

// All the items are in the same texture 'atlas'
extern sf::Texture itemAtlas;


class Item : public EntitySprite {
public:
    ItemType m_type;

    bool m_isOnGround;  // True if the item is on the ground
    
    float m_lifeTime;   // Time until the item disappears. 
                        // If it is 0, the item is removed from the game
                        // If it is -1, the item NEVER disappears

    Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
         const float _lifeTime);

    Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
         std::vector<AnimationManager::Frame>& _animationFrames, const float _lifeTime);

    void update(const float& deltaTime);

    void draw(sf::RenderWindow& window);
    
    ItemType getType() const;

    sf::Vector2f getPosition() const;

    void onCollision(Entity& other, Game& game) override;

    void onCollision_SolidTile(Entity& other);

    void hello() const override;

private:
    std::unique_ptr<AnimationManager> m_animationManager{nullptr};
    AnimationManager::Animation m_animation;        // This item's animation
};


// Loads the texture and the texture rectangle of all the items
bool loadItemTextures();

// Returns 'true' if the item is a subweapon. Else 'false'
bool isSubweaponItem(ItemType type);

// Returns 'true' if the item gives score points to Simon. Else 'false'
bool isScoringItem(ItemType type);

// Returns the score of an item. If the item is not a scoring item, returns 0
int getItemScore(ItemType item);

// Returns a new item based on the drop type
std::shared_ptr<Item> getDropItem(DropType dropType, sf::Vector2f position);

// Returns the sprite of an item based on the item type
std::shared_ptr<sf::Sprite> getItemSprite(ItemType type);
