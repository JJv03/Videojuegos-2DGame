#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : EntitySprite(_sprite, _hitboxes)
{
    // Store initial position for later reset
    originalPosition = sprite->getPosition();
}

void Enemy::checkBasicCollisions(const TileMap &tileMap, const sf::FloatRect &hitbox)
{
    // Tilemap collisions
    sf::FloatRect mapBounds = tileMap.getMapBounds();

    // Skip collision checks if enemy is outside map bounds
    if (!hitbox.findIntersection(mapBounds))
        return;

    // Check collisions with each solid tile
    for (size_t row = 0; row < tileMap.m_solidTiles.size(); ++row)
    {
        for (size_t col = 0; col < tileMap.m_solidTiles[row].size(); ++col)
        {
            for (auto tileBounds : tileMap.m_solidTiles[row][col].hitboxes)
            {
                if (tileBounds.size.x == 0 || tileBounds.size.y == 0)
                    continue;

                if (const std::optional<sf::FloatRect> intersection = hitbox.findIntersection(tileBounds))
                {
                    float overlapX = intersection->size.x;
                    float overlapY = intersection->size.y;

                    const float minHeightThreshold = 0.25f;

                    // Handle horizontal collision (left/right)
                    if (overlapX < overlapY && overlapY > minHeightThreshold)
                    {
                        if (hitbox.position.x < tileBounds.position.x + tileBounds.size.x / 2.f)
                        {
                            // Left
                            sprite->move({-overlapX, 0.f});
                            for (auto &h : hitboxes)
                                h.position.x -= overlapX;
                        }
                        else
                        {
                            // Right
                            sprite->move({overlapX, 0.f});
                            for (auto &h : hitboxes)
                                h.position.x += overlapX;
                        }
                        speed.x = -speed.x; // Reverse horizontal direction on collision
                    }
                    else // Vertical collision (up/down)
                    {
                        if (hitbox.position.y < tileBounds.position.y + tileBounds.size.y / 2.f)
                        {
                            // Up
                            sprite->move({0.f, -overlapY});
                            for (auto &h : hitboxes)
                                h.position.y -= overlapY;
                            isOnGround = true;
                        }
                        else
                        {
                            // Down
                            sprite->move({0.f, overlapY});
                            for (auto &h : hitboxes)
                                h.position.y += overlapY;
                        }
                    }
                }
            }
        }
    }
}

void Enemy::draw(sf::RenderWindow &window, bool debugDraw)
{
    window.draw(*sprite); // Draw the enemy sprite

    if (debugDraw) // Only draw hitboxes in debug mode
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

void Enemy::applyGravity(float deltaTime)
{
    // Apply gravity only when not on ground
    if (!isOnGround)
    {
        speed.y -= GRAVITY * deltaTime;
    }
    else if (isOnGround)
    {
        speed.y = 0.0f;
    }
}

void Enemy::resetPosition()
{
    // Reset to original position and restore initial speed

    sf::Vector2f offset = originalPosition - sprite->getPosition();

    needsPlayerToLeaveZone = true;

    sprite->setPosition(originalPosition);

    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    speed = {-75.0f, 0.0f};
}

// ------------------------- Entity functions -------------------------
void Enemy::onCollision(Entity& other, Game& game)
{
    
}