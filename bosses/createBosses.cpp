#include "createBosses.h"

// Creates a phantomBat enemy with spawn zone parameters
PhantomBat *createPhantomBat(const sf::Vector2f &position, const size_t &level, const size_t &stage)
{
    // Sprite sheet coordinates and dimensions
    const sf::IntRect PBAT_SPRITE_REGION = {{1, 11}, {16, 16}};
    const float HITBOX_WIDTH = 40.f;
    const float HITBOX_HEIGHT = 18.f;

    // Configure sprite
    auto phantomBatSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    phantomBatSprite->setTextureRect(PBAT_SPRITE_REGION);
    phantomBatSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = phantomBatSprite->getLocalBounds();
    phantomBatSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    return new PhantomBat(phantomBatSprite, hitboxes, position, level, stage);
}
