#include "createEnemies.h"

// Creates a zombie enemy with proper sprite and hitbox setup
Zombie *createZombie(const sf::Vector2f &position)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect ZOMBIE_SPRITE_REGION = {{1, 28}, {16, 32}};
    const float HITBOX_WIDTH = 12.f;
    const float HITBOX_HEIGHT = 30.f;

    // Configure sprite
    auto zombieSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    zombieSprite->setTextureRect(ZOMBIE_SPRITE_REGION);
    zombieSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = zombieSprite->getLocalBounds();
    zombieSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Zombie(zombieSprite, hitboxes);
}

// Creates a leopard enemy with level-specific settings
Leopard *createLeopard(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect LEOPARD_SPRITE_REGION = {{56, 11}, {24, 16}};
    const float HITBOX_WIDTH = 22.f;
    const float HITBOX_HEIGHT = 18.f;

    // Configure sprite
    auto leopardSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    leopardSprite->setTextureRect(LEOPARD_SPRITE_REGION);
    leopardSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = leopardSprite->getLocalBounds();
    leopardSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});
    
    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Leopard(leopardSprite, hitboxes, level, stage);
}

// Creates a bat enemy with spawn zone parameters
Bat *createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect BAT_SPRITE_REGION = {{184, 11}, {16, 16}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Configure sprite
    auto batSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    batSprite->setTextureRect(BAT_SPRITE_REGION);
    batSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = batSprite->getLocalBounds();
    batSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Bat(batSprite, hitboxes, position, zoneSize, level, stage);
}

// Creates a fishman enemy with spawn zone parameters
FishMan *createFishManSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage, std::mt19937 &rngReference)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect FISHMAN_SPRITE_REGION = {{309, 28}, {16, 32}};
    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 28.f;

    // Configure sprite
    auto fishmanSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    fishmanSprite->setTextureRect(FISHMAN_SPRITE_REGION);
    fishmanSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = fishmanSprite->getLocalBounds();
    fishmanSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new FishMan(fishmanSprite, hitboxes, position, zoneSize, level, stage, rngReference);
}