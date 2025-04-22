#pragma once

#include <SFML/Graphics.hpp>
#include "../entity.h"
#include "../tilemap.h"
#include "../player.h"

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
    bool needsPlayerToLeaveZone = false; // Reset condition flag

    animationID currentAnimation;        // Current animation ID

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    virtual void applyGravity(float deltaTime);

    // Resets enemy to its original position
    virtual void resetPosition();

    virtual void draw(sf::RenderWindow &window);

    void onCollision_SolidTile(Entity &solidTile);
    void onCollision_BreakableTile(const sf::FloatRect &tileBounds);
    void onCollision_OnlyGround(Entity &solidTile);

    // Check if the enemy is out of the map bounds and disable it
    bool checkMapBoundaries(const sf::FloatRect &mapBounds);

    // Applies damage to the enemy and returns true if the enemy is defeated
    bool applyDamage(float damageAmount, Player &player);

    bool checkForLedge(Entity &solidTile);

    void hello() const override;
};