#include "createBosses.h"

// Creates the phantomBat boss
PhantomBat *createPhantomBat(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect PBAT_SPRITE_REGION = {{1, 11}, {16, 16}};
    const float HITBOX_WIDTH_1 = 48.f;
    // const float HITBOX_WIDTH_2 = 32.f;
    const float HITBOX_HEIGHT = 24.f;

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
            {position.x - (HITBOX_WIDTH_1 / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH_1, HITBOX_HEIGHT}),
    };

    return new PhantomBat(phantomBatSprite, hitboxes, position, level, stage, mapDims);
}

// Creates Dracula boss
Dracula *createDracula(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect DRACULA_SPRITE_REGION = {{325, 74}, {24, 59}};
    const float HITBOX_WIDTH = 23.f;
    const float HITBOX_HEIGHT = 58.f;

    // Configure sprite
    auto draculaSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    draculaSprite->setTextureRect(DRACULA_SPRITE_REGION);
    draculaSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = draculaSprite->getLocalBounds();
    draculaSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new Dracula(draculaSprite, hitboxes, position, level, stage);
}

// Creates DraculaSpirit boss
DraculaSpirit *createDraculaSpirit(const sf::Vector2f &position, const size_t &level, const size_t &stage)
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

    return new DraculaSpirit(draculaSpiritSprite, hitboxes, position, level, stage);
}
