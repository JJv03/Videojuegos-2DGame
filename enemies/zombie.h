#pragma once

#include "enemy.h"

class Zombie : public Enemy
{
private:
    const sf::Vector2f ZOMBIE_SPEED = {-60.0f, 0.0f};

    const float ZOMBIE_LIFE = 1.0f;
    const float ZOMBIE_SCORE = 100.0f;
    const float ZOMBIE_DAMAGE = 2.0f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Zombie() = default;
    Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    void resetPosition() override;
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist);

    void update(float deltaTime);
    int checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);

    void updateAnimation(float deltaTime);
};