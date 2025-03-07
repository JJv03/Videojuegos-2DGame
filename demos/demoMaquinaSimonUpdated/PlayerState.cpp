#include "PlayerState.hpp"
#include <iostream>
using namespace std;

template <typename T>
std::unique_ptr<T> state() {
    return std::make_unique<T>();
}
constexpr float GRAVITY { 980.0f };       // aceleración en píxeles/segundo²
constexpr float JUMP_FORCE { 350.0f };    // velocidad inicial del salto (píxeles/segundo)

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
    player.isDucking = false;
    player.isJumping = false;
    player.isWalking = false;
    
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
    player.currentAnimation = idleSimon;
    
    /*
    if (player.gAnimationManager) {
        player.gAnimationManager->playAnimation(player.currentAnimation);
    } else {
        // Handle the error, e.g., log an error message or throw an exception
        std::cerr << "AnimationManager is null!" << std::endl;
    }
    
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
}

void PlayerIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
        player.sprite->setScale({-1.f, 1.f});
    } else {
        player.sprite->move({-1.f * deltaTime * gMovementSpeed, 0.f});
        player.sprite->setScale({1.f, 1.f});
    }
    player.currentAnimation = walkSimon;
    cout << "Walkiwng" << endl;
    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
}

// --------------------------------------------------------------


// ---------------------------- JUMP ----------------------------

PlayerJumpState::PlayerJumpState() : PlayerState()
{
    printf("Jumping\n");
}

void PlayerJumpState::init(Player& player)
{
    if(!player.isJumping && player.isOnGround){
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
        if (keyPressed->scancode == KEY_RIGHT) {
            player.dir = RIGHT;
            player.isWalking = true;
        }
        else if (keyPressed->scancode == KEY_LEFT) {
            player.dir = LEFT;
            player.isWalking = true;
            
        }
        else if (keyPressed->scancode == KEY_DOWN)
        {
            player.isDucking = true;
            player.setState(state<Duck>());
        }
        
    }
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = jumpSimon;
    if (!player.isOnGround)
    {
        player.verticalSpeed += GRAVITY * deltaTime;
        player.sprite->move({0.f, player.verticalSpeed * deltaTime});
    }
    
    if(player.isJumping && player.isOnGround){
        player.isOnGround = false;
        player.verticalSpeed = -JUMP_FORCE;
        player.setState(state<Idle>());
    }
    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
    
    
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerJumpState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
            player.attackedFinished = false;
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
    player.sprite->move({0.f, 3.0f});
    player.currentAnimation = duckSimon;

    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
}

void PlayerDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
    window.draw(*player.sprite);
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

    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
    
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerStairWalkState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
    player.attackedFinished = false;
}

void PlayerAttackIdleState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == KEY_ATTACK)
        {
            player.hasToPressAgain = true;
        }
   
    }
    
}

void PlayerAttackIdleState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
    player.currentAnimation = attackSimon;
    if (player.attackedFinished)
        {
            player.isAttacking = false;
            player.currentAnimation = idleSimon;
            player.setState(state<Idle>());
        }

    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
    
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
}

void PlayerAttackIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
    window.draw(*player.sprite);
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
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == KEY_ATTACK)
        {
            player.hasToPressAgain = true;
        }
   
    }
}

void PlayerAttackDuckState::update(Player& player, float deltaTime)
{
    
    player.currentAnimation = attackFloorSimon;
    if (player.attackedFinished)
        {
            player.isAttacking = false;
            player.currentAnimation = duckSimon;
            player.setState(state<Duck>());
        }
    /*
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
    
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
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
    window.draw(*player.sprite);
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
    window.draw(*player.sprite);
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
    window.draw(*player.sprite);
}

// --------------------------------------------------------------




