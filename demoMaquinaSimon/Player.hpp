#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "PlayerState.hpp"

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

    //sf::Texture texture;
    //sf::Sprite sprite;

    PlayerDirection dir;

    bool isOnGround;
    bool isOnStairs;

    bool isAttacking;
    bool isWalking;
    bool isJumping;
    bool isDucking;
    bool isDead;
    bool isBeingHurt;

// --------------------------------

    Player();

    void handleInput(sf::Event event);
    void update(float deltaTime); // Actualiza lógica
    void draw(sf::RenderWindow &window); // Renderiza

    void setState(StateRef newState);
    //void addState(StateRef newState);
    //void removeState();

    StateRef& getActiveState();
};