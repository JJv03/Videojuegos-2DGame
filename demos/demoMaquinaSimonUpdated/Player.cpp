#include "Player.hpp"



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

void Player::setState(StateRef newState)
{
    // Cancelar animaciones etc automáticamente?
    //this->activeState->pause(*this);
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

/*
void Player::addState(StateRef newState)
{
    replaceState(std::move(newState));
}

void Player::removeState()
{
    stateMachine.removeState();
}
*/

StateRef& Player::getActiveState()
{
    return activeState;
}