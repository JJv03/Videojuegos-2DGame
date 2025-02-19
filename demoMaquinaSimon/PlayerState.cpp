#include "PlayerState.hpp"

using namespace std;

template <typename T>
std::unique_ptr<T> state() {
    return std::make_unique<T>();
}

using Idle = PlayerIdleState;
using Walk = PlayerWalkState;
using Walk = PlayerWalkState;
using Jump = PlayerJumpState;
using Duck = PlayerDuckState;
using Stairs = PlayerStairState;
using StairsWalk = PlayerStairWalkState;
using AttackIdle = PlayerAttackIdleState;
using AttackJump = PlayerAttackJumpState;
using AttackDuck = PlayerAttackDuckState;
using AttackStairs = PlayerAttackStairState;

constexpr auto KEY_RIGHT = sf::Keyboard::Scancode::Right;
constexpr auto KEY_LEFT = sf::Keyboard::Scancode::Left;
constexpr auto KEY_DOWN = sf::Keyboard::Scancode::Down;
constexpr auto KEY_UP = sf::Keyboard::Scancode::Up;
constexpr auto KEY_JUMP = sf::Keyboard::Scancode::X;
constexpr auto KEY_ATTACK = sf::Keyboard::Scancode::Z;

PlayerState::PlayerState(){}

// ---------------------------- IDLE ----------------------------

PlayerIdleState::PlayerIdleState() : PlayerState()
{
    printf("Idle\n");
}

void PlayerIdleState::init(Player& player)
{
    if(player.isJumping){
        player.isJumping = false;
    }

    if(player.isWalking){
        player.isWalking = false;
    }

    if(player.isDucking){
        player.isDucking = false;
    }
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            if(player.dir == LEFT){
                player.dir = RIGHT;
            }

            player.isWalking = true;
            player.setState(state<Walk>());
        }
        
        if (keyPressed->scancode == KEY_LEFT) {
            if(player.dir == RIGHT){
                player.dir = LEFT;
            }
            
            player.isWalking = true;
            player.setState(state<Walk>());
        }
    
        if (keyPressed->scancode == KEY_DOWN) {
            player.isDucking = true;
            player.setState(state<Duck>());
        }
    
        if (keyPressed->scancode == KEY_JUMP) {
            player.isJumping = true;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.setState(state<AttackIdle>());
        }
    }
}

void PlayerIdleState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- WALK ----------------------------

PlayerWalkState::PlayerWalkState() : PlayerState()
{
    printf("Walking\n");
}

void PlayerWalkState::init(Player& player)
{
    if(!player.isWalking){
        player.isWalking = true;
    }
}

void PlayerWalkState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT && player.dir == LEFT) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == KEY_LEFT && player.dir == RIGHT) {
            player.dir = LEFT;
        }

        if (keyPressed->scancode == KEY_DOWN) {
            player.isWalking = false;
            player.isDucking = true;
            player.setState(state<Duck>());
        }
    
        if (keyPressed->scancode == KEY_JUMP) {
            player.isJumping = true;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == KEY_ATTACK) {
            player.isWalking = false;
            player.isAttacking = true;
            player.setState(state<AttackIdle>());
        }
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if ((keyReleased->scancode == KEY_RIGHT && player.dir == RIGHT) || 
            (keyReleased->scancode == KEY_LEFT && player.dir == LEFT)) {
            player.isWalking = false;
            player.setState(state<Idle>());
        }
    }
}

void PlayerWalkState::update(Player& player, float deltaTime)
{
    if(player.dir == RIGHT){
        player.sprite->move({1.f * deltaTime * gMovementSpeed, 0.f});
    } else {
        player.sprite->move({-1.f * deltaTime * gMovementSpeed, 0.f});
    }
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- JUMP ----------------------------

PlayerJumpState::PlayerJumpState() : PlayerState()
{
    printf("Jumping\n");
}

void PlayerJumpState::init(Player& player)
{
    if(!player.isJumping){
        player.isJumping = true;
    }
}

void PlayerJumpState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.setState(state<AttackJump>());
        }
    }
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    if(player.isOnGround){
        player.isJumping = false;
        player.setState(state<Idle>());
    }
    
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerJumpState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- DUCK ----------------------------

PlayerDuckState::PlayerDuckState() : PlayerState()
{
    printf("Ducking\n");
}

void PlayerDuckState::init(Player& player)
{
    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerDuckState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT && player.dir == LEFT) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == KEY_LEFT && player.dir == RIGHT) {
            player.dir = LEFT;
        }

        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.setState(state<AttackDuck>());
        }
          
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if (keyReleased->scancode == KEY_DOWN){
            player.isDucking = false;
            player.setState(state<Idle>());
        }          
    }    
}

void PlayerDuckState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------



// ---------------------------- STAIRS ----------------------------

PlayerStairState::PlayerStairState() : PlayerState()
{
    printf("Stairs\n");
}

void PlayerStairState::init(Player& player)
{
    if(!player.isOnStairs){
        player.isOnStairs = true;
    }
}

void PlayerStairState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {
            player.dir = RIGHT;
            player.isWalking = true;
            player.setState(state<StairsWalk>());
        }
        
        if (keyPressed->scancode == KEY_LEFT) {
            player.dir = LEFT;
            player.isWalking = true;
            player.setState(state<StairsWalk>());
        }

        // FALTA: Si es UP o DOWN, dir = LEFT o RIGHT dependiendo de la escalera (falta saber si es escalera dcha o izq)

        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.setState(state<AttackStairs>());
        }
          
    }
}

void PlayerStairState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerStairState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------



// ---------------------------- STAIRS-WALK ----------------------------

PlayerStairWalkState::PlayerStairWalkState() : PlayerState()
{
    printf("Ducking\n");
}

void PlayerStairWalkState::init(Player& player)
{
    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerStairWalkState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == KEY_LEFT) {
            player.dir = LEFT;
        }

        // FALTA: Si es UP o DOWN, dir = LEFT o RIGHT dependiendo de la escalera (falta saber si es escalera dcha o izq)

        if (keyPressed->scancode == KEY_ATTACK) {
            player.isWalking = false;
            player.isAttacking = true;
            player.setState(state<AttackStairs>());
        }
          
    }
}

void PlayerStairWalkState::update(Player& player, float deltaTime)
{
    if(!player.isOnStairs){
        player.setState(state<Idle>());

    } else if(!player.isWalking){
        player.setState(state<Stairs>());
    }
    
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerStairWalkState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------




// ---------------------------- ATTACK-IDLE ----------------------------

PlayerAttackIdleState::PlayerAttackIdleState() : PlayerState()
{
    printf("Attacking\n");
}

void PlayerAttackIdleState::init(Player& player)
{
    if(!player.isAttacking){
        player.isAttacking = true;
    }
}

void PlayerAttackIdleState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerAttackIdleState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------



// ---------------------------- ATTACK-JUMP ----------------------------

PlayerAttackJumpState::PlayerAttackJumpState() : PlayerState()
{
    printf("Attacking\n");
}

void PlayerAttackJumpState::init(Player& player)
{
    if(!player.isAttacking){
        player.isAttacking = true;
    }

    if(!player.isJumping){
        player.isJumping = true;
    }
}

void PlayerAttackJumpState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerAttackJumpState::update(Player& player, float deltaTime)
{
    if(player.isOnGround){
        player.isJumping = false;
        player.setState(state<AttackIdle>());
    }

    if(!player.isAttacking){
        player.setState(state<Jump>());
    }

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackJumpState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------



// ---------------------------- ATTACK-DUCK ----------------------------

PlayerAttackDuckState::PlayerAttackDuckState() : PlayerState()
{
    printf("Attacking\n");
}

void PlayerAttackDuckState::init(Player& player)
{
    if(!player.isAttacking){
        player.isAttacking = true;
    }

    if(!player.isDucking){
        player.isDucking = true;
    }
}

void PlayerAttackDuckState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerAttackDuckState::update(Player& player, float deltaTime)
{
    if(!player.isAttacking){
        player.setState(state<Duck>());
    }

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- ATTACK-STAIR ----------------------------

PlayerAttackStairState::PlayerAttackStairState() : PlayerState()
{
    printf("Attacking\n");
}

void PlayerAttackStairState::init(Player& player)
{
    if(!player.isAttacking){
        player.isAttacking = true;
    }

    if(!player.isOnStairs){
        player.isOnStairs = true;
    }
}

void PlayerAttackStairState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerAttackStairState::update(Player& player, float deltaTime)
{
    if(!player.isAttacking){
        player.setState(state<Stairs>());
    }

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackStairState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- HURT ----------------------------

PlayerHurtState::PlayerHurtState() : PlayerState()
{
    printf("Hurting\n");
}

void PlayerHurtState::init(Player& player)
{

}

void PlayerHurtState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerHurtState::update(Player& player, float deltaTime)
{
    if(!player.isBeingHurt){
        player.setState(state<Idle>());
    }
    // Se vuelve invencible por un tiempo

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerHurtState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------


// ---------------------------- DEAD ----------------------------

PlayerDeadState::PlayerDeadState() : PlayerState()
{
    printf("Hurting\n");
}

void PlayerDeadState::init(Player& player)
{

}

void PlayerDeadState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
}

void PlayerDeadState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerDeadState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
}

// --------------------------------------------------------------




