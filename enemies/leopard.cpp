#include "leopard.h"
#include <iostream>
#include <cmath>

Leopard::Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const size_t &level, const size_t &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = {0.0f, 0.0f};
    life = LEOPARD_LIFE;
    score = LEOPARD_SCORE;
    damage = LEOPARD_DAMAGE;

    // Campo de visión
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

void Leopard::updateVisionField()
{
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

bool Leopard::checkForLedge(const TileMap &tileMap)
{
    if (!isOnGround || std::abs(speed.x) < 0.1f)
        return false;

    float offsetX = (speed.x > 0) ? hitboxes[0].size.x + 10.0f : -10.0f;

    sf::Vector2f checkPoint = {
        hitboxes[0].position.x + offsetX,
        hitboxes[0].position.y + hitboxes[0].size.y + 5.0f};

    // Pequeño rectángulo para verificar colisión con el suelo
    sf::FloatRect checkRect = sf::FloatRect(
        {checkPoint.x - 5.0f, checkPoint.y},
        {10.0f, 10.0f});

    const float minHeightThreshold = 5.0f; // Tolerancia vertical

    for (const auto &row : tileMap.m_solidTiles)
    {
        for (const auto &tile : row)
        {
            if (tile.hitbox.size.x == 0 || tile.hitbox.size.y == 0)
                continue;

            if (const std::optional<sf::FloatRect> intersection = checkRect.findIntersection(tile.hitbox))
            {
                if (intersection->size.y >= minHeightThreshold)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void Leopard::jump()
{
    if (isOnGround)
    {
        speed.y = LEOPARD_JUMP_SPEED;
        isOnGround = false;
    }
}

void Leopard::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone, const sf::Vector2f &playerPos)
{
    playerPosition = playerPos;

    // GESTIÓN DE RESPAWN
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

    // Si el jugador está fuera de la zona de activación, se permite que el enemigo se reactive en el futuro
    if (!enemyInsideActivationZone)
    {
        needsPlayerToLeaveZone = false;
    }

    // Solo activamos si el jugador está en la zona, el enemigo no está activo y el jugador se alejó previamente
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        isActive = true;
    }

    // Se desactiva si el enemigo está activo y salió de la zona de desactivación
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    // GESTIÓN DE MOVIMIENTO
    if (isActive)
    {
        updateVisionField();

        if (playerDetected && speed.x == 0)
        {
            float directionToPlayer = (playerPosition.x > sprite->getPosition().x) ? 1.0f : -1.0f;
            speed.x = std::abs(LEOPARD_SPEED.x) * directionToPlayer;
        }
        else if (!playerDetected && !hasRedirected && speed.x != 0 && isOnGround)
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
}

void Leopard::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                              const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    playerDetected = visionField.findIntersection(simonBounds).has_value();

    for (auto &hitbox : hitboxes)
    {
        // COLISIONES CON EL ENTORNO
        sf::FloatRect mapBounds = tileMap.getMapBounds();

        if (!hitbox.findIntersection(mapBounds))
            continue;

        for (size_t row = 0; row < tileMap.m_solidTiles.size(); ++row)
        {
            for (size_t col = 0; col < tileMap.m_solidTiles[row].size(); ++col)
            {
                sf::FloatRect tileBounds = tileMap.m_solidTiles[row][col].hitbox;
                if (tileBounds.size.x == 0 || tileBounds.size.y == 0)
                    continue;

                if (const std::optional<sf::FloatRect> intersection = hitbox.findIntersection(tileBounds))
                {
                    float overlapX = intersection->size.x;
                    float overlapY = intersection->size.y;

                    const float minHeightThreshold = 0.1f;

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

        // Verificar si hay un precipicio adelante y saltar
        if (isOnGround && checkForLedge(tileMap))
        {
            jump();
        }

        // COLISIONES CON VAPIRE KILLER
        if (playerIsAtacking)
        {
            // EL SISTEMA DE ATAQUE DEBERÁ TENER UN COOLDAWN CUANDO GOLPEE ALGO
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

void Leopard::draw(sf::RenderWindow &window, bool debugDraw)
{
    if (sprite && isActive)
    {
        Enemy::draw(window, debugDraw);

        if (debugDraw)
        {
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

    // Voltear el sprite según la dirección del movimiento
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