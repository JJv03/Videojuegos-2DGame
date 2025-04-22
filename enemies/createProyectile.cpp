#include "createProjectile.h"

std::shared_ptr<Projectile> createProjectile(const sf::Vector2f &position, const sf::Vector2f &velocity, ProjectileType type, float damage)
{
    sf::IntRect spriteRegion;
    float hitboxWidth = 8.0f;
    float hitboxHeight = 8.0f;

    switch (type)
    {
    case ProjectileType::FISHMAN:
        spriteRegion = {{377, 29}, {8, 6}};
        hitboxWidth = 7.0f;
        hitboxHeight = 7.0f;
        break;
    default:
        spriteRegion = {{377, 28}, {8, 8}};
        hitboxWidth = 7.0f;
        hitboxHeight = 7.0f;
        break;
    }

    // Configure sprite
    auto projectileSprite = std::make_shared<sf::Sprite>(gTextures["enemy"]);
    projectileSprite->setTextureRect(spriteRegion);
    projectileSprite->setPosition(position);

    // Center sprite origin
    sf::FloatRect bounds = projectileSprite->getLocalBounds();
    projectileSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

    // Rotate sprite based on velocity direction
    if (velocity.x < 0)
    {
        projectileSprite->setScale({1.0f, 1.0f});
    }
    else if (velocity.x > 0)
    {
        projectileSprite->setScale({-1.0f, 1.0f});
    }

    // Create collision hitbox
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (hitboxWidth / 2.f), position.y - (hitboxHeight / 2.f)},
            {hitboxWidth, hitboxHeight})};

    // Create and return projectile with shared_ptr
    return std::make_shared<Projectile>(projectileSprite, hitboxes, position, velocity, damage);
}
