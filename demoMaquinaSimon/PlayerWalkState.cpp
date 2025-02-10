#include "PlayerWalkState.hpp"

void init(){
    
}

void PlayerWalkState::handleInput(Player& player, sf::Event event){
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
        //player.setState(new PlayerWalkState());
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::X)) {
        //player.setState(new PlayerJumpState());
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z)) {
        //player.setState(new PlayerAttackState());
    }

    else {
        //player.setState(new PlayerAttackState());
    }
}

void PlayerWalkState::update(Player& player, float deltaTime){
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window){
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerWalkState::pause(){
    // Detener animaciones, sonidos, etc
}

void PlayerWalkState::resume(){
    // Reanudar animaciones, sonidos, etc
}



