#include "leopard.h"
#include <iostream>
#include <cmath>

Leopard::Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Enemy(_sprite, _hitboxes)
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

Leopard Leopard::createLeopard(const sf::Vector2f &position)
{
    const sf::IntRect LEOPARD_SPRITE_REGION = {{56, 11}, {24, 16}};

    const float HITBOX_WIDTH = 22.f;
    const float HITBOX_HEIGHT = 13.f;

    // Crear la textura
    sf::Image leopardImage;
    if (!leopardImage.loadFromFile("../../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del leopard" << std::endl;
        throw std::runtime_error("Error cargando la imagen del leopard");
    }
    leopardImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *leopardTexture = new sf::Texture();
    if (!leopardTexture->loadFromImage(leopardImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete leopardTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto leopardSprite = std::make_shared<sf::Sprite>(*leopardTexture);
    leopardSprite->setTextureRect(LEOPARD_SPRITE_REGION);
    leopardSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = leopardSprite->getLocalBounds();
    leopardSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    // Crear y retornar el zombie
    return Leopard(leopardSprite, hitboxes);
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

bool Leopard::checkForLedge(const sf::FloatRect floorBounds)
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

    return !checkRect.findIntersection(floorBounds).has_value();
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
                              const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking, const float playerDamage)
{
    if (!isActive || !sprite)
        return;

    isOnGround = false;

    playerDetected = visionField.findIntersection(simonBounds).has_value();

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

        // Verificar si hay un precipicio adelante y saltar
        if (isOnGround && checkForLedge(boundsList[0]))
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

    // COLISIONES POR CONTACTO CON EL JUGADOR
    Enemy::checkHitByEnemy(simonBounds);
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