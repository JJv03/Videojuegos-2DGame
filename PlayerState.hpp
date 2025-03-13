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
};


// ---------------------------- STAIR ----------------------------
class PlayerStairState : public PlayerState
{
public:
PlayerStairState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};


// ---------------------------- STAIR-WALK ----------------------------
class PlayerStairWalkState : public PlayerState
{
public:
PlayerStairWalkState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};

// ---------------------------- ATTACK-IDLE ----------------------------
class PlayerAttackIdleState : public PlayerState
{
public:
PlayerAttackIdleState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};


// ---------------------------- ATTACK-JUMP ----------------------------
class PlayerAttackJumpState : public PlayerState
{
public:
PlayerAttackJumpState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};


// ---------------------------- ATTACK-DUCK ----------------------------
class PlayerAttackDuckState : public PlayerState
{
public:
PlayerAttackDuckState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};


// ---------------------------- ATTACK-STAIR ----------------------------
class PlayerAttackStairState : public PlayerState
{
public:
PlayerAttackStairState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};

// ---------------------------- HURT ----------------------------
class PlayerHurtState : public PlayerState
{
public:
PlayerHurtState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};

// ---------------------------- DEAD ----------------------------
class PlayerDeadState : public PlayerState
{
public:
PlayerDeadState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};

// ---------------------------- INVULNERABLE ----------------------------
class PlayerInvulnerableState : public PlayerState
{
public:
PlayerInvulnerableState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
};



