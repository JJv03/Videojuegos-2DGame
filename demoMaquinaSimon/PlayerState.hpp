#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"




// ---------------------------- BASE (virtual) ----------------------------
class PlayerState
{
public:
    PlayerState() {}
    virtual ~PlayerState() = default;


    virtual void init(Player& player) = 0;
    virtual void handleInput(Player& player, sf::Event event) = 0;
    virtual void update(Player& player, float deltaTime) = 0;
    virtual void draw(Player& player, sf::RenderWindow &window) = 0;

    virtual void pause(Player& player) = 0;
    virtual void resume(Player& player) = 0;
};

// ---------------------------- IDLE ----------------------------
class PlayerIdleState : public PlayerState
{
    PlayerIdleState() {}

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void resume(Player& player) override;
};

// ---------------------------- WALK ----------------------------
class PlayerWalkState : public PlayerState
{
    PlayerWalkState() {}

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void resume(Player& player) override;
};

// ---------------------------- JUMP ----------------------------
class PlayerJumpState : public PlayerState
{
    PlayerJumpState() {}

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void resume(Player& player) override;
};

// ---------------------------- ATTACK ----------------------------
class PlayerAttackState : public PlayerState
{
    PlayerAttackState() {}

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void resume(Player& player) override;
};


// ---------------------------- DUCK ----------------------------
class PlayerDuckState : public PlayerState
{
    PlayerDuckState() {}

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause(Player& player) override;
    void resume(Player& player) override;
};

