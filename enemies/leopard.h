#pragma once

#include "enemy.h"
#include "../tilemap.h"

class Leopard : public Enemy
{
private:
    const sf::Vector2f LEOPARD_SPEED = {-125.0f, 0.0f};
    const float LEOPARD_JUMP_SPEED = 300.0f;

    const float LEOPARD_LIFE = 1.0f;
    const float LEOPARD_SCORE = 200.0f;
    const float LEOPARD_DAMAGE = 2.0f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    float animTimer = 0.0f;
    int currentFrame = 0;

    // Campo de visión
    const float VISION_RANGE = 80.0f;
    sf::FloatRect visionField;
    bool playerDetected = false;
    bool hasRedirected = false;
    sf::Vector2f playerPosition;

    // Detección de precipicios
    bool checkForLedge(const TileMap &tileMap);
    void jump();

public:
    size_t level;
    size_t stage;

    Leopard() = default;
    Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const size_t &level, const size_t &stage);

    void resetPosition() override;
    void draw(sf::RenderWindow &window, bool debugDraw) override;

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone, const sf::Vector2f &playerPos);
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);
    void updateAnimation(float deltaTime);
    void updateVisionField();
};