#include "projectile.h"
#include <iostream>

Projectile::Projectile(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
                       const sf::Vector2f &position, const sf::Vector2f &_velocity, float _damage)
    : EntitySprite(_sprite, _hitboxes), velocity(_velocity), damage(_damage)
{
}

void Projectile::update(float deltaTime, const sf::FloatRect &deactivationZone)
{
    if (!isActive || !sprite)
    {
        return;
    }

    // Move the projectile
    sf::Vector2f movement = velocity * deltaTime;
    sprite->move(movement);

    // Update hitboxes
    for (auto &hitbox : hitboxes)
    {
        hitbox.position.x += movement.x;
        hitbox.position.y += movement.y;
    }

    // Check if projectile is outside deactivation zone
    bool isInsideZone = false;
    for (const auto &hitbox : hitboxes)
    {
        if (deactivationZone.findIntersection(hitbox).has_value())
        {
            isInsideZone = true;
            break;
        }
    }

    if (!isInsideZone)
    {
        isActive = false;
    }
}

void Projectile::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive)
    {
        return;
    }

    // Handle collision with player
    if (dynamic_cast<Player *>(&other))
    {
        isActive = false;
    }
    // Handle collision with weapons
    else if (dynamic_cast<Whip *>(&other))
    {
        isActive = false;
    }
    else if (dynamic_cast<SubWeapon *>(&other))
    {
        isActive = false;
    }
}

void Projectile::reset()
{
    isActive = false;
}

void Projectile::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        window.draw(*sprite);

        if (gDrawHitboxes) // Only draw hitboxes in debug mode
        {
            for (const auto &hitbox : hitboxes)
            {
                sf::RectangleShape hitboxShape({hitbox.size.x, hitbox.size.y});
                hitboxShape.setPosition({hitbox.position.x, hitbox.position.y});
                hitboxShape.setFillColor(sf::Color::Transparent);
                hitboxShape.setOutlineColor(sf::Color::Red);
                hitboxShape.setOutlineThickness(1.0f);
                window.draw(hitboxShape);
            }
        }
    }
}