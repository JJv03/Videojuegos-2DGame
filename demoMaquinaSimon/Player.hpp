#pragma once
#include "PlayerStateMachine.hpp"

enum PlayerDirection
{
    LEFT,
    RIGHT,
};


class Player
{
protected:
    PlayerStateMachine stateMachine;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    PlayerDirection dir;
    bool isWalking;
    bool isDucking;

    Player();

    void handleInput(sf::Event event);
    void update(float deltaTime); // Actualiza lógica
    void draw(sf::RenderWindow &window); // Renderiza

    void setState(StateRef newState);
    void addState(StateRef newState);
    void removeState();

    StateRef& getActiveState();
};