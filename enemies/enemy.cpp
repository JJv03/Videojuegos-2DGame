#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : EntitySprite(_sprite, _hitboxes)
{
    // Store initial position for later reset
    originalPosition = sprite->getPosition();
}

void Enemy::onCollision_SolidTile(Entity &solidTile)
{
    sf::FloatRect enemyHitbox = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    for (auto tileBound : tileBounds)
    {
        if (tileBound.size.x == 0 || tileBound.size.y == 0)
            continue;

        if (const std::optional<sf::FloatRect> intersection = enemyHitbox.findIntersection(tileBound))
        {
            float overlapX = intersection->size.x;
            float overlapY = intersection->size.y;

            const float minHeightThreshold = 0.25f;

            // Handle horizontal collision (left/right)
            if (overlapX < overlapY && overlapY > minHeightThreshold)
            {
                if (enemyHitbox.position.x < tileBound.position.x + tileBound.size.x / 2.f)
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
                if (enemyHitbox.position.y < tileBound.position.y + tileBound.size.y / 2.f)
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

void Enemy::onCollision_BreakableTile(const sf::FloatRect &tileBound)
{
    sf::FloatRect enemyHitbox = this->sprite->getGlobalBounds();

    if (tileBound.size.x == 0 || tileBound.size.y == 0) return;

    if (const std::optional<sf::FloatRect> intersection = enemyHitbox.findIntersection(tileBound))
    {
        float overlapX = intersection->size.x;
        float overlapY = intersection->size.y;

        const float minHeightThreshold = 0.25f;

        // Handle horizontal collision (left/right)
        if (overlapX < overlapY && overlapY > minHeightThreshold)
        {
            if (enemyHitbox.position.x < tileBound.position.x + tileBound.size.x / 2.f)
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
            if (enemyHitbox.position.y < tileBound.position.y + tileBound.size.y / 2.f)
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

void Enemy::onCollision_OnlyGround(Entity &solidTile)
{
    sf::FloatRect enemyHitbox = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    for (auto tileBound : tileBounds)
    {
        if (tileBound.size.x == 0 || tileBound.size.y == 0)
            continue;

        if (const std::optional<sf::FloatRect> intersection = enemyHitbox.findIntersection(tileBound))
        {
            float enemyBottom = enemyHitbox.position.y + enemyHitbox.size.y;
            float tileTop = tileBound.position.y;

            float overlapY = intersection->size.y;
            if (enemyBottom >= tileTop && enemyBottom <= tileTop + overlapY + 5.0f)
            {
                float newY = tileTop - enemyHitbox.size.y;
                float deltaY = newY - enemyHitbox.position.y;

                sprite->move({0.f, deltaY});
                for (auto &h : hitboxes)
                    h.position.y += deltaY;

                isOnGround = true;
            }
        }
    }
}

bool Enemy::checkForLedge(Entity &solidTile)
{
    if (!isOnGround || std::abs(speed.x) < 0.1f)
        return false;

    // Check area in movement direction
    float offsetX = (speed.x > 0) ? hitboxes[0].size.x + 10.0f : -10.0f;

    sf::Vector2f checkPoint = {
        hitboxes[0].position.x + offsetX,
        hitboxes[0].position.y + hitboxes[0].size.y + 5.0f};

    // Small rectangle to check collision with the ground
    sf::FloatRect checkRect = sf::FloatRect(
        {checkPoint.x - 5.0f, checkPoint.y},
        {10.0f, 10.0f});

    const float minHeightThreshold = 5.0f; // Vertical tolerance

    for (const auto &hitbox : solidTile.hitboxes)
    {
        if (hitbox.size.x == 0 || hitbox.size.y == 0)
            continue;

        if (const std::optional<sf::FloatRect> intersection = checkRect.findIntersection(hitbox))
        {
            if (intersection->size.y >= minHeightThreshold)
            {
                return false;
            }
        }
    }

    return true;
}

void Enemy::draw(sf::RenderWindow &window)
{
    window.draw(*sprite); // Draw the enemy sprite

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

bool Enemy::applyDamage(float damage)
{
    life -= damage;
    if (life <= 0.0f)
    {
        isActive = false;
        return true; // Enemy defeated
    }

    return false; // Enemy still alive
}

void Enemy::hello() const
{
    std::cout << "Soy Enemy" << std::endl;
}