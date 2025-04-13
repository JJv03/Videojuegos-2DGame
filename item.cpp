#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <random>
#include "item.h"
#include "globals.h"
#include "tile.h"
#include "player.h"
#include "game.h"

std::unordered_map<ItemType, std::shared_ptr<sf::Texture>, ItemTypeHash> itemTextures;



Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes):
          m_type(_type), m_isOnGround(false), m_lifeTime(5.0f), EntitySprite(_sprite, _hitboxes) {}


void Item::update(const float& deltaTime) {
    if (m_lifeTime > 0.f) {       // Item can disappear
        m_lifeTime -= deltaTime;
        if (m_lifeTime < 0.f)     // Prevent negative lifetime because it's not a permanent item
            m_lifeTime = 0.f;
    }

    if (!m_isOnGround) {
        sprite->move({0.f, gItemGravity * deltaTime});
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


void Item::onCollision(Entity& other, Game& game) {
    if (dynamic_cast<SolidTile*>(&other)) {
        onCollision_SolidTile(other);
    } else if (dynamic_cast<Player*>(&other)){
        this->m_lifeTime = 0.f;
    }
}

void Item::onCollision_SolidTile(Entity& other){
    sf::FloatRect tileBounds = other.getBounds()[0];

    if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) return;

    sf::FloatRect itemBounds = this->sprite->getGlobalBounds();
    if (const std::optional<sf::FloatRect> intersection = itemBounds.findIntersection(tileBounds))
    {
        const float overlapX = intersection->size.x;
        const float overlapY = intersection->size.y;

        if (overlapX >= overlapY) // Vertical collision
        {
            if ((itemBounds.position.y + itemBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
            { // Bottom collision
                this->sprite->move({0.f, -overlapY});
                itemBounds.position.y -= overlapY;
                this->m_isOnGround = true; // Set item to be on ground
            }
        }
    }
}


bool loadItemTextures() {
    sf::Image image1;
    if (!image1.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    image1.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    auto daggerTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({1, 18}, {16, 16}));
    itemTextures[ItemType::DAGGER] = daggerTexture;

    auto axeTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({18, 18}, {16, 16}));
    itemTextures[ItemType::AXE] = axeTexture;

    auto fireBombTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({35, 18}, {16, 16}));
    itemTextures[ItemType::FIRE_BOMB] = fireBombTexture;

    auto boomerangTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({52, 18}, {16, 16}));
    itemTextures[ItemType::BOOMERANG] = boomerangTexture;

    auto stopwatchTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({103, 18}, {16, 16}));
    itemTextures[ItemType::STOPWATCH] = stopwatchTexture;



    auto morningStarTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({1, 1}, {16, 16}));
    itemTextures[ItemType::MORNING_STAR] = morningStarTexture;

    auto smallHeartTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({18, 1}, {8, 16}));
    itemTextures[ItemType::SMALL_HEART] = smallHeartTexture;

    auto largeHeartTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({29, 6}, {12, 10}));
    itemTextures[ItemType::LARGE_HEART] = largeHeartTexture;

    auto redMoneyBagTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({44, 1}, {16, 16}));
    itemTextures[ItemType::RED_MONEY_BAG] = redMoneyBagTexture;

    auto purpleMoneyBagTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({61, 1}, {16, 16}));
    itemTextures[ItemType::PURPLE_MONEY_BAG] = purpleMoneyBagTexture;

    auto whiteMoneyBagTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({78, 1}, {16, 16}));
    itemTextures[ItemType::WHITE_MONEY_BAG] = whiteMoneyBagTexture;



    auto rosaryTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({35, 35}, {16, 16}));
    itemTextures[ItemType::ROSARY] = rosaryTexture;

    auto invisibilityPotionTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({18, 35}, {16, 16}));
    itemTextures[ItemType::INVISIBILITY_POTION] = invisibilityPotionTexture;
    
    auto porkChopTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({1, 35}, {16, 16}));
    itemTextures[ItemType::PORK_CHOP] = porkChopTexture;



    auto doubleShotTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({120, 18}, {16, 16}));
    itemTextures[ItemType::DOUBLE_SHOT] = doubleShotTexture;

    auto tripleShotTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({137, 18}, {16, 16}));
    itemTextures[ItemType::TRIPLE_SHOT] = tripleShotTexture;



    //auto flashingMoneyBag = std::make_shared<sf::Texture>(image1, false, sf::IntRect({69, 35}, {16, 16}));
    itemTextures[ItemType::FLASHING_MONEY_BAG] = redMoneyBagTexture;
    
    auto crownTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({69, 35}, {16, 16}));
    itemTextures[ItemType::CROWN] = crownTexture;

    auto chestTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({137, 35}, {16, 16}));
    itemTextures[ItemType::CHEST] = chestTexture;

    auto moaiTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({137, 35}, {16, 16}));
    itemTextures[ItemType::MOAI] = moaiTexture;
    

    
    auto oneUpTexture = std::make_shared<sf::Texture>(image1, false, sf::IntRect({52, 35}, {16, 16}));
    itemTextures[ItemType::ONEUP] = oneUpTexture;


    return true;
}


bool isSubweaponItem(ItemType type) {
    if (type == ItemType::DAGGER ||
        type == ItemType::AXE ||
        type == ItemType::FIRE_BOMB ||
        type == ItemType::BOOMERANG ||
        type == ItemType::STOPWATCH) {
        return true;
    }

    return false;
}


bool isScoringItem(ItemType type) {
    if (type == ItemType::RED_MONEY_BAG ||
        type == ItemType::PURPLE_MONEY_BAG ||
        type == ItemType::WHITE_MONEY_BAG ||
        type == ItemType::FLASHING_MONEY_BAG ||
        type == ItemType::CROWN ||
        type == ItemType::CHEST ||
        type == ItemType::MOAI) {
        return true;
    }

    return false;
}


int getItemScore(ItemType item) {
    switch (item) {
        case ItemType::RED_MONEY_BAG:
            return 100;
        case ItemType::PURPLE_MONEY_BAG:
            return 400;
        case ItemType::WHITE_MONEY_BAG:
            return 700;
        case ItemType::FLASHING_MONEY_BAG:
            return 1000;
        case ItemType::CROWN:
            return 2000;
        case ItemType::CHEST:
            return 2000;
        case ItemType::MOAI:
            return 4000;
        default:
            return 0;
    }
}


std::shared_ptr<Item> getDropItem(DropType dropType, sf::Vector2f position) {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    
    ItemType type;

    switch (dropType) {
        case DropType::NONE:
            return nullptr;                 // No item to drop
        
        case DropType::DEFAULT: {
            static const std::vector<ItemType> defaultDropItems = {
                ItemType::RED_MONEY_BAG,
                ItemType::SMALL_HEART,
                ItemType::LARGE_HEART,
                ItemType::MORNING_STAR,
            };
            std::uniform_int_distribution<size_t> dist(0, defaultDropItems.size() - 1);
            type = defaultDropItems[dist(rng)];
            break;
        }
        
        case DropType::WEAPON: {
            static const std::vector<ItemType> weaponDropItems = {
                ItemType::MORNING_STAR,
                ItemType::DAGGER,
                ItemType::STOPWATCH,
                ItemType::ROSARY,
            };
            std::uniform_int_distribution<size_t> dist(0, weaponDropItems.size() - 1);
            type = weaponDropItems[dist(rng)];
            break;
        }

        case DropType::CROWN:
            type = ItemType::CROWN;
            break;

        case DropType::FLASHING_MONEY_BAG:
            type = ItemType::FLASHING_MONEY_BAG;
            break;
        
        case DropType::CHEST:
            type = ItemType::CHEST;
            break;

        case DropType::MOAI:
            type = ItemType::MOAI;
            break;

        case DropType::PORK_CHOP:
            type = ItemType::PORK_CHOP;
            break;
        
        case DropType::DOUBLE_SHOT:
            type = ItemType::DOUBLE_SHOT;
            break;

        case DropType::TRIPLE_SHOT:
            type = ItemType::TRIPLE_SHOT;
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


void Item::hello() const {
    std::cout << "Soy Item" << std::endl;
}
