#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <iostream>
#include <memory>
#include "item.h"
#include "entity.h"

constexpr float GRAVITY = 100.f;

struct ItemTypeHash {       // Hash function for the items enum to texture
    std::size_t operator()(const ItemType& t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};

std::unordered_map<ItemType, std::shared_ptr<sf::Texture>, ItemTypeHash> itemTextures;



Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes):
          m_type(_type), m_isOnGround(false), m_lifeTime(5.0f), EntitySprite(_sprite, _hitboxes) {}

void Item::onCollision(Entity& other) {

}

void Item::update(const float& deltaTime) {
    if (m_lifeTime > 0.f) {       // Item can disappear
        m_lifeTime -= deltaTime;
        if (m_lifeTime < 0.f)     // Prevent negative lifetime because it's not a permanent item
            m_lifeTime = 0.f;
    }

    if (!m_isOnGround) {
        sprite->move({0.f, GRAVITY * deltaTime});
    }
}

void Item::draw(sf::RenderWindow& window) {
    window.draw(*sprite);
}

ItemType Item::getType() const {
    return m_type;
}

sf::Vector2f Item::getPosition() const {
    return sprite->getPosition();
}


bool loadItemTextures() {
    sf::Image image;
    if (!image.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    image.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    auto morningStarTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({1, 1}, {16, 16}));
    itemTextures[ItemType::MORNING_STAR] = morningStarTexture;

    auto smallHeartTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({18, 1}, {8, 8}));
    itemTextures[ItemType::SMALL_HEART] = smallHeartTexture;

    auto bigHeartTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({29, 6}, {12, 10}));
    itemTextures[ItemType::BIG_HEART] = bigHeartTexture;

    auto redMoneyBagTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({44, 1}, {16, 16}));
    itemTextures[ItemType::RED_MONEY_BAG] = redMoneyBagTexture;

    auto purpleMoneyBagTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({61, 1}, {16, 16}));
    itemTextures[ItemType::PURPLE_MONEY_BAG] = purpleMoneyBagTexture;

    auto whiteMoneyBagTexture = std::make_shared<sf::Texture>(image, false, sf::IntRect({78, 1}, {16, 16}));
    itemTextures[ItemType::WHITE_MONEY_BAG] = whiteMoneyBagTexture;

    return true;
}

std::shared_ptr<Item> createDropItem(DropType dropType, sf::Vector2f position) {
    ItemType type;

    switch (dropType) {
        case DropType::NONE:
            return nullptr;                 // No item to drop
        case DropType::DEFAULT:
            type = ItemType::RED_MONEY_BAG; // Default item drop
            break;
        case DropType::MORNING_STAR:
            type = ItemType::MORNING_STAR;
            break;
        default:
            std::cerr << "Unknown drop type: " << static_cast<int>(dropType) << std::endl;
            return nullptr;
    }

    auto iterator = itemTextures.find(type);
    if (iterator == itemTextures.end()) {
        std::cerr << "Texture for item type not found.\n";
        return nullptr;
    }

    auto texture = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(*texture);
    sprite->setPosition(position);

    std::vector<sf::FloatRect> hitboxes{ sprite->getGlobalBounds() };
    return std::make_shared<Item>(type, sprite, hitboxes);
}