#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "playerState.h"
#include "animationManager.h"
#include "globals.h"
#include "entity.h"
#include "item.h"

class PlayerState;
class Game;

enum PlayerDirection
{
    LEFT,
    RIGHT,
};

enum class PlayerPosition
{
    CENTER,
    LEFT,
    RIGHT,
};

typedef std::unique_ptr<PlayerState> PlayerStateRef;

class Whip : public EntitySprite
{
public:
    int whipState;
    int whipFrames;
    int whipLvl;
    int whipDmg;

    AnimationManager *animationManager{nullptr};

    std::vector<AnimationManager::Frame> noAttackFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f},
    };

    std::vector<AnimationManager::Frame> lvl1Frames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 477), sf::Vector2(8, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(10, 477), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 484), sf::Vector2(26, 10)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 484), sf::Vector2(26, 10)), 0.1f},
    };

    Whip();
    ~Whip() = default;

    // Entity functions
    std::vector<sf::FloatRect> getBounds() const override;
    void onCollision(Entity &other, Game &game) override;
    void hello() const override;
};

class SubWeapon : public EntitySprite
{
public:
    AnimationManager *animationManager{nullptr};

    std::vector<AnimationManager::Frame> daggerFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(354, 477), sf::Vector2(16, 16)), 0.1f},
    };

    std::vector<AnimationManager::Frame> axeFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(515, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(532, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(549, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(566, 477), sf::Vector2(16, 16)), 0.1f},
    };

    std::vector<AnimationManager::Frame> firebombFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(587, 477), sf::Vector2(8, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(596, 477), sf::Vector2(8, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(605, 477), sf::Vector2(8, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(614, 477), sf::Vector2(8, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(623, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(640, 477), sf::Vector2(16, 16)), 0.1f},
    };

    std::vector<AnimationManager::Frame> boomerangFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(401, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(418, 477), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(435, 477), sf::Vector2(16, 16)), 0.1f},
    };

    std::vector<AnimationManager::Frame> stopwatchFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 1), sf::Vector2(0, 0)), 0.1f},
    };

    SubWeapon();
    ~SubWeapon() = default;

    ItemType type;
    PlayerDirection direction;
    float verticalSpeed = 0.f;
    float horizontalSpeed = 0.f;
    // Entity functions
    std::vector<sf::FloatRect> getBounds() const override;
    void onCollision(Entity &other, Game &game) override;
    void hello() const override;
};

class Player : public EntitySprite
{
public:
    animationID currentAnimation;
    PlayerStateRef activeState;
    std::vector<SubWeapon> activeSubWeapons;
    PlayerDirection dir;

    bool isOnGround;
    bool isOnStairs;
    bool isAttacking;
    bool attackedFinished;
    bool isWalking;
    bool isJumping;
    bool isDucking;
    bool isDead;
    bool isBeingHurt;
    bool hasToPressAgain;
    bool isPressingUp;
    bool hasDied;

    // Secondary weapon
    bool weaponIsActive;

    // Stats
    float damage = 1.0f;
    int maxHealth;
    int health;
    int lives;
    int score;
    int hearts;

    // Interactions
    bool isInvulnerable;
    float invulnerableTime;
    bool isPassingObject;
    float invulnerableTimeCounter;
    float blinkTimer;
    bool visible;
    float blinkInterval;

    float verticalSpeed;
    float horizontalSpeed;

    sf::Clock revivingClock;
    bool deathRestart;

    // Weapons
    Whip whip;
    SubWeapon subWeapon;
    ItemType subWeaponType = ItemType::NONE;

    // Activation zone
    sf::FloatRect gPlayerActivationZone;
    sf::FloatRect gPlayerDeactivationZone;
    const float ACTIVATION_WIDTH = gWindowWidth;
    const float ACTIVATION_HEIGHT = gWindowHeight;
    const float DEACTIVATION_WIDTH = gWindowWidth * 1.2f;
    const float DEACTIVATION_HEIGHT = gWindowHeight * 1.2f;

    // Animations
    AnimationManager *animationManager{nullptr};

    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.2f}};

    std::vector<AnimationManager::Frame> hurtFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(275, 21), sf::Vector2(16, 32)), 0.2f}};

    std::vector<AnimationManager::Frame> jumpFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 24)), 0.1f}};

    std::vector<AnimationManager::Frame> duckFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 24)), 0.1f}};

    std::vector<AnimationManager::Frame> walkFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
    };

    std::vector<AnimationManager::Frame> walkSlowFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
    };

    std::vector<AnimationManager::Frame> attackFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(26, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.1f}};

    std::vector<AnimationManager::Frame> attackFloorFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 24)), 0.01f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(172, 78), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 24)), 0.1f}};

    std::vector<AnimationManager::Frame> deadFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(308, 21), sf::Vector2(16, 24)), 0.5f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(325, 21), sf::Vector2(32, 16)), 0.5f},

    };
    // --------------------------------

    Player();
    ~Player() = default;

    void handleInput(sf::Event event);
    void update(float deltaTime, const sf::Vector2f &viewPosition);
    void draw(sf::RenderWindow &window);

    // Finishes the current state, replaces it with <newState> and initializes this new state
    void setState(PlayerStateRef newState);

    PlayerStateRef &getActiveState();
    void updateActivationZones(const sf::Vector2f &viewPosition);
    PlayerPosition getPlayerOffsetPosition();
    bool onCollision_SolidTile(Entity &solidTile);
    void onCollision_DoorTile(int doorId, Game &game);
    void onCollision_Item(Entity &entityItem);

    // Entity functions
    std::vector<sf::FloatRect> getBounds() const override;
    void onCollision(Entity &other, Game &game) override;
    void hello() const override;

    // SubWeapon functions
    void updateActiveSubWeapons(float deltaTime);
};
