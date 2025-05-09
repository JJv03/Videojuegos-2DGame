#pragma once

#include "../entity.h"
#include "../player.h"

class Axe : public EntitySprite
{
private:
    // State tracking
    bool isActive = false;
    animationID currentAnimation;
    std::vector<AnimationManager::Frame> normalFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(290, 122), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(309, 122), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(324, 122), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(341, 122), sf::Vector2(16, 16)), 0.15f}
    };

    const float maxDist = 7 * 16; // 7 halftiles

public:
    // Movement properties
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::FloatRect mapDims;
    bool facingRight = false;
    // Damage value
    float damage;

    float timer = 0.f;
    const float timeToMove = 2.5f;
    bool changedSpeed = false;

    Axe() = default;
    Axe(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float _damage = 2.0f);

    // Update axe position and check if it's still in the valid zone
    void update(float deltaTime, const sf::FloatRect &deactivationZone);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Check if axe is active
    bool getActive() const { return isActive; }

    // Set axe active state
    void setActive(bool active) { isActive = active; }

    // Set axe speed
    void planPosSpeed(const sf::Vector2f goal);

    // Reset scythe to initial state
    void reset();

    // Draw the scythe
    void draw(sf::RenderWindow &window);
};