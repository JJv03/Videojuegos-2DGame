#include "phantomBat.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Constructor
PhantomBat::PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
         const sf::Vector2f &zoneSize, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), spawnZone({position.x - zoneSize.x / 2.0f, position.y - zoneSize.y / 2.0f}, zoneSize), level(level), stage(stage)
{
    speed = PBAT_SPEED;
    life = PBAT_LIFE;
    score = PBAT_SCORE;
    damage = PBAT_DAMAGE;
}

// Update bat logic
void PhantomBat::update(float deltaTime, const sf::FloatRect &playerActivationZone,
                 const sf::FloatRect &playerDeactivationZone, const sf::FloatRect &playerBounds)
{
    bool playerInZone = spawnZone.findIntersection(playerActivationZone).has_value();

    if (phantomBatToSpawn)
    {
        spawnerActive = true;
    }
    else
    {
        spawnerActive = false;
    }

    // Solo se genera si no se ha generado antes
    if (playerInZone && !isActive && !spawnerActive && !phantomBatSpawned)
    {
        batSpawnTimers = 1.5f; // Delay antes de aparecer
        phantomBatToSpawn = true;
        phantomBatSpawned = true; // Marcar como generado
    }

    if (phantomBatToSpawn)
    {
        batSpawnTimers -= deltaTime;

        if (batSpawnTimers <= 0.0f)
        {
            movePositionToBorder(playerActivationZone, playerBounds);

            isActive = true;
            phantomBatToSpawn = false;
        }
    }

    if (isActive)
    {
        bool batInsideDeactivationZone = false;

        for (const auto &hitbox : hitboxes)
        {
            if (playerDeactivationZone.findIntersection(hitbox).has_value())
            {
                batInsideDeactivationZone = true;
                break;
            }
        }

        if (!batInsideDeactivationZone)
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

void PhantomBat::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                          const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    for (auto &hitbox : hitboxes)
    {
        if (playerIsAtacking && weaponBounds.findIntersection(hitbox).has_value())
        {
            std::cout << "Vida = " << life << std::endl;
            life -= playerDamage;
            std::cout << "Daño recibido = " << playerDamage << " | Vida = " << life << std::endl;
            if (life <= 0.0f)
            {
                isActive = false;
                resetPosition();
                std::cout << "Enemigo eliminado. Puntos = " << score << std::endl;
                break;
            }
        }
    }

    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            isActive = false;
            resetPosition();
            break;
        }
    }
}

void PhantomBat::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            resetPosition();
        }
    }
    else if (dynamic_cast<Player *>(&other))
    {
        isActive = false;
        resetPosition();
    }
}

void PhantomBat::resetPosition()
{
    Enemy::resetPosition();

    speed = PBAT_SPEED;
    life = PBAT_LIFE;

    spawnTime = 0.0f;
    animTimer = 0.0f;
    currentFrame = 0;
}

void PhantomBat::movePositionToBorder(const sf::FloatRect &playerActivationZone,
                               const sf::FloatRect &playerBounds)
{
    if (!sprite)
        return;

    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x;
    float playerCenterY = playerBounds.position.y + playerBounds.size.y / 2.0f;
    float alignedY = playerCenterY - 5.0f;

    sf::Vector2f oldPosition = sprite->getPosition();
    sf::Vector2f newPosition(rightEdgeX, alignedY);
    sprite->setPosition(newPosition);

    sf::Vector2f offset = newPosition - oldPosition;
    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }
}

void PhantomBat::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }

    if (gDrawHitboxes)
    {
        sf::RectangleShape zoneShape({spawnZone.size.x, spawnZone.size.y});
        zoneShape.setPosition({spawnZone.position.x, spawnZone.position.y});
        zoneShape.setFillColor(sf::Color(255, 255, 0, 50));
        zoneShape.setOutlineColor(sf::Color::Yellow);
        zoneShape.setOutlineThickness(1.0f);
        window.draw(zoneShape);
    }
}

void PhantomBat::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite)
        return;

    animTimer += deltaTime;

    if (animTimer >= ANIM_FRAME_TIME)
    {
        animTimer = 0.0f;
        currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
    }

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

void PhantomBat::hello() const
{
    std::cout << "Soy PhantomBat" << std::endl;
}