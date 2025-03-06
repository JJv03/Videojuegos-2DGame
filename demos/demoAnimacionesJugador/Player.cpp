#include "Player.hpp"
#include <iostream>

Player::Player() {
    activeState = std::make_unique<PlayerIdleState>();
    dir = RIGHT;

    isOnGround = true;
    isOnStairs = false;

    isAttacking = false;
    isWalking = false;
    isJumping = false;
    isDucking = false;
    isDead = false;
    isBeingHurt = false;

    animationManager = new AnimationManager(*sprite);
    initAnimations();
    std::cout << "Player created" << std::endl;
}

void Player::handleInput(sf::Event event) {
    getActiveState()->handleInput(*this, event);
}

void Player::update(float deltaTime) {
    getActiveState()->update(*this, deltaTime);
    animationManager->update(deltaTime);
}

void Player::draw(sf::RenderWindow &window) {
    getActiveState()->draw(*this, window);
}

void Player::setState(StateRef newState) {
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

StateRef& Player::getActiveState() {
    return activeState;
}

void Player::initAnimations() {
    std::vector<AnimationManager::Frame> idleFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> hurtFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(275, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> jumpFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> duckFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> walkFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
    };

    std::vector<AnimationManager::Frame> walkSlowFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
    };

    std::vector<AnimationManager::Frame> attackFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(26, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)),  0.1f}
    };

    std::vector<AnimationManager::Frame> attackFloorFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(172, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.1f}
    };

    animationManager->addAnimation(idleSimon, idleFrames);
    animationManager->addAnimation(jumpSimon, jumpFrames);
    animationManager->addAnimation(walkSimon, walkFrames);
    animationManager->addAnimation(walkSlowSimon, walkSlowFrames, false);
    animationManager->addAnimation(duckSimon, duckFrames);
    animationManager->addAnimation(attackSimon, attackFrames, false);
    animationManager->addAnimation(attackFloorSimon, attackFloorFrames, false);

    //animationManager->playAnimation(idleSimon);
    currentAnimation = idleSimon;

}