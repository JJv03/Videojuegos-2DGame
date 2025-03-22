#pragma once

#include "enemy.h"

class Bat : public Enemy
{
private:
    const sf::Vector2f BAT_SPEED = {-75.0f, 0.0f};

    const float BAT_LIFE = 1.0f;
    const float BAT_SCORE = 100.0f;
    const float BAT_DAMAGE = 2.0f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Bat() = default;
    Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    static Bat createBat(const sf::Vector2f &position);

    void resetPosition() override;
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist);

    void update(float deltaTime);
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking, const float playerDamage);

    void updateAnimation(float deltaTime);
};