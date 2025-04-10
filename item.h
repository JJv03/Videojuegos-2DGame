#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "entity.h"

// All the collectable items in the game
enum class ItemType {
    MORNING_STAR,
    SMALL_HEART,
    BIG_HEART,
    RED_MONEY_BAG,
    PURPLE_MONEY_BAG,
    WHITE_MONEY_BAG,
    ROSARY,
    INVISIBILITY_POTION,
    PORK_CHOP,
    DOUBLE_SHOT,
    TRIPLE_SHOT,
};

enum class DropType {       // Types of items that can drop from breakable tiles.
    NONE = 0,               // No item drops
    DEFAULT = 1,            // Default item drop (whip, secondary weapon, etc). Should be handled in game logic
    MORNING_STAR = 2,
    CROWN = 3,
    MEAT = 4,
};


class Item : public EntitySprite {
public:
    ItemType m_type;

    bool m_isOnGround;  // True if the item is on the ground
    
    float m_lifeTime;   // Time until the item disappears. 
                        // If it is 0, the item is removed from the game
                        // If it is -1, the item NEVER disappears

    Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void onCollision(Entity& other) override;
    
    void update(const float& deltaTime);

    void draw(sf::RenderWindow& window);
    
    ItemType getType() const;

    sf::Vector2f getPosition() const;
};


// Loads the textures of the items
bool loadItemTextures();

// Returns a new item based on the drop type
std::shared_ptr<Item> createDropItem(DropType dropType, sf::Vector2f position);