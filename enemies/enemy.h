#pragma once

#include <SFML/Graphics.hpp>
#include "../entity.h"
#include "../tilemap.h"

class Enemy : public EntitySprite
{
private:
    const float GRAVITY = 750.0f;

    sf::Vector2f originalPosition;

public:
    sf::Vector2f speed = {-75.0f, 0.0f};
    float score = 100.0f;
    float damage = 2.0f;
    float life = 1.0f;

    bool isActive = false;
    bool isOnGround = false;
    bool needsPlayerToLeaveZone = false;

    Enemy() = default;
    Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void checkBasicCollisions(const TileMap &tileMap, const sf::FloatRect &hitbox);
    virtual void applyGravity(float deltaTime);
    virtual void resetPosition();

    virtual void draw(sf::RenderWindow &window, bool debugDraw);

    // Entity functions
    sf::FloatRect getBounds() const override;
    void onCollision(Entity& other) override;
};