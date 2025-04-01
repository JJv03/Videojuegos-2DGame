#include "player.h"
#include "globals.h"
#include <iostream>


Player::Player(){
    activeState = std::make_unique<PlayerIdleState>();
    dir = RIGHT;
    isOnGround = true;
    isOnStairs = false;
    hasToPressAgain = true;
    isAttacking = false;
    isWalking = false;
    isJumping = false;
    isDucking = false;
    isDead = false;
    isBeingHurt = false;
    attackedFinished = false;
    verticalSpeed = 0.0f;
    horizontalSpeed = 0.0f;

    //Stats
    health=16;
    lives=3;
    score=0;
    hearts=5;

    //Interactions
    isInvulnerable = false;
    invulnerableTime = 3.0f; //3 segs
    isPassingObject = false;
}

void Player::handleInput(sf::Event event)
{
    getActiveState()->handleInput(*this, event);
}

void Player::update(float deltaTime)
{
    getActiveState()->update(*this, deltaTime);
    updateActivationZones();
}

void Player::draw(sf::RenderWindow &window)
{
    getActiveState()->draw(*this, window);
}

void Player::setState(PlayerStateRef newState)
{
    this->activeState->end(*this);
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

PlayerStateRef& Player::getActiveState()
{
    return activeState;
}

void Player::updateActivationZones()        // AAAAAAAAAAAAAAAAAAAAAAAAAH WTF IS THIS
{
    sf::Vector2f playerPos = sprite->getPosition();

    sf::Vector2f windowSize = {static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight)};

    // Calcular las dimensiones de las zonas en función del tamaño de la ventana
    float activationWidth = static_cast<int>(windowSize.x); // 100% del ancho de la ventana
    float activationHeight = static_cast<int>(windowSize.y); // 100% de la altura de la ventana
    float deactivationWidth = static_cast<int>(windowSize.x * 1.2f); // 120% del ancho de la ventana
    float deactivationHeight = static_cast<int>(windowSize.y * 1.2f); // 120% de la altura de la ventana

    gPlayerActivationZone = sf::FloatRect(
        {playerPos.x - activationWidth / 2.0f,
         playerPos.y - activationHeight / 2.0f},
        {activationWidth, activationHeight});

    gPlayerDeactivationZone = sf::FloatRect(
        {playerPos.x - deactivationWidth / 2.0f,
         playerPos.y - deactivationHeight / 2.0f},
        {deactivationWidth, deactivationHeight});
}



// ----------------------------- WHIP -----------------------------
Whip::Whip(){
    whipState = 0;
    whipFrames = 6;
    whipDmg = 1;
    whipLvl = 1;
}

// ----------------------------- SUBWEAPON -----------------------------
SubWeapon::SubWeapon(){
    type = NONE;
}