#pragma once

#include "boss.h"

class DraculaBody : public EntitySprite {
public:
    std::vector<AnimationManager::Frame> idleDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(249, 74), sf::Vector2(24, 48)), 0.1f}
    };
    std::vector<AnimationManager::Frame> attackDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(274, 74), sf::Vector2(32, 48)), 0.1f}
    };

    DraculaBody() = default;
    DraculaBody(std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes);


    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    std::vector<sf::FloatRect> getBounds() const override;

    void hello() const override;
};


class Dracula : public Boss
{
private:
    // Movement and combat constants
    const sf::Vector2f DRACULA_SPEED = {0.0f, 0.0f};
    const float DRACULA_LIFE = 24.0f;
    const float DRACULA_SCORE = 0.0f;
    const float DRACULA_DAMAGE = 4.0f;

    const float ASLEEP_TIME = 2.f;
    float asleepTimeCounter;

    const float MASK_MAX_HEIGHT = 100.f;

    std::vector<AnimationManager::Frame> noAnimationFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f}
    };
    std::vector<AnimationManager::Frame> maskDraculaFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(307, 74), sf::Vector2(8, 16)), 0.1f}
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
    DraculaBody* draculaBody;
    sf::Vector2f draculaBodyPosition;

    Dracula() = default;
    Dracula(std::shared_ptr<sf::Sprite> _maskSprite, std::vector<sf::FloatRect> &_maskHitboxes,
            std::shared_ptr<sf::Sprite> _draculaSprite, std::vector<sf::FloatRect> &_draculaHitboxes, const int &level, const int &stage);

    // Render dracula mask and debug info
    void draw(sf::RenderWindow &window) override;

    // Render dracula body and debug info
    void drawBody(sf::RenderWindow &window);

    // Update dracula logic (spawn, movement, etc.)
    void update(float deltaTime, const int phase, const Player &player, const sf::FloatRect &mapBounds);

    // Handle collisions
    void onCollision(Entity &other, Game &game) override;

    // Update animation frame
    void updateAnimation(float deltaTime);

    // Apply animation to mask and body
    void applyMaskBodyAnimation(animationID id);

    // Apply flying mask animation
    void deadMaskAnimation();

    std::vector<sf::FloatRect> getBounds() const override;

    void hello() const override;
};
