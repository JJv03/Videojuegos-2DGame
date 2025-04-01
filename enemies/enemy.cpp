#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : EntitySprite(_sprite, _hitboxes)
{
    // Guarda la posición inicial
    originalPosition = sprite->getPosition();
}

void Enemy::checkBasicCollisions(const TileMap &tileMap, const sf::FloatRect &hitbox)
{
    // Tilemap collisions
    sf::FloatRect mapBounds = tileMap.getMapBounds();

    if (!hitbox.findIntersection(mapBounds))
        return;

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

                    if (overlapX < overlapY && overlapY > minHeightThreshold) // Colisión horizontal
                    {
                        if (hitbox.position.x < tileBounds.position.x + tileBounds.size.x / 2.f)
                        {
                            // Colisión desde la izquierda
                            sprite->move({-overlapX, 0.f});
                            for (auto &h : hitboxes)
                                h.position.x -= overlapX;
                        }
                        else
                        {
                            // Colisión desde la derecha
                            sprite->move({overlapX, 0.f});
                            for (auto &h : hitboxes)
                                h.position.x += overlapX;
                        }
                        speed.x = -speed.x;
                    }
                    else // Colisión vertical
                    {
                        if (hitbox.position.y < tileBounds.position.y + tileBounds.size.y / 2.f)
                        {
                            // Colisión desde arriba
                            sprite->move({0.f, -overlapY});
                            for (auto &h : hitboxes)
                                h.position.y -= overlapY;
                            isOnGround = true;
                        }
                        else
                        {
                            // Colisión desde abajo
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
    window.draw(*sprite);

    if (debugDraw)
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
    sf::Vector2f offset = originalPosition - sprite->getPosition();

    needsPlayerToLeaveZone = true;

    sprite->setPosition(originalPosition);

    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    speed = {-75.0f, 0.0f};
}