#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Player.hpp"

class Player;

// ---------------------------- BASE (virtual) ----------------------------
class PlayerState
{
public:
    PlayerState();
    virtual ~PlayerState() = default;


    virtual void init(Player& player) = 0;
    virtual void handleInput(Player& player, sf::Event event) = 0;
    virtual void update(Player& player, float deltaTime) = 0;
    virtual void draw(Player& player, sf::RenderWindow &window) = 0;

    virtual void pause(Player& player) = 0;
    virtual void start(Player& player) = 0;
};

// ---------------------------- IDLE ----------------------------
class PlayerIdleState : public PlayerState
{
public:
    PlayerIdleState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void start(Player& player) override;
};

// ---------------------------- WALK ----------------------------
class PlayerWalkState : public PlayerState
{
public:
    PlayerWalkState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void start(Player& player) override;
};

// ---------------------------- JUMP ----------------------------
class PlayerJumpState : public PlayerState
{
public:
    PlayerJumpState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void start(Player& player) override;
};

// ---------------------------- ATTACK ----------------------------
class PlayerAttackState : public PlayerState
{
public:
    PlayerAttackState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void start(Player& player) override;
};


// ---------------------------- DUCK ----------------------------
class PlayerDuckState : public PlayerState
{
public:
    PlayerDuckState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void start(Player& player) override;
};

