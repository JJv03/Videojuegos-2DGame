#pragma once

#include "enemy.h"

class Zombie : public Enemy
{
private:
    const sf::IntRect ZOMBIE_SPRITE_REGION = {{1, 28}, {16, 32}};
    const sf::Vector2f ZOMBIE_SPEED = {-75.0f, 0.0f};
    const float HITBOX_WIDTH = 12.f;
    const float HITBOX_HEIGHT = 30.f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Zombie() = default;
    Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    static Zombie createZombie(const sf::Vector2f &position);

    void update(float deltaTime) override;
    void checkCollisions(const std::vector<sf::FloatRect> &boundsList) override;
    void resetPosition() override;

    void updateAnimation(float deltaTime);
};