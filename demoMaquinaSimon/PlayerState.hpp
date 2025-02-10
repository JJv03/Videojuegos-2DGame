#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

enum PlayerDirection
{
    LEFT,
    RIGHT,
};

class PlayerState
{
public:
    PlayerState() {}
    virtual ~PlayerState() = default;

    PlayerDirection dir;

    virtual void init() = 0;
    virtual void handleInput(Player& player, sf::Event event) = 0;
    virtual void update(Player& player, float deltaTime) = 0;
    virtual void draw(Player& player, sf::RenderWindow &window) = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;
};
