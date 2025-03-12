#include "zombie.h"
#include <iostream>

Zombie::Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Enemy(_sprite, _hitboxes) {}

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
    if (isActive)
    {
        applyGravity(deltaTime);

        sf::Vector2f horizontalMovement = {getSpeed().x * deltaTime, 0.f};
        sprite->move(horizontalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.x += horizontalMovement.x;
        }

        updateAnimation(deltaTime);
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

        /*switch (currentFrame)
        {
        case 0:

            break;
        case 1:

            break;
        case 2:

            break;
        }*/
    }

    // Voltear el sprite según la dirección del movimiento
    sf::Vector2f currentSpeed = getSpeed();

    if (currentSpeed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}