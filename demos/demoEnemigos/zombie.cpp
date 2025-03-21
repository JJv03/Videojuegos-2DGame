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

Zombie Zombie::createZombie(const sf::Vector2f &position)
{
    // Crear la textura
    sf::Image zombieImage;
    if (!zombieImage.loadFromFile("../../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del zombie" << std::endl;
        throw std::runtime_error("Error cargando la imagen del zombie");
    }
    zombieImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *zombieTexture = new sf::Texture();
    if (!zombieTexture->loadFromImage(zombieImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete zombieTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto zombieSprite = std::make_shared<sf::Sprite>(*zombieTexture);
    zombieSprite->setTextureRect({{1, 28}, {16, 32}}); // Primera frame del zombie
    zombieSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = zombieSprite->getLocalBounds();
    zombieSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (12.f / 2.f), position.y - 30.f},
            {12.f, 30.f}),
    };

    // Crear y retornar el zombie
    return Zombie(zombieSprite, hitboxes);
}

void Zombie::update(float deltaTime)
{
    // EL RESPAWN SE DEBE GESTIONAR AQUI (SOLO EN LOS ZOMBIES SE TIENE UN SPAWNER)

    if (isActive)
    {
        applyGravity(deltaTime);

        sf::Vector2f horizontalMovement = {speed.x * deltaTime, 0.f};
        sprite->move(horizontalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.x += horizontalMovement.x;
        }

        updateAnimation(deltaTime);
    }
}

void Zombie::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                             const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    for (auto &hitbox : hitboxes)
    {
        // COLISIONES CON EL ENTORNO
        for (const auto &bounds : boundsList)
        {
            if (const std::optional<sf::FloatRect> intersection = hitbox.findIntersection(bounds))
            {
                float overlapX = intersection->size.x;
                float overlapY = intersection->size.y;

                if (&bounds == &boundsList[0]) // Suelo
                {
                    sprite->move({0.f, -overlapY});
                    for (auto &h : hitboxes)
                        h.position.y -= overlapY;
                    isOnGround = true;
                }
                else if (overlapX < overlapY) // Colisión horizontal
                {
                    if (hitbox.position.x < bounds.position.x + bounds.size.x / 2.f)
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
                    if (hitbox.position.y < bounds.position.y + bounds.size.y / 2.f)
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

    // COLISIONES POR CONTACTO CON EL JUGADOR
    Enemy::checkHitByEnemy(simonBounds);
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