#pragma once

#include "enemy.h"

class Leopard : public Enemy
{
private:
    const sf::Vector2f LEOPARD_SPEED = {-125.0f, 0.0f};

    const float LEOPARD_LIFE = 1.0f;
    const float LEOPARD_SCORE = 200.0f;
    const float LEOPARD_DAMAGE = 2.0f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Leopard() = default;
    Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    static Leopard createLeopard(const sf::Vector2f &position);

    void resetPosition() override;
    void draw(sf::RenderWindow &window, bool debugDraw) override;

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking, const float playerDamage);

    void updateAnimation(float deltaTime);
};