#include "bat.h"
#include <iostream>

Bat::Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Enemy(_sprite, _hitboxes)
{
    speed = BAT_SPEED;
    life = BAT_LIFE;
    score = BAT_SCORE;
    damage = BAT_DAMAGE;
}

Bat Bat::createBat(const sf::Vector2f &position)
{
    const sf::IntRect BAT_SPRITE_REGION = {{184, 11}, {16, 16}};

    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Crear la textura
    sf::Image batImage;
    if (!batImage.loadFromFile("../../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del bat" << std::endl;
        throw std::runtime_error("Error cargando la imagen del bat");
    }
    batImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *batTexture = new sf::Texture();
    if (!batTexture->loadFromImage(batImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete batTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto batSprite = std::make_shared<sf::Sprite>(*batTexture);
    batSprite->setTextureRect(BAT_SPRITE_REGION);
    batSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = batSprite->getLocalBounds();
    batSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    // Crear y retornar el bat
    return Bat(batSprite, hitboxes);
}

void Bat::update(float deltaTime)
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

void Bat::checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
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

void Bat::resetPosition()
{
    Enemy::resetPosition();

    speed = BAT_SPEED;
    life = BAT_LIFE;

    animTimer = 0.0f;
    currentFrame = 0;
}

void Bat::movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist)
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