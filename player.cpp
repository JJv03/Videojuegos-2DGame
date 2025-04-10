#include "player.h"
#include "globals.h"
#include <iostream>
#include "tile.h"
#include "enemies/enemy.h"

Player::Player()
{
    activeState = std::make_unique<PlayerIdleState>();
    dir = RIGHT;
    isOnGround = false;
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
    isPressingUp = false;
    died = false;

    // Stats
    health = 16;
    lives = 3;
    score = 0;
    hearts = 5;

    // Interactions
    isInvulnerable = false;
    invulnerableTime = 3.0f; // 3 segs
    invulnerableTimeCounter = 0.0f;
    isPassingObject = false;
    blinkTimer = 0.0f;
    blinkInterval = 0.05f; // 0.1 segs
    visible = true;
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
    //newState->hello();

    this->activeState->end(*this);
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

PlayerStateRef &Player::getActiveState()
{
    return activeState;
}

void Player::updateActivationZones() // AAAAAAAAAAAAAAAAAAAAAAAAAH WTF IS THIS
{
    sf::Vector2f playerPos = sprite->getPosition();

    sf::Vector2f windowSize = {static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight)};

    // Calcular las dimensiones de las zonas en función del tamaño de la ventana
    float activationWidth = static_cast<int>(windowSize.x);           // 100% del ancho de la ventana
    float activationHeight = static_cast<int>(windowSize.y);          // 100% de la altura de la ventana
    float deactivationWidth = static_cast<int>(windowSize.x * 1.4f);  // 140% del ancho de la ventana
    float deactivationHeight = static_cast<int>(windowSize.y * 1.4f); // 140% de la altura de la ventana

    gPlayerActivationZone = sf::FloatRect(
        {playerPos.x - activationWidth / 2.0f,
         playerPos.y - activationHeight / 2.0f},
        {activationWidth, activationHeight});

    gPlayerDeactivationZone = sf::FloatRect(
        {playerPos.x - deactivationWidth / 2.0f,
         playerPos.y - deactivationHeight / 2.0f},
        {deactivationWidth, deactivationHeight});
}


sf::FloatRect Player::getBounds() const {
    return animationManager->getGlobalBounds();
}

void Player::onCollision(Entity& other){
    if (SolidTile* player = dynamic_cast<SolidTile*>(&other)) {
        std::cout << "Es un SolidTile\n";
    }
    else if (Enemy* enemy = dynamic_cast<Enemy*>(&other)) {
        std::cout << "Es un Enemy\n";
    }
    else {
        std::cout << "Tipo desconocido\n";
    }
}

// ----------------------------- WHIP -----------------------------
Whip::Whip()
{
    whipState = 0;
    whipFrames = 6;
    whipDmg = 1;
    whipLvl = 1;
}

sf::FloatRect Whip::getBounds() const {
    return animationManager->getGlobalBounds();
}

void Whip::onCollision(Entity& other){

}

// ----------------------------- SUBWEAPON -----------------------------
SubWeapon::SubWeapon()
{
    type = KNIFE;
}

sf::FloatRect SubWeapon::getBounds() const {
    return sf::FloatRect();
}

void SubWeapon::onCollision(Entity& other){

}