#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "player.h"
#include "configManager.h"

class Player;

// ---------------------------- BASE (virtual) ----------------------------
class PlayerState
{
public:
    PlayerState();
    virtual ~PlayerState() = default;

    // Method that is called when a new state is created
    virtual void init(Player& player) = 0;

    // Method that manages the user's input events
    virtual void handleInput(Player& player, sf::Event event) = 0;

    // Method that manages the player's logic
    virtual void update(Player& player, float deltaTime) = 0;

    // Method that displays the player's graphics onto the window
    virtual void draw(Player& player, sf::RenderWindow &window) = 0;

    // Method that is called when a state is finished
    virtual void end(Player& player) = 0;

    // Debugging method that indicates the current state throw standard output
    virtual void hello() = 0;

    configManager &configManager;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
};

// ---------------------------- AUTO-WALK ----------------------------
class PlayerAutoWalkState : public PlayerState
{
public:
    PlayerAutoWalkState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
};


// ---------------------------- STAIR ----------------------------
class PlayerStairIdleState : public PlayerState
{
public:
PlayerStairIdleState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
};

// ---------------------------- HURT STAIR ----------------------------
class PlayerHurtStairState : public PlayerState
{
public:
    PlayerHurtStairState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
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
    void end(Player& player) override;
    void hello() override;
};


// ---------------------------- ATTACK SECONDARY ----------------------------
class PlayerAttackSecondaryState : public PlayerState
{
public:
PlayerAttackSecondaryState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
};

// ---------------------------- ATTACK JUMP SECONDARY ----------------------------
class PlayerAttackJumpSecondaryState : public PlayerState
{
public:
PlayerAttackJumpSecondaryState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
};

// ---------------------------- WHIP UPGRADE ANIMATION ----------------------------
class PlayerWhipUpgradeState : public PlayerState
{
public:
PlayerWhipUpgradeState();

    void init(Player& player) override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;
    void end(Player& player) override;
    void hello() override;
};

