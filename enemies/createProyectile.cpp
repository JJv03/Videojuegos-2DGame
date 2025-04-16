#include "createProjectile.h"

std::shared_ptr<Projectile> createProjectile(const sf::Vector2f &position, const sf::Vector2f &velocity, ProjectileType type, float damage)
{
    sf::IntRect spriteRegion;
    std::string texturePath = "./assets/sprites/enemies/enemies.png";
    float hitboxWidth = 8.0f;
    float hitboxHeight = 8.0f;

    switch (type)
    {
    case ProjectileType::FISHMAN:
        spriteRegion = {{377, 28}, {8, 8}};
        hitboxWidth = 7.0f;
        hitboxHeight = 7.0f;
        break;
    default:
        spriteRegion = {{377, 28}, {8, 8}};
        hitboxWidth = 7.0f;
        hitboxHeight = 7.0f;
        break;
    }

    // Load and process sprite texture
    sf::Image projectileImage;
    if (!projectileImage.loadFromFile(texturePath))
    {
        std::cerr << "Error loading projectile sprite sheet" << std::endl;
        throw std::runtime_error("Error loading projectile sprite sheet");
    }
    projectileImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // Remove background color

    sf::Texture *projectileTexture = new sf::Texture();
    if (!projectileTexture->loadFromImage(projectileImage))
    {
        std::cerr << "Error loading texture from image" << std::endl;
        delete projectileTexture;
        throw std::runtime_error("Error loading texture from image");
    }

    // Configure sprite
    auto projectileSprite = std::make_shared<sf::Sprite>(*projectileTexture);
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
