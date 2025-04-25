#pragma once

#include "boss.h"

class Dracula : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f DRACULA_SPEED = {0.0f, 0.0f};
    const float DRACULA_LIFE = 24.0f;
    const float DRACULA_SCORE = 0.0f;
    const float DRACULA_DAMAGE = 4.0f;

public:
    int level; // Current game level
    int stage; // Current stage within level

    Dracula() = default;
    Dracula(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position, const int &level, const int &stage);

    // Render dracula and debug info
    void draw(sf::RenderWindow &window) override;

    // Update dracula logic (spawn, movement, etc.)
    void update(float deltaTime, const sf::FloatRect &playerActivationZone);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};
