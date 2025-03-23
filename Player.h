#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "playerState.h"
#include "animationManager.h"
#include "globals.h"

constexpr float gMovementSpeed { 120.0f };

class PlayerState;

enum PlayerDirection
{
    LEFT,
    RIGHT,
};

typedef std::unique_ptr<PlayerState> PlayerStateRef;

class Player
{
public:
    animationID currentAnimation;
    PlayerStateRef activeState;
    sf::Texture texture;
    sf::Sprite* sprite;
    
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

    // Stats
    int health;
    int lives;
    int score;
    int hearts;

    //Interactions
    bool isInvulnerable;
    float invulnerableTime;
    bool isPassingObject;
    
    float verticalSpeed;
    float horizontalSpeed;
    
    // Whip 
    sf::Texture whipTexture;
    sf::Sprite* whipSprite;
    int whipState;
    int whipFrames;
    int whipLvl;
    int whipDmg;

    // Secondary weapon
    sf::Texture secondaryWeaponTexture;
    sf::Sprite* secondaryWeaponSprite;
    int secondaryWeapon; // 0 nada, 1 knife, 2 axe, 3 holy water, 4 cross, 5 stop watch

    // Activation zone
    sf::FloatRect gPlayerActivationZone;
    sf::FloatRect gPlayerDeactivationZone;
    const float ACTIVATION_WIDTH = gWindowWidth;
    const float ACTIVATION_HEIGHT = gWindowHeight;
    const float DEACTIVATION_WIDTH = gWindowWidth * 1.2f;
    const float DEACTIVATION_HEIGHT = gWindowHeight * 1.2f;

    AnimationManager* gAnimationManager { nullptr };
    AnimationManager* gWhipAnimationManager { nullptr };

    std::vector<AnimationManager::Frame> idleFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> hurtFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(275, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> jumpFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> duckFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };
    std::vector<AnimationManager::Frame> walkFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
    };

    std::vector<AnimationManager::Frame> walkSlowFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
    };

    std::vector<AnimationManager::Frame> attackFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(26, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)),  0.1f}
    };

    std::vector<AnimationManager::Frame> attackFloorFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.01f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(172, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> whipLvl1Frames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 477), sf::Vector2(8, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(10, 477), sf::Vector2(16, 24)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 484), sf::Vector2(26, 10)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 484), sf::Vector2(26, 10)), 0.1f},
    };
    

// --------------------------------

    Player();

    void handleInput(sf::Event event);
    void update(float deltaTime); // Actualiza lógica
    void draw(sf::RenderWindow &window); // Renderiza

    void setState(PlayerStateRef newState);
    //void addState(PlayerStateRef newState);
    //void removeState();

    PlayerStateRef& getActiveState();
    void updateActivationZones();

    
};