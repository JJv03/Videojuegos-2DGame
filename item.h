#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

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


class Item {
public:
    Item(ItemType type, const sf::Vector2f& position, const sf::Sprite& sprite);
    
    void update(float deltaTime);

    void draw(sf::RenderWindow& window);
    
    ItemType getType() const;

    const sf::Vector2f& getPosition() const;

    bool isOnGround() const;
    
private:
    ItemType m_type;

    std::shared_ptr<sf::Sprite> m_sprite;

    sf::Vector2f m_position;
    
    bool m_isOnGround;
    
    float m_verticalSpeed;
};


// Loads the textures of the items
bool loadItemTextures();