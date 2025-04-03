#include "bat.h"
#include <iostream>
#include <cmath>

Bat::Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
         const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = BAT_SPEED;
    life = BAT_LIFE;
    score = BAT_SCORE;
    damage = BAT_DAMAGE;
}

void Bat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone)
{
    // GESTIÓN DE RESPAWN

    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    if (batToSpawn)
    {
        spawnerActive = true;
    }
    else
    {
        spawnerActive = false;
    }

    // Activación del proceso de spawn
    if (playerInZone && !isActive && !spawnerActive)
    {
        batSpawnTimers = 1.5f;

        batToSpawn = true;
    }

    // Spawn según el timer

    if (batToSpawn)
    {
        batSpawnTimers -= deltaTime;

        if (batSpawnTimers <= 0.0f)
        {
            movePositionToBorder(playerActivationZone);

            isActive = true;
            batToSpawn = false;
        }
    }

    // GESTIÓN DE MOVIMIENTO
    if (isActive)
    {

        bool zombieInsideDeactivationZone = false;

        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                zombieInsideDeactivationZone = true;
                break;
            }
        }

        if (!zombieInsideDeactivationZone)
        {

            isActive = false;
            resetPosition();
        }
        else
        {
            float horizontalSpeed = speed.x * deltaTime;
            sprite->move({horizontalSpeed, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += horizontalSpeed;
            }

            spawnTime += deltaTime;
            float verticalOscillation = cos(spawnTime * OSCILLATION_SPEED) * OSCILLATION_AMPLITUDE;
            sprite->move({0.f, verticalOscillation});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y += verticalOscillation;
            }

            updateAnimation(deltaTime);
        }
    }
}

int Bat::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                          const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return 0;
    int score2Return = 0;
    for (auto &hitbox : hitboxes)
    {

        // Vampire killer (whip) collisions
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
                    score2Return = score;
                    break;
                }
            }
        }
    }

    // Player collisions
    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            isActive = false;
            resetPosition();
            break;
        }
    }
    return score2Return;
}

void Bat::resetPosition()
{
    Enemy::resetPosition();

    speed = BAT_SPEED;
    life = BAT_LIFE;

    spawnTime = 0.0f;

    animTimer = 0.0f;
    currentFrame = 0;
}

void Bat::movePositionToBorder(const sf::FloatRect &playerActivationZone)
{

    if (!sprite)
        return;

    // Borde derecho de la zona de activación
    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x;

    // Misma altura que el player
    float originalY = (playerActivationZone.position.y + playerActivationZone.size.y / 2.0f) - 5.0f;

    // Guardar la posición actual
    sf::Vector2f oldPosition = sprite->getPosition();

    // Nueva posición
    sf::Vector2f newPosition(rightEdgeX, originalY);
    sprite->setPosition(newPosition);

    // Actualiza las hitboxes
    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

void Bat::draw(sf::RenderWindow &window, bool debugDraw)
{
    if (sprite && isActive)
    {
        Enemy::draw(window, debugDraw);
    }

    if (debugDraw)
    {
        sf::RectangleShape zoneShape({spawnZone.size.x, spawnZone.size.y});
        zoneShape.setPosition({spawnZone.position.x, spawnZone.position.y});
        zoneShape.setFillColor(sf::Color(255, 255, 0, 50));
        zoneShape.setOutlineColor(sf::Color::Yellow);
        zoneShape.setOutlineThickness(1.0f);
        window.draw(zoneShape);
    }
}

void Bat::updateAnimation(float deltaTime)
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