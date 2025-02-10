#include "Player.hpp"
#include "PlayerIdleState.hpp"


Player::Player() : sprite(texture)
{
    texture.loadFromFile("player.png");
    sprite.setTexture(texture);
    stateMachine.addState(std::make_unique<PlayerIdleState>());
}


void Player::handleInput(sf::Event event)
{
    stateMachine.getActiveState()->handleInput(*this, event);
}

void Player::update(float deltaTime)
{
    stateMachine.getActiveState()->update(*this, deltaTime);
}

void Player::draw(sf::RenderWindow &window)
{
    stateMachine.getActiveState()->draw(*this, window);
}


void Player::setState(StateRef newState)
{
    stateMachine.replaceState(std::move(newState));
}

void Player::addState(StateRef newState)
{
    stateMachine.replaceState(std::move(newState));
}

void Player::removeState()
{
    stateMachine.removeState();
}

StateRef& Player::getActiveState()
{
    return this->stateMachine.getActiveState();
}