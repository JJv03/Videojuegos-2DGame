#pragma once

#include "enemy.h"

class Zombie : public Enemy
{
private:
    // Movement constant
    const sf::Vector2f ZOMBIE_SPEED = {-60.0f, 0.0f};

    // Combat stats
    const float ZOMBIE_LIFE = 1.0f;
    const float ZOMBIE_SCORE = 100.0f;
    const float ZOMBIE_DAMAGE = 2.0f;

    // Animation settings
    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    // Animation tracking
    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Zombie() = default;
    Zombie(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    // Reset zombie to initial state
    void resetPosition() override;

    // Position for spawning
    void movePositionToBorder(const sf::FloatRect &playerActivationZone, const float dist);

    // Main update loop
    void update(float deltaTime);

    // Handle collisions
    void checkCollisions(const sf::FloatRect &weaponBounds, const TileMap &tileMap, const bool playerIsAtacking, const float playerDamage);
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};