#include "createBosses.h"
#include "../configManager.h"

// Creates the phantomBat boss
PhantomBat *createPhantomBat(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect PBAT_SPRITE_REGION = {{1, 11}, {16, 16}};

    // Configure sprite
    auto phantomBatSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    phantomBatSprite->setTextureRect(PBAT_SPRITE_REGION);
    phantomBatSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = phantomBatSprite->getLocalBounds();
    phantomBatSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        // Wings opened
        sf::FloatRect(
            {position.x + 4.f, position.y},
            {16.f, 7.f}
        ),
        // sf::FloatRect(
        //     {position.x - 8.f, position.y - 16.f},
        //     {48.f, 16.f}),
        sf::FloatRect(
            {position.x - 4.f, position.y - 16.f},
            {32.f, 16.f}),
    };

    return new PhantomBat(phantomBatSprite, hitboxes, position, level, stage, mapDims);
}

// Creates Dracula boss
Dracula *createDracula(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // -------- MASK (main sprite) --------
    // Sprite sheet coordinates and dimensions
    const sf::IntRect MASK_SPRITE_REGION = {{307, 91}, {8, 16}};
    const float MASK_HITBOX_WIDTH = 8.f;
    const float MASK_HITBOX_HEIGHT = 16.f;

    // Configure sprite
    auto maskSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    maskSprite->setTextureRect(MASK_SPRITE_REGION);
    maskSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect maskBounds = maskSprite->getLocalBounds();
    maskSprite->setOrigin({maskBounds.size.x / 2.f, maskBounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> maskHitboxes = {
        sf::FloatRect(
            {position.x - (MASK_HITBOX_WIDTH / 2.f), position.y - MASK_HITBOX_HEIGHT},
            {MASK_HITBOX_WIDTH, MASK_HITBOX_HEIGHT}),
    };

    // -------- BODY (secondary sprite) --------
    // Sprite sheet coordinates and dimensions
    const sf::IntRect DRACULA_SPRITE_REGION = {{249, 74}, {24, 48}};
    const float DRACULA_HITBOX_WIDTH = 24.f;
    const float DRACULA_HITBOX_HEIGHT = 48.f;

    sf::Vector2f bodyPosition = sf::Vector2f({position.x + 2.f, position.y + 43.f});

    // Configure sprite
    auto draculaSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);

    draculaSprite->setTextureRect(DRACULA_SPRITE_REGION);
    draculaSprite->setPosition(bodyPosition);

    // Center sprite origin
    sf::FloatRect draculaBounds = draculaSprite->getLocalBounds();
    draculaSprite->setOrigin({draculaBounds.size.x / 2.f, draculaBounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> draculaHitboxes = {
        sf::FloatRect(
            {bodyPosition.x - (DRACULA_HITBOX_WIDTH / 2.f), bodyPosition.y - DRACULA_HITBOX_HEIGHT},
            {DRACULA_HITBOX_WIDTH, DRACULA_HITBOX_HEIGHT}),
    };

    return new Dracula(maskSprite, maskHitboxes, 
                        draculaSprite, draculaHitboxes, 
                        level, stage, bodyPosition);
}

// Creates DraculaSpirit boss
DraculaSpirit *createDraculaSpirit(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect DRACULA_SPRITE_REGION = {{1, 148}, {48, 64}};
    const float HITBOX_WIDTH = 46.f;
    const float HITBOX_HEIGHT = 62.f;

    // Configure sprite
    auto draculaSpiritSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    draculaSpiritSprite->setTextureRect(DRACULA_SPRITE_REGION);
    draculaSpiritSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = draculaSpiritSprite->getLocalBounds();
    draculaSpiritSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new DraculaSpirit(draculaSpiritSprite, hitboxes, position, level, stage, mapDims);
}

// Creates the phantomBat boss
Death *createDeath(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims)
{
    configManager &configManager = configManager::getInstance();
    auto skins = configManager.getSkins();
    // Sprite sheet coordinates and dimensions
    sf::IntRect DEATH_SPRITE_REGION;
    if(skins.activated){
        DEATH_SPRITE_REGION = {{97, 74}, {40, 48}}; // Ibon
    }
    else{
        DEATH_SPRITE_REGION = {{138, 74}, {40, 48}}; // Death
    }

    // Configure sprite
    auto deathSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    deathSprite->setTextureRect(DEATH_SPRITE_REGION);
    deathSprite->setPosition(position);

    sf::FloatRect bounds = deathSprite->getLocalBounds();
    deathSprite->setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y));

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        // Wings opened
        sf::FloatRect(
            {position.x - 12.f, position.y - 48.f},
            {24.f, 48.f}
        )
    };

    return new Death(deathSprite, hitboxes, position, level, stage, mapDims);
}

// Creates the phantomBat boss
MummyMan *createMummyMan(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims)
{
    // Sprite sheet coordinates and dimensions
    sf::IntRect DEATH_SPRITE_REGION;
    
    DEATH_SPRITE_REGION = {{274, 19}, {16, 40}}; // Mummy Left

    // Configure sprite
    auto mummySprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    mummySprite->setTextureRect(DEATH_SPRITE_REGION);
    mummySprite->setPosition(position);

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        // Wings opened
        sf::FloatRect(
            {position.x, position.y},
            {40.f, 48.f}
        )
    };

    return new MummyMan(mummySprite, hitboxes, position, level, stage, mapDims);
}