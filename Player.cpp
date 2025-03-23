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
    updateActivationZones();
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

void Player::updateActivationZones()
{
    sf::Vector2f playerPos = sprite->getPosition();

    sf::Vector2u windowSize = {gWindowWidth, gWindowHeight};

    // Calcular las dimensiones de las zonas en función del tamaño de la ventana
    float activationWidth = windowSize.x; // 100% del ancho de la ventana
    float activationHeight = windowSize.y; // 100% de la altura de la ventana
    float deactivationWidth = windowSize.x * 1.2f; // 120% del ancho de la ventana
    float deactivationHeight = windowSize.y * 1.2f; // 120% de la altura de la ventana

    gPlayerActivationZone = sf::FloatRect(
        {playerPos.x - activationWidth / 2.0f,
         playerPos.y - activationHeight / 2.0f},
        {activationWidth, activationHeight});

    gPlayerDeactivationZone = sf::FloatRect(
        {playerPos.x - deactivationWidth / 2.0f,
         playerPos.y - deactivationHeight / 2.0f},
        {deactivationWidth, deactivationHeight});
}