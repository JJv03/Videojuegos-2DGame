#pragma once

#include "enemy.h"

class Leopard : public Enemy
{
private:
    // Movement constants
    const sf::Vector2f LEOPARD_SPEED = {-125.0f, 0.0f};
    const float LEOPARD_JUMP_SPEED = 300.0f;

    // Combat stats
    const float LEOPARD_LIFE = 1.0f;
    const float LEOPARD_SCORE = 200.0f;
    const float LEOPARD_DAMAGE = 2.0f;

    // Animation settings
    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    // Animation tracking
    float animTimer = 0.0f;
    int currentFrame = 0;

    // Vision and AI
    const float VISION_RANGE = 75.0f;
    sf::FloatRect visionField;
    bool playerDetected = false;
    bool hasRedirected = false;
    sf::Vector2f playerPosition;

    // Navigation methods
    void jump(); // Jump action

public:
    int level; // Current game level
    int stage; // Current stage

    Leopard() = default;
    Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage);

    // Reset leopard to initial state
    void resetPosition() override;

    // Render leopard and debug info
    void draw(sf::RenderWindow &window) override;

    // Update leopard logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Update detection area
    void updateVisionField();

    void hello() const override;
};