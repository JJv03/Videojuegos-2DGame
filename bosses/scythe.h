#pragma once

#include "../entity.h"
#include "../player.h"

class Scythe : public EntitySprite
{
private:
    // State tracking
    bool isActive = false;
    animationID currentAnimation;
    std::vector<AnimationManager::Frame> normalFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(179, 74), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(196, 74), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(213, 74), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(230, 74), sf::Vector2(16, 16)), 0.15f}
    };
    std::vector<AnimationManager::Frame> gamePadFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(179, 106), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(196, 106), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(213, 106), sf::Vector2(16, 16)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(230, 106), sf::Vector2(16, 16)), 0.15f}
    };

public:
    // Movement properties
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::FloatRect mapDims;
    // Damage value
    float damage;

    float timer = 0.f;
    const float timeToMove = 2.5f;

    Scythe() = default;
    Scythe(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::FloatRect &mapDims, float _damage = 2.0f);

    // Update scythe position and check if it's still in the valid zone
    void update(float deltaTime, const sf::FloatRect &deactivationZone);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Check if scythe is active
    bool getActive() const { return isActive; }

    // Set scythe active state
    void setActive(bool active) { isActive = active; }

    // Set scythe speed
    void planPosSpeed(const sf::Vector2f goal);

    // Reset scythe to initial state
    void reset();

    // Draw the scythe
    void draw(sf::RenderWindow &window);
};