#include "leopard.h"
#include <iostream>
#include <cmath>

// Initialize leopard with stats and vision field
Leopard::Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = {0.0f, 0.0f};
    life = LEOPARD_LIFE;
    score = LEOPARD_SCORE;
    damage = LEOPARD_DAMAGE;

    // Set initial detection area
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

// Update vision field based on current position
void Leopard::updateVisionField()
{
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

// Detect ledges/cliffs to prevent falling
bool Leopard::checkForLedge(const TileMap &tileMap)
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

    // Scan solid tiles for ground collision
    for (const auto &row : tileMap.m_solidTiles)
    {
        for (const auto &tile : row)
        {
            for (const auto &hitbox : tile.hitboxes)
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
        }
    }

    return true;
}

// Jump action
void Leopard::jump()
{
    if (isOnGround)
    {
        speed.y = LEOPARD_JUMP_SPEED;
        isOnGround = false;
    }
}

// Main update loop
void Leopard::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                     const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds)
{
    playerPosition = playerPos;

    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    for (const auto &hitbox : hitboxes)
    {
        if (playerActivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideActivationZone = true;
        }
        if (playerDeactivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideDeactivationZone = true;
        }
    }

    // If the player is outside the activation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideActivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // We only activate if the player is in the area, the enemy is not active and the player has previously moved away
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        isActive = true;
    }

    // Deactivates if the enemy is active and has left the deactivation zone
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    // MOVEMENT AI LOGIC
    if (isActive)
    {
        updateVisionField();

        if (playerDetected && speed.x == 0) // Chase player if detected
        {
            float directionToPlayer = (playerPosition.x > sprite->getPosition().x) ? 1.0f : -1.0f;
            speed.x = std::abs(LEOPARD_SPEED.x) * directionToPlayer;
        }
        else if (!playerDetected && !hasRedirected && speed.x != 0 && isOnGround) // Continue towards last known position
        {
            float directionToLastPlayerPos = (playerPosition.x > sprite->getPosition().x) ? 1.0f : -1.0f;
            speed.x = std::abs(LEOPARD_SPEED.x) * directionToLastPlayerPos;
            hasRedirected = true;
        }

        applyGravity(deltaTime);

        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }

        updateAnimation(deltaTime);
    }

    // Right before checkCollisions
    isOnGround = false;
    playerDetected = false;
    for (auto &hitbox : simonBounds)
    {
        if (visionField.findIntersection(hitbox).has_value())
        {
            playerDetected = true;
            break;
        }
    }
}

// BORRAR
// Handle collisions
void Leopard::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                              const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    playerDetected = visionField.findIntersection(simonBounds).has_value();

    for (auto &hitbox : hitboxes)
    {
        // Environment collisions
        checkBasicCollisions(tileMap, hitbox);

        // Jump over ledges
        if (isOnGround && checkForLedge(tileMap))
        {
            jump();
        }

        // Weapon collisions
        if (playerIsAtacking)
        {
            if (weaponBounds.findIntersection(hitbox).has_value())
            {
                std::cout << "Vida = " << life << std::endl;
                life -= playerDamage;
                std::cout << "Daño recibido = " << playerDamage << " | " << "Vida = " << life << std::endl;
                if (life <= 0.0f)
                {
                    isActive = false;
                    resetPosition();
                    std::cout << "Enemigo eliminado. Puntos = " << score << std::endl;
                    break;
                }
            }
        }
    }
}

void Leopard::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        // std::cout << "LEOPARDO COLISIONA" << std::endl;
        onCollision_SolidTile(other);

        if (isOnGround && Enemy::checkForLedge(other))
        {
            jump();
        }
    }
    else if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (applyDamage(whip->whipDmg))
        {
            resetPosition();
        }
    }
    else if (dynamic_cast<Player *>(&other))
    {
        // Something?
    }
}

// Reset to initial state
void Leopard::resetPosition()
{
    Enemy::resetPosition();

    speed = {0.0f, 0.0f};
    life = LEOPARD_LIFE;

    animTimer = 0.0f;
    currentFrame = 0;
    playerDetected = false;
    hasRedirected = false;
}

// Render with optional debug visuals
void Leopard::draw(sf::RenderWindow &window, bool debugDraw)
{
    if (sprite && isActive)
    {
        Enemy::draw(window, debugDraw);

        if (debugDraw)
        {
            // Draw vision field
            sf::RectangleShape visionRect;
            visionRect.setPosition(visionField.position);
            visionRect.setSize(visionField.size);
            visionRect.setFillColor(sf::Color(0, 255, 0, 50));
            visionRect.setOutlineColor(sf::Color(0, 255, 0));
            visionRect.setOutlineThickness(1.0f);
            window.draw(visionRect);
        }
    }
}

// Update animation frame and direction
void Leopard::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite)
        return;

    animTimer += deltaTime;

    if (animTimer >= ANIM_FRAME_TIME)
    {
        animTimer = 0.0f;
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;

        /*if (currentFrame == 0) {

        } else {

        }*/
    }

    // Flip sprite based on movement direction
    sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}

void Leopard::hello() const
{
    std::cout << "Soy Leopard" << std::endl;
}