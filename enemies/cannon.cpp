#include "cannon.h"
#include <iostream>

Cannon::Cannon(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage){

}

// Reset cannon to initial state
void Cannon::resetPosition(){

}

// Render cannon and debug info
void Cannon::draw(sf::RenderWindow &window){

}

// Update cannon logic (spawn, etc.)
void Cannon::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
            const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds){

}

// Handle collisions
void Cannon::onCollision(Entity &other, Game &game){

}

// Update animation frame
void Cannon::updateAnimation(float deltaTime){

}

// Update detection area
void Cannon::updateVisionField(){

}


void Cannon::hello() const {
    std::cout << "Soy Cannon" << std::endl;
}