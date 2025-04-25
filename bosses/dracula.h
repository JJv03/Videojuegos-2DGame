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

    const float asleepTime = 100.f;
    float asleepTimeCounter = 0.f;

    const float maskMaxHeight = 100.f;

    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };
    std::vector<AnimationManager::Frame> maskDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(307, 74), sf::Vector2(8, 16)), 0.1f}
    };
    std::vector<AnimationManager::Frame> idleDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(325, 74), sf::Vector2(24, 59)), 0.1f}
    };
    std::vector<AnimationManager::Frame> attackDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(350, 74), sf::Vector2(32, 59)), 0.1f}
    };

public:
    int level; // Current game level
    int stage; // Current stage within level

    enum class DraculaState
    {
        ASLEEP,
        MASK_APPEAR,
        MASK_ELEVATE,
        BATTLE_IDLE,
        BATTLE_APPEAR,
        BATTLE_ATTACK,
        BATTLE_DISAPPEAR,
        DEAD_MASK_OFF   
    };

    DraculaState currentState = DraculaState::ASLEEP;

    Dracula() = default;
    Dracula(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position, const int &level, const int &stage);

    // Render dracula and debug info
    void draw(sf::RenderWindow &window) override;

    // Update dracula logic (spawn, movement, etc.)
    void update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    void hello() const override;
};
