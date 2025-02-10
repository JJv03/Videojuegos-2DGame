#include "PlayerIdleState.hpp"

PlayerIdleState::PlayerIdleState() : PlayerState()
{

}

void PlayerIdleState::init()
{
    
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) && dir == LEFT) {
        dir = RIGHT;
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) && dir == RIGHT) {
        dir = LEFT;
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::X)) {
        //player.setState(new PlayerJumpState());
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z)) {
        //player.setState(new PlayerAttackState());
    }

    else
    {
        player.setState(std::make_unique<PlayerIdleState>());
    }
}

void PlayerIdleState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerIdleState::pause()
{
    // Detener animaciones, sonidos, etc
}

void PlayerIdleState::resume()
{
    // Reanudar animaciones, sonidos, etc
}



