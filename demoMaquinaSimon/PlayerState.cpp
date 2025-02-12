#include "PlayerState.hpp"

PlayerState::PlayerState(){}

// ---------------------------- IDLE ----------------------------

PlayerIdleState::PlayerIdleState() : PlayerState()
{

}

void PlayerIdleState::init(Player& player)
{
    if(player.isWalking){
        player.isWalking = false;
    }

    if(player.isDucking){
        player.isDucking = false;
    }
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
        if(player.dir == LEFT){
            player.dir = RIGHT;
        }

        player.setState(std::make_unique<PlayerWalkState>());
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
        if(player.dir == RIGHT){
            player.dir = LEFT;
        }

        player.setState(std::make_unique<PlayerWalkState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
        player.setState(std::make_unique<PlayerDuckState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::X)) {
        player.setState(std::make_unique<PlayerJumpState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z)) {
        player.setState(std::make_unique<PlayerAttackState>());
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

void PlayerIdleState::pause(Player& player)
{
    // Detener animaciones, sonidos, etc
}

void PlayerIdleState::start(Player& player)
{
    // Poner en marcha animaciones, sonidos, etc
}
// --------------------------------------------------------------


// ---------------------------- WALK ----------------------------

PlayerWalkState::PlayerWalkState() : PlayerState()
{

}

void PlayerWalkState::init(Player& player)
{
    if(!player.isWalking){
        player.isWalking = true;
    }
}

void PlayerWalkState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    const auto* keyReleased = event.getIf<sf::Event::KeyReleased>();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) && player.dir == LEFT) {
        player.dir = RIGHT;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) && player.dir == RIGHT) {
        player.dir = LEFT;
    }

    if (keyReleased->scancode == sf::Keyboard::Scancode::Right && player.dir == RIGHT || 
        keyReleased->scancode == sf::Keyboard::Scancode::Left && player.dir == LEFT) {
        player.setState(std::make_unique<PlayerIdleState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
        player.setState(std::make_unique<PlayerDuckState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::X)) {
        player.setState(std::make_unique<PlayerJumpState>());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z)) {
        player.setState(std::make_unique<PlayerAttackState>());
    }
}

void PlayerWalkState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerWalkState::pause(Player& player)
{
    // Detener animaciones, sonidos, etc
}

void PlayerWalkState::start(Player& player)
{
    // Poner en marcha animaciones, sonidos, etc
}

// --------------------------------------------------------------


// ---------------------------- JUMP ----------------------------

PlayerJumpState::PlayerJumpState() : PlayerState()
{

}

void PlayerJumpState::init(Player& player)
{
    if(player.isWalking){
        player.isWalking = false;
    }

    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerJumpState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    const auto* keyReleased = event.getIf<sf::Event::KeyReleased>();
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerJumpState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerJumpState::pause(Player& player)
{
    // Detener animaciones, sonidos, etc
}

void PlayerJumpState::start(Player& player)
{
    // Poner en marcha animaciones, sonidos, etc
}

// --------------------------------------------------------------


// ---------------------------- ATTACK ----------------------------

PlayerAttackState::PlayerAttackState() : PlayerState()
{

}

void PlayerAttackState::init(Player& player)
{
    if(player.isWalking){
        player.isWalking = false;
    }

    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerAttackState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    const auto* keyReleased = event.getIf<sf::Event::KeyReleased>();
}

void PlayerAttackState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerAttackState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerAttackState::pause(Player& player)
{
    // Detener animaciones, sonidos, etc
}

void PlayerAttackState::start(Player& player)
{
    // Poner en marcha animaciones, sonidos, etc
}

// --------------------------------------------------------------


// ---------------------------- DUCK ----------------------------

PlayerDuckState::PlayerDuckState() : PlayerState()
{

}

void PlayerDuckState::init(Player& player)
{
    if(player.isWalking){
        player.isWalking = false;
    }

    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerDuckState::handleInput(Player& player, sf::Event event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    const auto* keyReleased = event.getIf<sf::Event::KeyReleased>();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) && player.dir == LEFT) {
        player.dir = RIGHT;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) && player.dir == RIGHT) {
        player.dir = LEFT;
    }

    if (keyReleased->scancode == sf::Keyboard::Scancode::Down){
        player.setState(std::make_unique<PlayerIdleState>());
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z)) {
        player.setState(std::make_unique<PlayerAttackState>());
    }
}

void PlayerDuckState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, etc)
}

void PlayerDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

void PlayerDuckState::pause(Player& player)
{
    // Detener animaciones, sonidos, etc
}

void PlayerDuckState::start(Player& player)
{
    // Poner en marcha animaciones, sonidos, etc
}



