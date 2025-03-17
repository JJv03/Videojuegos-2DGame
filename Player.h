#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "playerState.h"
#include "animationManager.h"

constexpr float gMovementSpeed { 75.0f };

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

    
    float verticalSpeed;
    float horizontalSpeed;


    // Whip 
    sf::Texture whipTexture;
    sf::Sprite* whipSprite;
    int whipState;
    int whipFrames;

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
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 477), sf::Vector2(26, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(27, 477), sf::Vector2(26, 16)), 0.1f},
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


    
};