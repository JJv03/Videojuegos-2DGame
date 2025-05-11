#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <random>
#include <cmath>
#include "item.h"
#include "globals.h"
#include "tile.h"
#include "player.h"
#include "game.h"


const float OSCILLATION_AMPLITUDE = 30.f;   // Amplitude of the sinusoidal movement
const float OSCILLATION_SPEED = 4.f;        // Speed of the sinusoidal movement

static std::random_device rd;       // we only want 1 instance of random_device
static std::mt19937 rng(rd());

std::unordered_map<ItemType, sf::IntRect, ItemTypeHash> item_To_TextureRect;




Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
           const float _lifeTime, const float _spawnDelay): EntitySprite(_sprite, _hitboxes),
           m_type(_type), m_lifeTime(_lifeTime), m_spawnDelay(_spawnDelay), m_elapsedSpawnDelay(0.f),
           m_sinusoidalTotalTime(0.f) {}


Item::Item(ItemType _type, std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
           std::vector<AnimationManager::Frame>& _animationFrames, const float _lifeTime,
           const float _spawnDelay): EntitySprite(_sprite, _hitboxes), m_type(_type),
           m_lifeTime(_lifeTime), m_spawnDelay(_spawnDelay), m_elapsedSpawnDelay(0.f),
           m_sinusoidalTotalTime(0.f)
{
    m_animationManager = std::make_unique<AnimationManager>(*sprite);

    m_animation.id = notRelevant;
    m_animation.frames = std::move(_animationFrames);
    m_animation.loop = true;

    m_animationManager->playAnimation(m_animation);
}


void Item::update(const float deltaTime) {
    //==================== SPAWN DELAY MANAGEMENT ====================

    if (m_elapsedSpawnDelay < m_spawnDelay) {
        m_elapsedSpawnDelay += deltaTime;
        if (m_elapsedSpawnDelay > m_spawnDelay)
            m_elapsedSpawnDelay = m_spawnDelay;
    }

    const bool isSpawning = m_elapsedSpawnDelay < m_spawnDelay;
    const bool isMagicCrystal = (m_type == ItemType::MAGIC_CRYSTAL);


    // MAGIC_CRYSTAL has a special behavior before spawning: doesn't fall and it's blinking
    if (isMagicCrystal && isSpawning) {
        if (!m_animationManager->isBlinking())
            m_animationManager->setBlinking(true, 0.05f);

        m_animationManager->update(deltaTime);
        return;
    }
    else if (isMagicCrystal && !isSpawning) {   // Stop blinking once it's spawned
        if (m_animationManager->isBlinking())
            m_animationManager->setBlinking(false, 0.05f);
    }


    // ============================ AFTER SPAWN ===================================

    if (m_lifeTime > 0.f) {       // Item can disappear
        m_lifeTime -= deltaTime;
        if (m_lifeTime < 0.f)     // Prevent negative lifetime because it's not a permanent item
            m_lifeTime = 0.f;
        
        if (m_lifeTime <= 1.f && !m_animationManager->isBlinking()) {
            m_animationManager->setBlinking(true, 0.1f);
        }
    }

    // Falling down logic
    if (!isOnGround) {
        float moveX = 0.f;
        float moveY = gItemGravity * deltaTime;

        if (m_type == ItemType::SMALL_HEART) {      // Special: has sinusoidal movement
            m_sinusoidalTotalTime += deltaTime;
            float offsetX = OSCILLATION_AMPLITUDE * std::sin(OSCILLATION_SPEED * m_sinusoidalTotalTime);
            moveX = offsetX * deltaTime;
            moveY /= 3.f;
        }

        sprite->move({moveX, moveY});
        hitboxes[0].position.x += moveX;
        hitboxes[0].position.y += moveY;
    }

    if (m_animationManager != nullptr) {
        m_animationManager->update(deltaTime);
    }

    //==================== SECRET ITEM TEXTURE GROWTH ====================
    if (isSecretItem(m_type) && isSpawning) {
        sf::IntRect texRect = sprite->getTextureRect();
        int newHeight = (m_elapsedSpawnDelay / m_spawnDelay) * item_To_TextureRect[m_type].size.y;
        texRect.size.y = newHeight + 1;
        sprite->setTextureRect(texRect);
        for (auto& hitbox : hitboxes) {
            hitbox.size.y = newHeight;
        }
    }
}

void Item::draw(sf::RenderWindow& window) {
    if (m_spawnDelay == 0.f || m_elapsedSpawnDelay != 0.f) window.draw(*sprite);
}

ItemType Item::getType() const {
    return m_type;
}

sf::Vector2f Item::getPosition() const {
    return sprite->getPosition();
}


void Item::onCollision(Entity& other, Game& game, const sf::FloatRect& intersectionRect) {
    if (dynamic_cast<SolidTile*>(&other)) {
        onCollision_SolidTile(other);
    } else if (dynamic_cast<Player*>(&other)){
        if ((m_type == ItemType::MAGIC_CRYSTAL) && (m_elapsedSpawnDelay < m_spawnDelay)) {   // Special case
            return;
        }

        this->m_lifeTime = 0.f;     // Remove item from the game
        
        if (this->m_type == ItemType::MAGIC_CRYSTAL) {
            gTriggerEndLvlScoreAnimation = true;
        }

        int score = getItemScore(this->m_type);
        if (score == 0)
            return; // No score for this item

        sf::Vector2f offset(sprite->getGlobalBounds().size.x, 0.0f);
        sf::Vector2f scorePosition = this->getPosition() + offset;
        game.particleSystem.spawnPointsParticle(scorePosition, score);
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
                this->isOnGround = true; // Set item to be on ground
            }
        }
    }
}


bool loadItemTextures() {
    sf::Image image1;
    if (!image1.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    image1.createMaskFromColor(gColorKeyGrey);
    image1.createMaskFromColor(gColorKeyGreen);

    sf::Texture tex1;
    if (!tex1.loadFromImage(image1)) return false;
    tex1.setSmooth(false);
    gTextures["items_1"] = std::move(tex1);

    item_To_TextureRect[ItemType::NONE] = sf::IntRect({0, 0}, {0, 0});      // No item, won't draw anything
    
    item_To_TextureRect[ItemType::DAGGER] = sf::IntRect({1, 18}, {16, 16});
    item_To_TextureRect[ItemType::AXE] = sf::IntRect({18, 18}, {16, 16});
    item_To_TextureRect[ItemType::FIRE_BOMB] = sf::IntRect({35, 18}, {16, 16});
    item_To_TextureRect[ItemType::BOOMERANG] = sf::IntRect({69, 18}, {16, 16});
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
    item_To_TextureRect[ItemType::DOUBLE_SHOT_GUI] = sf::IntRect({129, 1}, {16, 16});
    item_To_TextureRect[ItemType::TRIPLE_SHOT] = sf::IntRect({137, 18}, {16, 16});
    item_To_TextureRect[ItemType::TRIPLE_SHOT_GUI] = sf::IntRect({154, 18}, {16, 16});

    item_To_TextureRect[ItemType::FLASHING_MONEY_BAG] = item_To_TextureRect[ItemType::RED_MONEY_BAG];
    item_To_TextureRect[ItemType::CROWN] = sf::IntRect({69, 35}, {16, 16});
    item_To_TextureRect[ItemType::CHEST] = sf::IntRect({137, 35}, {16, 16});
    item_To_TextureRect[ItemType::MOAI] = sf::IntRect({137, 35}, {16, 16});    

    item_To_TextureRect[ItemType::MAGIC_CRYSTAL] = sf::IntRect({95, 1}, {16, 16});
    item_To_TextureRect[ItemType::MAGIC_CRYSTAL_ACTIVE] = sf::IntRect({112, 1}, {16, 16});   

    item_To_TextureRect[ItemType::ONEUP] = sf::IntRect({52, 35}, {16, 16});


    return true;
}


bool isSubweaponItem(const ItemType type) {
    if (type == ItemType::DAGGER ||
        type == ItemType::AXE ||
        type == ItemType::FIRE_BOMB ||
        type == ItemType::BOOMERANG ||
        type == ItemType::STOPWATCH) {
        return true;
    }

    return false;
}


bool isSecretItem(const ItemType type) {
    if (type == ItemType::MOAI ||
        type == ItemType::FLASHING_MONEY_BAG ||
        type == ItemType::ONEUP ||
        type == ItemType::CROWN) {
        return true;
    }

    return false;
}


bool isScoringItem(const ItemType type) {
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


int getItemScore(const ItemType item) {
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
            { item_To_TextureRect.at(ItemType::MAGIC_CRYSTAL), 0.1f },
            { item_To_TextureRect.at(ItemType::MAGIC_CRYSTAL_ACTIVE), 0.1f },
        }},
        { ItemType::ONEUP, {
            { item_To_TextureRect.at(ItemType::ONEUP), gLevelTimeLimit },
        }},
    };

    static const std::vector<Frame> empty;

    auto it = itemTypeToAnimation.find(type);
    return (it != itemTypeToAnimation.end()) ? it->second : empty;
}


ItemType chooseWeightedItem(const std::vector<std::pair<ItemType, float>>& weightedItems,
                            std::uniform_real_distribution<float>& uniformZeroToOne) {
    float myProbability = uniformZeroToOne(rng);
    
    float cumulativeProb = 0.f;
    for (const auto& [item, weight] : weightedItems) {
        cumulativeProb += weight;
        if (myProbability <= cumulativeProb) {
            return item;
        }
    }

    std::cerr << "[ERROR] Weighted item selection failed. Cumulative: " << cumulativeProb << std::endl;
    return weightedItems.back().first;  // In case the total probability is less than 1.0
}


std::shared_ptr<Item> getDropItem(const DropType dropType, sf::Vector2f position, bool canDropWhip,
                                  const ItemType subWeaponType, const bool hasDoubleShot) {
    std::uniform_real_distribution<float> uniformZeroToOne(0.f, 1.f);    // [0, 1] range

    static const std::vector<std::pair<ItemType, float>> defaultDrops = {
        {ItemType::RED_MONEY_BAG, 0.1f},
        {ItemType::SMALL_HEART, 0.6f},
        {ItemType::LARGE_HEART, 0.3},
    };

    static const std::vector<ItemType> defaultSpecialItems = {
        ItemType::STOPWATCH,
        ItemType::ROSARY,
    };

    static const std::vector<ItemType> weaponDrop = {
        ItemType::DAGGER,
        ItemType::AXE,
        ItemType::FIRE_BOMB,
        ItemType::BOOMERANG,
        ItemType::STOPWATCH,
        ItemType::ROSARY,
    };

    static const std::vector<std::pair<ItemType, float>> firepitWeapons = {
        {ItemType::DAGGER, 0.34f },
        {ItemType::FIRE_BOMB, 0.33f },
        {ItemType::BOOMERANG, 0.33f },
    };

    static const std::vector<std::pair<ItemType, float>> axeOrDefaultDropItems = {
        { ItemType::AXE, 0.7f },
        { ItemType::BOOMERANG, 0.15f },
        { ItemType::DAGGER, 0.15f },
    };
    
    const float defaultWhipProb = 0.9f;     // Probability of dropping whip upgrade
    const float defaultVsWeaponProb = 0.95f;    // Probability of dropping a default item
    const float weaponWhipProb = 0.5f;     // Probability of dropping whip upgrade when dropping a weapon

    const float enemyDropProb = 0.25f;      // Probability of dropping an item from enemies (default probability)
    const float enemySpecialDropProb = 0.25f;   // Probability of dropping a special item from enemies


    ItemType type;
    float lifeTime = 5.f;       // Lifetime of the item (in seconds). If "-1", item doesn't dispawn
    float spawnDelay = 0.f;     // Delay before the item is spawned

    switch (dropType) {
        case DropType::NONE:
            return nullptr;                 // No item to drop
        
        case DropType::DEBUG:
            type = ItemType::FIRE_BOMB;
            break;
        
        case DropType::DEFAULT: {   // Drops whip or an item from 'defaultDrop" pool
            if (canDropWhip && uniformZeroToOne(rng) < defaultWhipProb) {
                type = ItemType::MORNING_STAR;
                break;
            }
            type = chooseWeightedItem(defaultDrops, uniformZeroToOne);
            break;
        }
        
        case DropType::ALL_WEAPONS: {   // drops whip or a weapon from "weaponDrop" pool
            if (canDropWhip && uniformZeroToOne(rng) < defaultWhipProb) {
                type = ItemType::MORNING_STAR;
                break;
            }
            std::uniform_int_distribution<size_t> dist(0, weaponDrop.size() - 1);
            type = weaponDrop[dist(rng)];
            break;
        }

        case DropType::FIREPIT_WEAPON: {    // Drops a weapon from "firepitWeapons" pool
            type = chooseWeightedItem(firepitWeapons, uniformZeroToOne);
            break;
        }

        case DropType::AXE_OR_OTHERS: {   // Drops whip or an item from "axeOrDefaultDropItems" pool
            if (canDropWhip && uniformZeroToOne(rng) < defaultWhipProb) {
                type = ItemType::MORNING_STAR;
                break;
            }
            type = chooseWeightedItem(axeOrDefaultDropItems, uniformZeroToOne);
            break;
        }

        case DropType::DEFAULT_OR_WEAPON: {     // Drops whip or an item from "defaultDrops" or "weaponDrop" pools
            if (uniformZeroToOne(rng) < defaultVsWeaponProb) {      // Drop a default item
                type = chooseWeightedItem(defaultDrops, uniformZeroToOne);
            }
            else {      // Drop a weapon
                if (canDropWhip && uniformZeroToOne(rng) < weaponWhipProb) {
                    type = ItemType::MORNING_STAR;
                }
                else {
                    std::uniform_int_distribution<size_t> dist(0, weaponDrop.size() - 1);
                    type = weaponDrop[dist(rng)];
                }
            }
            break;
        }

        case DropType::DEFAULT_ENEMIES: {
            float prob = uniformZeroToOne(rng);
            if (prob < enemyDropProb) {   // Drop an item
                if (canDropWhip && uniformZeroToOne(rng) < weaponWhipProb) {   // Drop a whip
                    type = ItemType::MORNING_STAR;
                }
                else {
                    if (uniformZeroToOne(rng) < enemySpecialDropProb) {   // Drop a special item
                        std::uniform_int_distribution<size_t> dist(0, defaultSpecialItems.size() - 1);
                        type = defaultSpecialItems[dist(rng)];
                    }
                    else {  // Drop a default item
                        type = chooseWeightedItem(defaultDrops, uniformZeroToOne);
                    }
                }
                break;
            }
            else {  // No item to drop
                return nullptr;
            }
        }

        case DropType::INVISIBILITY_POTION:
            type = ItemType::INVISIBILITY_POTION;
            break;

        case DropType::PURPLE_MONEY_BAG:
            type = ItemType::PURPLE_MONEY_BAG;
            break;
        
        case DropType::WHITE_MONEY_BAG:
            type = ItemType::WHITE_MONEY_BAG;
            break;

        case DropType::FLASHING_MONEY_BAG:
            type = ItemType::FLASHING_MONEY_BAG;
            lifeTime = 10.f;
            spawnDelay = 1.5f;  
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
        
        case DropType::MAGIC_CRYSTAL:
            type = ItemType::MAGIC_CRYSTAL;
            lifeTime = -1.f;
            spawnDelay = 2.f;
            break;

        case DropType::FIRE_BOMB:
            type = ItemType::FIRE_BOMB;
            break;
        
        case DropType::AXE:
            type = ItemType::AXE;
            break;
        
        case DropType::BOOMERANG:
            type = ItemType::BOOMERANG;
            break;
        
        case DropType::STOPWATCH:
            type = ItemType::STOPWATCH;
            break;

        case DropType::DAGGER:
            type = ItemType::DAGGER;
            break;

        case DropType::ONE_UP:
            type = ItemType::ONEUP;
            lifeTime = 10.f;
            spawnDelay = 1.5f;
            break;

        default:
            std::cerr << "[ERROR] Unknown drop type: " << static_cast<int>(dropType) << std::endl;
            return nullptr;
    }

    
    if (subWeaponType == type) {    // Avoid dropping the same item as the one the player has
        type = ItemType::SMALL_HEART;
    }

    if (hasDoubleShot && type == ItemType::DOUBLE_SHOT) {  // Avoid dropping double shot if the player has it
        type = ItemType::TRIPLE_SHOT;
    }

    std::shared_ptr<sf::Sprite> sprite = getItemSprite(type);
    sprite->setPosition(position);
    std::vector<sf::FloatRect> hitboxes{ sprite->getGlobalBounds() };
    std::vector<AnimationManager::Frame> animationFrames = getItemAnimationFrames(type);
    
    return std::make_shared<Item>(type, sprite, hitboxes, animationFrames, lifeTime, spawnDelay);
}


std::shared_ptr<sf::Sprite> getItemSprite(ItemType type) {
    auto iterator = item_To_TextureRect.find(type);
    if (iterator == item_To_TextureRect.end()) {
        std::cerr << "[ERROR] Texture for item type \"" << static_cast<int>(type) << "\" not found." << std::endl;
        return nullptr;
    }

    sf::IntRect textureRect = iterator->second;
    std::shared_ptr<sf::Sprite> sprite = std::make_shared<sf::Sprite>(gTextures.at("items_1"));
    sprite->setTextureRect(textureRect);

    return sprite;
}


void Item::hello() const {
    std::cout << "Soy Item" << std::endl;
}
