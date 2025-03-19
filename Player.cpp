#include "player.h"
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
    whipState = 0;
    whipFrames = 6;
    whipDmg = 1;
    whipLvl = 1;

    // Secondary weapon
    secondaryWeapon = 0;

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
}

void Player::draw(sf::RenderWindow &window)
{
    getActiveState()->draw(*this, window);
}

void Player::setState(PlayerStateRef newState)
{
    // Cancelar animaciones etc automáticamente?
    //this->activeState->pause(*this);
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

/*
void Player::addState(PlayerStateRef newState)
{
    replaceState(std::move(newState));
}

void Player::removeState()
{
    stateMachine.removeState();
}
*/

PlayerStateRef& Player::getActiveState()
{
    return activeState;
}
