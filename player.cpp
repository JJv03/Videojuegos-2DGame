#include "player.h"
#include "globals.h"
#include <iostream>
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


std::vector<sf::FloatRect> Player::getBounds() const {
    return std::vector<sf::FloatRect>({animationManager->getGlobalBounds()});
}

void Player::onCollision(Entity& other){
    if (SolidTile* solidTile = dynamic_cast<SolidTile*>(&other)) {
        //std::cout << "Es un SolidTile\n";
        this->onCollision_SolidTile(other);
    }
    else if (Enemy* enemy = dynamic_cast<Enemy*>(&other)) {
        std::cout << "Es un Enemy\n";
    }
}

void Player::onCollision_SolidTile(Entity& solidTile){
    sf::FloatRect playerBounds = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    
    for (auto tileBound : tileBounds){
        if(const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBound)){
            const float overlapX = intersection->size.x;
            const float overlapY = intersection->size.y;
            //std::cout << "Overlap: " << overlapX << ", " << overlapY << std::endl;

            if (overlapX < overlapY)    // Horizontal collision
            { 
                if ((playerBounds.position.x + playerBounds.size.x * 0.5f) < (tileBound.position.x + tileBound.size.x * 0.5f))
                {
                    this->sprite->move({-overlapX, 0.f});
                    playerBounds.position.x -= overlapX;
                }
                else
                {
                    this->sprite->move({overlapX, 0.f});
                    playerBounds.position.x += overlapX;
                }
            }
            else    // Vertical collision
            { 
                if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBound.position.y + tileBound.size.y * 0.5f))
                { // Simon's feet are collisioning with the tile

                    if (!this->isOnGround && this->verticalSpeed >= 0.0f)
                    // if (player.verticalSpeed >= 0.0f)        // CUANDO ESTÉN TODO CON HITBOXES BUENAS
                    { // If player is NOT going up

                        // Option 1: adjust overlapedY and make it be .15f
                        // float theoreticallyCorrectPositionY = player.sprite->getPosition().y - overlapY;
                        // float targetPositionY = static_cast<int>(theoreticallyCorrectPositionY) + gSimonFeetCollisionNewHeight;
                        // float moveY = theoreticallyCorrectPositionY - targetPositionY;

                        // Option 2: read the tile and place the player on top of it, regardless of anything else
                        float targetPositionY = tileBound.position.y - playerBounds.size.y + gSimonFeetCollisionNewHeight;
                        float moveY = targetPositionY - playerBounds.position.y;

                        this->sprite->move({0.f, moveY});
                        playerBounds.position.y += moveY;
                        this->verticalSpeed = 0.0f; // (For security) Simon stops falling
                        this->isOnGround = true;    // Set Simon to be on ground
                    }
                }
                else // Simon's head is collisioning with the tile
                {
                    this->sprite->move({0.f, overlapY});
                    playerBounds.position.y += overlapY;
                    this->verticalSpeed = 0.0f; // (For security) Simon starts falling
                }
            }
        }
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

std::vector<sf::FloatRect> Whip::getBounds() const {
    return std::vector<sf::FloatRect>({animationManager->getGlobalBounds()});
}

void Whip::onCollision(Entity& other){

}

// ----------------------------- SUBWEAPON -----------------------------
SubWeapon::SubWeapon()
{
    type = SubWeaponType::DAGGER;
}

std::vector<sf::FloatRect> SubWeapon::getBounds() const {
    return std::vector<sf::FloatRect>();
}

void SubWeapon::onCollision(Entity& other){

}