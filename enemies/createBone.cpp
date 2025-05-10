#include "createBone.h"

std::shared_ptr<Bone> createBone(const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float verticalSpeed, float damage)
{
    sf::IntRect spriteRegion;
    float hitboxWidth = 14.0f;
    float hitboxHeight = 14.0f;

    spriteRegion = {{383, 75}, {16, 16}};
    hitboxWidth = 14.0f;
    hitboxHeight = 14.0f;

    // Configure sprite
    auto projectileSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
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
    return std::make_shared<Bone>(projectileSprite, hitboxes, position, mapDims, facingRight, verticalSpeed, damage);
}
