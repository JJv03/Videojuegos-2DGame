#pragma once

#include "enemy.h"

class Ghost : public Enemy
{
private:
    // Movement constants
    const sf::Vector2f GHOST_SPEED = {-30.0f, 0.0f};
    const float GHOST_SPEED_MAGNITUDE = 30.0f;

    // Combat stats
    const float GHOST_LIFE = 5.0f;
    const float GHOST_SCORE = 300.0f;
    const float GHOST_DAMAGE = 2.0f;

    // AI
    sf::Vector2f playerPosition;
    float stunTime = 0.0f;
    const float STUN_DURATION = 0.05f;
    const float MIN_PLAYER_DISTANCE_TO_SPAWN = 90.0f;

public:
    int level; // Current game level
    int stage; // Current stage

    Ghost() = default;
    Ghost(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset ghost to initial state
    void resetPosition() override;

    // Render ghost and debug info
    void draw(sf::RenderWindow &window) override;

    // Update ghost logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};