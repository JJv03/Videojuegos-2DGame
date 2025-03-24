#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "item.h"

constexpr float GRAVITY = 980.f;


// Hash function for the breakable tile type
struct ItemTypeHash {
    std::size_t operator()(const ItemType& t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};

// Dicctionary with the textures of the breakable tiles
std::unordered_map<ItemType, std::shared_ptr<sf::Texture>, ItemTypeHash> itemTextures;



bool loadItemTextures() {
    sf::Image image;
    if (!image.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    image.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    auto smallHeartTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({18, 1}, {8, 8}));
    itemTextures[ItemType::SMALL_HEART] = smallHeartTexture;

    auto purpleMoneyBagTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({61, 1}, {16, 16}));
    itemTextures[ItemType::PURPLE_MONEY_BAG] = purpleMoneyBagTexture;
}


Item::Item(ItemType type, const sf::Vector2f& position, const sf::Sprite& sprite):
        m_type(type), m_position(position), m_isOnGround(false), m_verticalSpeed(0.f), m_sprite(std::make_shared<sf::Sprite>(sprite))
{
    m_sprite->setPosition(m_position);
}

void Item::update(float deltaTime) {
    if (!m_isOnGround) {
        m_verticalSpeed += GRAVITY * deltaTime * 1.2f;
        m_position.y += m_verticalSpeed * deltaTime;
        m_sprite->setPosition(m_position);
    }
}

void Item::draw(sf::RenderWindow& window) {
    window.draw(*m_sprite);
}

ItemType Item::getType() const {
    return m_type;
}

const sf::Vector2f& Item::getPosition() const {
    return m_position;
}

bool Item::isOnGround() const {
    return m_isOnGround;
}