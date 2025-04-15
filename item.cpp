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

std::unordered_map<ItemType, sf::IntRect, ItemTypeHash> item_To_TextureRect;
sf::Texture itemAtlas;



Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
           const float _lifeTime = 5.0f): EntitySprite(_sprite, _hitboxes), m_type(_type),
           m_isOnGround(false), m_lifeTime(_lifeTime) {}


Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
           std::vector<AnimationManager::Frame>& _animationFrames, const float _lifeTime = 5.0f):
           EntitySprite(_sprite, _hitboxes), m_type(_type), m_isOnGround(false), m_lifeTime(_lifeTime)
{
    m_animationManager = std::make_unique<AnimationManager>(*sprite);

    m_animation.id = notRelevant;
    m_animation.frames = std::move(_animationFrames);
    m_animation.loop = true;

    m_animationManager->playAnimation(m_animation);
}


void Item::update(const float& deltaTime) {
    if (m_lifeTime > 0.f) {       // Item can disappear
        m_lifeTime -= deltaTime;
        if (m_lifeTime < 0.f)     // Prevent negative lifetime because it's not a permanent item
            m_lifeTime = 0.f;
        
        if (m_lifeTime <= 1.f && !m_animationManager->isBlinking()) {
            m_animationManager->setBlinking(true, 0.1f);
        }
    }

    if (!m_isOnGround) {
        sprite->move({0.f, gItemGravity * deltaTime});
        hitboxes[0].position.y += gItemGravity * deltaTime;
    }

    if (m_animationManager != nullptr) {
        m_animationManager->update(deltaTime);
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

    sf::FloatRect& itemBounds = this->hitboxes[0];
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
    if (!itemAtlas.loadFromImage(image1)) return false;
    itemAtlas.setSmooth(false);

    item_To_TextureRect[ItemType::NONE] = sf::IntRect({0, 0}, {0, 0});      // No item, won't draw anything
    
    item_To_TextureRect[ItemType::DAGGER] = sf::IntRect({1, 18}, {16, 16});
    item_To_TextureRect[ItemType::AXE] = sf::IntRect({18, 18}, {16, 16});
    item_To_TextureRect[ItemType::FIRE_BOMB] = sf::IntRect({35, 18}, {16, 16});
    item_To_TextureRect[ItemType::BOOMERANG] = sf::IntRect({52, 18}, {16, 16});
    item_To_TextureRect[ItemType::STOPWATCH] = sf::IntRect({103, 18}, {16, 16});

    item_To_TextureRect[ItemType::MORNING_STAR] = sf::IntRect({1, 1}, {16, 16});
    item_To_TextureRect[ItemType::SMALL_HEART] = sf::IntRect({18, 1}, {8, 16});
    item_To_TextureRect[ItemType::LARGE_HEART] = sf::IntRect({29, 6}, {12, 10});
    item_To_TextureRect[ItemType::RED_MONEY_BAG] = sf::IntRect({44, 1}, {16, 16});
    item_To_TextureRect[ItemType::PURPLE_MONEY_BAG] = sf::IntRect({61, 1}, {16, 16});
    item_To_TextureRect[ItemType::WHITE_MONEY_BAG] = sf::IntRect({78, 1}, {16, 16});

    item_To_TextureRect[ItemType::ROSARY] = sf::IntRect({35, 35}, {16, 16});
    item_To_TextureRect[ItemType::INVISIBILITY_POTION] = sf::IntRect({18, 35}, {16, 16});
    item_To_TextureRect[ItemType::PORK_CHOP] = sf::IntRect({1, 35}, {16, 16});

    item_To_TextureRect[ItemType::DOUBLE_SHOT] = sf::IntRect({120, 18}, {16, 16});
    item_To_TextureRect[ItemType::TRIPLE_SHOT] = sf::IntRect({137, 18}, {16, 16});

    item_To_TextureRect[ItemType::FLASHING_MONEY_BAG] = item_To_TextureRect[ItemType::RED_MONEY_BAG];
    item_To_TextureRect[ItemType::CROWN] = sf::IntRect({69, 35}, {16, 16});
    item_To_TextureRect[ItemType::CHEST] = sf::IntRect({137, 35}, {16, 16});
    item_To_TextureRect[ItemType::MOAI] = sf::IntRect({137, 35}, {16, 16});    

    item_To_TextureRect[ItemType::MAGIC_CRYSTAL] = sf::IntRect({95, 1}, {16, 16});   

    item_To_TextureRect[ItemType::ONEUP] = sf::IntRect({52, 35}, {16, 16});


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


const std::vector<AnimationManager::Frame>& getItemAnimationFrames(ItemType type) {
    using Frame = AnimationManager::Frame;

    static const std::unordered_map<ItemType, std::vector<Frame>> itemTypeToAnimation = {
        { ItemType::DAGGER, {
            { item_To_TextureRect.at(ItemType::DAGGER), gLevelTimeLimit },
        }},
        { ItemType::AXE, {
            { item_To_TextureRect.at(ItemType::AXE), gLevelTimeLimit },
        }},
        { ItemType::FIRE_BOMB, {
            { item_To_TextureRect.at(ItemType::FIRE_BOMB), gLevelTimeLimit },
        }},
        { ItemType::BOOMERANG, {
            { item_To_TextureRect.at(ItemType::BOOMERANG), gLevelTimeLimit },
        }},
        { ItemType::STOPWATCH, {
            { item_To_TextureRect.at(ItemType::STOPWATCH), gLevelTimeLimit },
        }},
        { ItemType::MORNING_STAR, {
            { item_To_TextureRect.at(ItemType::MORNING_STAR), gLevelTimeLimit },
        }},
        { ItemType::SMALL_HEART, {
            { item_To_TextureRect.at(ItemType::SMALL_HEART), gLevelTimeLimit },
        }},
        { ItemType::LARGE_HEART, {
            { item_To_TextureRect.at(ItemType::LARGE_HEART), gLevelTimeLimit },
        }},
        { ItemType::RED_MONEY_BAG, {
            { item_To_TextureRect.at(ItemType::RED_MONEY_BAG), gLevelTimeLimit },
        }},
        { ItemType::PURPLE_MONEY_BAG, {
            { item_To_TextureRect.at(ItemType::PURPLE_MONEY_BAG), gLevelTimeLimit },
        }},
        { ItemType::WHITE_MONEY_BAG, {
            { item_To_TextureRect.at(ItemType::WHITE_MONEY_BAG), gLevelTimeLimit },
        }},
        { ItemType::ROSARY, {
            { item_To_TextureRect.at(ItemType::ROSARY), gLevelTimeLimit },
        }},
        { ItemType::INVISIBILITY_POTION, {
            { item_To_TextureRect.at(ItemType::INVISIBILITY_POTION), gLevelTimeLimit },
        }},
        { ItemType::PORK_CHOP, {
            { item_To_TextureRect.at(ItemType::PORK_CHOP), gLevelTimeLimit },
        }},
        { ItemType::DOUBLE_SHOT, {
            { item_To_TextureRect.at(ItemType::DOUBLE_SHOT), gLevelTimeLimit },
        }},
        { ItemType::TRIPLE_SHOT, {
            { item_To_TextureRect.at(ItemType::TRIPLE_SHOT), gLevelTimeLimit },
        }},
        { ItemType::FLASHING_MONEY_BAG, {
            { item_To_TextureRect.at(ItemType::RED_MONEY_BAG),    0.1f },
            { item_To_TextureRect.at(ItemType::PURPLE_MONEY_BAG), 0.1f },
            { item_To_TextureRect.at(ItemType::WHITE_MONEY_BAG),  0.1f },
        }},
        { ItemType::CROWN, {
            { item_To_TextureRect.at(ItemType::CROWN), gLevelTimeLimit },
        }},
        { ItemType::CHEST, {
            { item_To_TextureRect.at(ItemType::CHEST), gLevelTimeLimit },
        }},
        { ItemType::MOAI, {
            { item_To_TextureRect.at(ItemType::MOAI), gLevelTimeLimit },
        }},
        { ItemType::MAGIC_CRYSTAL, {
            { item_To_TextureRect.at(ItemType::MAGIC_CRYSTAL), gLevelTimeLimit },
        }},
        { ItemType::ONEUP, {
            { item_To_TextureRect.at(ItemType::ONEUP), gLevelTimeLimit },
        }},
    };

    static const std::vector<Frame> empty;

    auto it = itemTypeToAnimation.find(type);
    return (it != itemTypeToAnimation.end()) ? it->second : empty;
}


std::shared_ptr<Item> getDropItem(DropType dropType, sf::Vector2f position) {
    static std::random_device rd;       // we only want 1 instance of random_device
    static std::mt19937 rng(rd());
    
    ItemType type;
    float lifeTime = 5.f;

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
            if (type == ItemType::MORNING_STAR) lifeTime = -1.f; // Permanent item
            break;
        }
        
        case DropType::WEAPON: {
            static const std::vector<ItemType> weaponDropItems = {
                ItemType::MORNING_STAR,
                ItemType::DAGGER,
                ItemType::AXE,
                ItemType::FIRE_BOMB,
                ItemType::BOOMERANG,
                ItemType::STOPWATCH,
                ItemType::ROSARY,
            };
            std::uniform_int_distribution<size_t> dist(0, weaponDropItems.size() - 1);
            type = weaponDropItems[dist(rng)];
            if (type == ItemType::MORNING_STAR) lifeTime = -1.f; // Permanent item
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
            std::cerr << "[ERROR] Unknown drop type: " << static_cast<int>(dropType) << std::endl;
            return nullptr;
    }

    std::shared_ptr<sf::Sprite> sprite = getItemSprite(type);
    sprite->setPosition(position);

    std::vector<sf::FloatRect> hitboxes{ sprite->getGlobalBounds() };

    std::vector<AnimationManager::Frame> animationFrames = getItemAnimationFrames(type);
    if (animationFrames.size() > 0)
        return std::make_shared<Item>(type, sprite, hitboxes, animationFrames, lifeTime);
    return std::make_shared<Item>(type, sprite, hitboxes, lifeTime);
}


std::shared_ptr<sf::Sprite> getItemSprite(ItemType type) {
    auto iterator = item_To_TextureRect.find(type);
    if (iterator == item_To_TextureRect.end()) {
        std::cerr << "[ERROR] Texture for item type \"" << static_cast<int>(type) << "\" not found." << std::endl;
        return nullptr;
    }

    sf::IntRect textureRect = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(itemAtlas);
    sprite->setTextureRect(textureRect);

    return sprite;
}


void Item::hello() const {
    std::cout << "Soy Item" << std::endl;
}
