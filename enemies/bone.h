#pragma once

#include "../entity.h"
#include "../player.h"

class Bone : public EntitySprite
{
private:
    // State tracking
    bool isActive = false;
    animationID currentAnimation;
    std::vector<AnimationManager::Frame> rightFlipFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(400, 75), sf::Vector2(16, 16)), 0.35f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(417, 75), sf::Vector2(16, 16)), 0.35f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(383, 75), sf::Vector2(16, 16)), 0.35f}
    };

    std::vector<AnimationManager::Frame> leftFlipFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(417, 75), sf::Vector2(16, 16)), 0.35f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(400, 75), sf::Vector2(16, 16)), 0.35f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(383, 75), sf::Vector2(16, 16)), 0.35f}
    };

public:
    // Movement properties
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::FloatRect mapDims;
    bool facingRight = false;

    // Damage value
    float damage;

    Bone() = default;
    Bone(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float verticalSpeed, float _damage = 2.0f);

    // Update bone position and check if it's still in the valid zone
    void update(float deltaTime);

    // Handle collisions with other entities
    void onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect) override;

    // Check if bone is active
    bool getActive() const { return isActive; }

    // Set bone active state
    void setActive(bool active) { isActive = active; }

    // Reset bone to initial state
    void reset();

    // Draw the bone
    void draw(sf::RenderWindow &window);
};