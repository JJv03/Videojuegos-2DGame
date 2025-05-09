#pragma once

#include "../entity.h"
#include "../player.h"

class Projectile : public EntitySprite
{
private:
    // Movement properties
    sf::Vector2f velocity;

    // State tracking
    bool isActive = false;


public:

    // Damage value
    float damage = 1.0f;

    Projectile() = default;
    Projectile(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::Vector2f &_velocity, float _damage = 1.0f);

    // Update projectile position and check if it's still in the valid zone
    void update(float deltaTime, const sf::FloatRect &deactivationZone);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Check if projectile is active
    bool getActive() const { return isActive; }

    // Set projectile active state
    void setActive(bool active) { isActive = active; }

    // Reset projectile to initial state
    void reset();

    // Draw the projectile
    void draw(sf::RenderWindow &window);
};