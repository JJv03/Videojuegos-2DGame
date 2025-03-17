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

    gAnimationManager = new AnimationManager(*sprite);
    currentAnimation = idleSimon;

    gAnimationManager->addAnimation(idleSimon, idleFrames);
    gAnimationManager->addAnimation(jumpSimon, jumpFrames);
    gAnimationManager->addAnimation(walkSimon, walkFrames);
    gAnimationManager->addAnimation(walkSlowSimon, walkSlowFrames, false);
    gAnimationManager->addAnimation(duckSimon, duckFrames);
    gAnimationManager->addAnimation(attackSimon, attackFrames, false);
    gAnimationManager->addAnimation(attackFloorSimon, attackFloorFrames, false);

    //animationManager->playAnimation(idleSimon);
    currentAnimation = idleSimon;

    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    simonImage.createMaskFromColor(sf::Color(0, 128, 0)); // color key
    texture = sf::Texture(simonImage, false);

    sprite = new sf::Sprite(texture);
    sprite->setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    sprite->setPosition({245.f, 171.f});
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
