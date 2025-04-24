#include "boss.h"
#include <iostream>

Boss::Boss(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : EntitySprite(_sprite, _hitboxes), configManager(configManager::getInstance()) 
{
    // Store initial position for later reset
    originalPosition = sprite->getPosition();
    originalHitboxes = _hitboxes;
}

void Boss::onCollision_SolidTile(Entity &solidTile)
{
    sf::FloatRect bossHitbox = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    for (auto tileBound : tileBounds)
    {
        if (tileBound.size.x == 0 || tileBound.size.y == 0)
            continue;

        if (const std::optional<sf::FloatRect> intersection = bossHitbox.findIntersection(tileBound))
        {
            float overlapX = intersection->size.x;
            float overlapY = intersection->size.y;

            const float minHeightThreshold = 0.25f;

            // Handle horizontal collision (left/right)
            if (overlapX < overlapY && overlapY > minHeightThreshold)
            {
                if (bossHitbox.position.x < tileBound.position.x + tileBound.size.x / 2.f)
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
                if (bossHitbox.position.y < tileBound.position.y + tileBound.size.y / 2.f)
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

void Boss::draw(sf::RenderWindow &window)
{
    window.draw(*sprite); // Draw the boss sprite

    if (gDrawHitboxes) // Only draw hitboxes in debug mode
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

void Boss::applyGravity(float deltaTime)
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

void Boss::resetPosition()
{
    // Reset to original position and restore initial speed
    sprite->setPosition(originalPosition);

    hitboxes = originalHitboxes;
}

bool Boss::applyDamage(float damage, Player &player)
{
    life -= damage;
    if (life <= 0.0f)
    {
        player.score += this->score;
        isActive = false;
        return true; // Boss defeated
    }

    return false; // Boss still alive
}

void Boss::hello() const
{
    std::cout << "Soy Boss" << std::endl;
}