#include "zombie.h"
#include <iostream>

Zombie::Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Enemy(_sprite, _hitboxes)
{
    speed = ZOMBIE_SPEED;
    life = ZOMBIE_LIFE;
    score = ZOMBIE_SCORE;
    damage = ZOMBIE_DAMAGE;
}

void Zombie::update(float deltaTime)
{
    // EL RESPAWN SE DEBE GESTIONAR AQUI (SOLO EN LOS ZOMBIES SE TIENE UN SPAWNER)

    if (isActive)
    {
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

int Zombie::checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return 0;
    int score2Return = 0;
    isOnGround = false;

    for (auto &hitbox : hitboxes)
    {
        // COLISIONES CON EL ENTORNO
        checkBasicCollisions(tileMap, hitbox);

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
                    score2Return = score;
                    break;
                }
            }
        }
    }
    return score2Return;
}

void Zombie::resetPosition()
{
    Enemy::resetPosition();

    speed = ZOMBIE_SPEED;
    life = ZOMBIE_LIFE;

    animTimer = 0.0f;
    currentFrame = 0;
}

void Zombie::movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist)
{
    if (!sprite)
        return;

    // Borde derecho de la zona de activación
    float rightEdgeX = playerActivationZone.position.x + playerActivationZone.size.x + dist + 20.0f;

    // Altura original del zombie
    float originalY = sprite->getPosition().y;

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

void Zombie::updateAnimation(float deltaTime)
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