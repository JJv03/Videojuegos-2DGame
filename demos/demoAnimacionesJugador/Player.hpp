#pragma once
#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "PlayerState.hpp"
#include "animationManager.hpp"

constexpr float gMovementSpeed { 75.0f };

class PlayerState;

enum PlayerDirection
{
    LEFT,
    RIGHT,
};

typedef std::unique_ptr<PlayerState> StateRef;

class Player
{
public:
    StateRef activeState;

    sf::Texture texture;
    sf::Sprite* sprite;

    PlayerDirection dir;

    bool isOnGround;
    bool isOnStairs;

    bool isAttacking;
    bool isWalking;
    bool isJumping;
    bool isDucking;
    bool isDead;
    bool isBeingHurt;

    Player();

    void handleInput(sf::Event event);
    void update(float deltaTime); // Actualiza lógica
    void draw(sf::RenderWindow &window); // Renderiza

    void setState(StateRef newState);
    StateRef& getActiveState();

    void initAnimations();

    AnimationManager* animationManager { nullptr };
    animationID currentAnimation;
};