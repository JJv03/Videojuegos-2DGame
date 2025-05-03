#pragma once

#include "../entity.h"
#include "../player.h"

class Scythe : public EntitySprite
{
private:
    // Movement properties
    sf::Vector2f velocity;

    // State tracking
    bool isActive = false;


public:

    // Damage value
    float damage = 1.0f;

    Scythe() = default;
    Scythe(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::Vector2f &_velocity, float _damage = 1.0f);

    // Update scythe position and check if it's still in the valid zone
    void update(float deltaTime, const sf::FloatRect &deactivationZone);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game) override;

    // Check if scythe is active
    bool getActive() const { return isActive; }

    // Set scythe active state
    void setActive(bool active) { isActive = active; }

    // Reset scythe to initial state
    void reset();

    // Draw the scythe
    void draw(sf::RenderWindow &window);
};