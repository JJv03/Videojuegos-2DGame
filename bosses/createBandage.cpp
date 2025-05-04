#include "createBandage.h"

std::shared_ptr<Bandage> createBandage(const sf::Vector2f &position, const sf::FloatRect &mapDims, float damage)
{
    sf::IntRect spriteRegion;
    float hitboxWidth = 16.0f;
    float hitboxHeight = 9.0f;

    spriteRegion = {{376, 16}, {16, 9}};
    hitboxWidth = 16.0f;
    hitboxHeight = 9.0f;

    // Configure sprite
    auto projectileSprite = std::make_shared<sf::Sprite>(gTextures["boss"]);
    projectileSprite->setTextureRect(spriteRegion);
    projectileSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = projectileSprite->getLocalBounds();
    projectileSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (hitboxWidth / 2.f), position.y - (hitboxHeight / 2.f)},
            {hitboxWidth, hitboxHeight})};

    // Create and return projectile with shared_ptr
    return std::make_shared<Bandage>(projectileSprite, hitboxes, position, mapDims, damage);
}
