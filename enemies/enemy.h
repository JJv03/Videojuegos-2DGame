#pragma once

#include <SFML/Graphics.hpp>
#include "../entity.h"
#include "../tilemap.h"

class Enemy : public EntitySprite
{
private:
    const float GRAVITY = 750.0f; // Gravity force

    sf::Vector2f originalPosition; // Initial position

public:
    sf::Vector2f speed = {-75.0f, 0.0f}; // Default movement speed (left direction)
    float score = 100.0f;                // Points awarded when defeated
    float damage = 2.0f;                 // Damage dealt to player on collision
    float life = 1.0f;                   // Enemy health

    bool isActive = false;               // Whether the enemy is currently active/visible
    bool isOnGround = false;             // Flag for ground collision detection
    bool needsPlayerToLeaveZone = false; // Reset condition flag

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    // Checks collisions with solid tiles in the tilemap
    void checkBasicCollisions(const TileMap &tileMap, const sf::FloatRect &hitbox);

    virtual void applyGravity(float deltaTime);

    // Resets enemy to its original position
    virtual void resetPosition();

    virtual void draw(sf::RenderWindow &window, bool debugDraw);

    // Entity functions
    void onCollision(Entity &other) override;
};