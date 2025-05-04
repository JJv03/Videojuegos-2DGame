#pragma once

#include "../entity.h"
#include "../player.h"

class Bandage : public EntitySprite
{
private:
    // State tracking
    bool isActive = false;
    animationID currentAnimation;
    std::vector<AnimationManager::Frame> normalFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(376, 16), sf::Vector2(16, 9)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(393, 16), sf::Vector2(16, 9)), 0.15f}
    };

public:
    // Movement properties
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::FloatRect mapDims;
    // Damage value
    float damage;

    float curveTime = 0.0f;

    Bandage() = default;
    Bandage(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::FloatRect &mapDims, float _damage = 2.0f);

    // Update Bandage position and check if it's still in the valid zone
    void update(float deltaTime, const sf::FloatRect &deactivationZone);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game) override;

    // Check if Bandage is active
    bool getActive() const { return isActive; }

    // Set Bandage active state
    void setActive(bool active) { isActive = active; }

    // Reset Bandage to initial state
    void reset();

    // Draw the Bandage
    void draw(sf::RenderWindow &window);
};