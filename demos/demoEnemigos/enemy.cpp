#include "enemy.h"
#include <iostream>

Enemy::Enemy(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone, sf::FloatRect _deactivationZone)
    : Entity(_sprite, _hitboxes), activationZone(_activationZone), deactivationZone(_deactivationZone)
{
    // Guarda la posición inicial
    originalPosition = sprite->getPosition();
}

Enemy Enemy::createEnemy(const sf::Vector2f &position)
{
    // ESTO SERÁN CONSTANTES DE LA CLASE EN ESPECÍFICO DEL ENEMIGO
    const sf::IntRect spriteRegion = {{1, 28}, {16, 32}};
    const float hitboxWidth = 12.f;
    const float hitboxHeight = 30.f;
    const float activationWidth = 250.f;
    const float activationHeight = 500.f;
    const float deactivationWidth = 400.f;
    const float deactivationHeight = 500.f;

    // Crear la textura
    sf::Image enemyImage;
    if (!enemyImage.loadFromFile("../../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del enemigo" << std::endl;
        throw std::runtime_error("Error cargando la imagen del enemigo");
    }
    enemyImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *enemyTexture = new sf::Texture();
    if (!enemyTexture->loadFromImage(enemyImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete enemyTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto enemySprite = std::make_shared<sf::Sprite>(*enemyTexture);
    enemySprite->setTextureRect(spriteRegion);
    enemySprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = enemySprite->getLocalBounds();
    enemySprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (hitboxWidth / 2.f), position.y - hitboxHeight},
            {hitboxWidth, hitboxHeight}),
    };

    // Crear las zonas de activación y desactivación
    sf::FloatRect activationZone(
        {position.x - activationWidth / 2.f, position.y - activationHeight / 2.f},
        {activationWidth, activationHeight});

    sf::FloatRect deactivationZone(
        {position.x - deactivationWidth / 2.f, position.y - deactivationHeight / 2.f},
        {deactivationWidth, deactivationHeight});

    // Crear y retornar el enemigo
    return Enemy(enemySprite, hitboxes, activationZone, deactivationZone);
}

void Enemy::updateEnemyRespawn(float deltaTime, sf::Sprite *gSimonSprite)
{
    sf::FloatRect playerBounds = gSimonSprite->getGlobalBounds();

    bool playerInsideActivationZone = activationZone.findIntersection(playerBounds).has_value();
    bool playerInsideDeactivationZone = deactivationZone.findIntersection(playerBounds).has_value();

    if (playerInsideActivationZone && !playerWasNearActivation && !isActive)
    {
        isActive = true;
    }

    if (isActive && !playerInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }

    playerWasNearActivation = playerInsideActivationZone;
}

void Enemy::update(float deltaTime)
{
    if (isActive)
    {
        applyGravity(deltaTime);

        sf::Vector2f horizontalMovement = {speed.x * deltaTime, 0.f};
        sprite->move(horizontalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.x += horizontalMovement.x;
        }

        deactivationZone.position.x += horizontalMovement.x;
    }
}

void Enemy::applyGravity(float deltaTime)
{
    if (!isOnGround)
    {
        sf::Vector2f verticalMovement = {0.f, (VERTICAL_SPEED * deltaTime)};
        sprite->move(verticalMovement);

        for (auto &hitbox : hitboxes)
        {
            hitbox.position.y += verticalMovement.y;
        }

        deactivationZone.position.y += verticalMovement.y;
    }
}

void Enemy::checkCollisions(const std::vector<sf::FloatRect> &boundsList)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    for (auto &hitbox : hitboxes)
    {
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
                    deactivationZone.position.y -= overlapY;
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
                        deactivationZone.position.x -= overlapX;
                    }
                    else
                    {
                        // Colisión desde la derecha
                        sprite->move({overlapX, 0.f});
                        for (auto &h : hitboxes)
                            h.position.x += overlapX;
                        deactivationZone.position.x += overlapX;
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
                        deactivationZone.position.y -= overlapY;
                        isOnGround = true;
                    }
                    else
                    {
                        // Colisión desde abajo
                        sprite->move({0.f, overlapY});
                        for (auto &h : hitboxes)
                            h.position.y += overlapY;
                        deactivationZone.position.y += overlapY;
                    }
                }
            }
        }
    }
}

bool Enemy::checkHitByEnemy(const sf::FloatRect simonBounds)
{
    for (const auto &hitbox : hitboxes)
    {
        if (hitbox.findIntersection(simonBounds))
        {
            std::cout << "Contacto con enemigo" << std::endl;
            return true;
        }
    }
    return false;
}

void Enemy::resetPosition()
{
    sf::Vector2f offset = originalPosition - sprite->getPosition();

    sprite->setPosition(originalPosition);

    for (auto &hitbox : hitboxes)
    {
        hitbox.position += offset;
    }

    deactivationZone.position += offset;

    speed = ORIGINAL_SPEED;
}