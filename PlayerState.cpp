#include "playerState.h"
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
    printf("init");
    player.isDucking = false;
    player.isJumping = false;
    player.isWalking = false;
    player.isAttacking = false;
    player.isOnStairs = false;
    player.isOnGround = true;
    player.hasToPressAgain = true;
    player.attackedFinished = false;
    player.isBeingHurt = false;
    player.isDead = false;    
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            player.dir = RIGHT;
            player.isWalking = true;
            player.setState(state<Walk>());
        }
        
        if (keyPressed->scancode == KEY_LEFT) {
            player.dir = LEFT;
            player.isWalking = true;
            player.setState(state<Walk>());
        }
    
        if (keyPressed->scancode == KEY_DOWN) {
            player.isDucking = true;
            player.setState(state<Duck>());
        }
    
        if (keyPressed->scancode == KEY_JUMP) {
            player.isJumping = true;
            player.verticalSpeed = -JUMP_FORCE;
            player.isOnGround = false;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == KEY_ATTACK && player.hasToPressAgain) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
            player.setState(state<AttackIdle>());
        }
    }
}

void PlayerIdleState::update(Player& player, float deltaTime)
{
    
    // Implementar logica (actualizar fisicas, animaciones, etc)
    player.currentAnimation = idleSimon;
    
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    
    player.gAnimationManager->update(deltaTime);
}

void PlayerIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    if (player.dir == RIGHT)
    {
        player.sprite->setScale({-1.f, 1.f});
    }
    else
    {
        player.sprite->setScale({1.f, 1.f});
    }
    
    
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
            //player.wasWalking = true;
            player.isOnGround = false;
            player.verticalSpeed = -JUMP_FORCE;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == KEY_ATTACK && player.hasToPressAgain) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
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
        player.sprite->move({0.5f * deltaTime * gMovementSpeed, 0.f});
        player.sprite->setScale({-1.f, 1.f});
    } else {
        player.sprite->move({-0.5f * deltaTime * gMovementSpeed, 0.f});
        player.sprite->setScale({1.f, 1.f});
    }
    player.currentAnimation = walkSimon;
    
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    
    if (player.isAttacking && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }
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
    if (player.isWalking) {
        player.horizontalSpeed = (player.dir == RIGHT) ? gMovementSpeed : -gMovementSpeed;
    } else {
        player.horizontalSpeed = 0.0f; // No horizontal movement if not walking
    }
}

void PlayerJumpState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.attackedFinished = false;
            player.setState(state<AttackJump>());
        }

    }
    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (keyReleased->scancode == KEY_LEFT || keyReleased->scancode == KEY_RIGHT)
        {
            player.horizontalSpeed = 0.0f;
            player.isWalking = false;
        }
        
    }
    

    
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = jumpSimon;
    bool wasWalking;
    // Apply gravity
    player.verticalSpeed += GRAVITY * deltaTime* 1.2f;
    
    // Move the player vertically
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    // If Simon was walking before jumping, move in the x direction
    if (player.isWalking)
    {
        wasWalking = true;
        player.horizontalSpeed = 50.0f ;  // Adjust as needed
        if (player.dir == RIGHT)
        {
            player.sprite->move({player.horizontalSpeed* deltaTime, 0.f});
        }
        else
        {
            player.horizontalSpeed = -player.horizontalSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime , 0.f});
        }
    }

    // Play jump animation if not already playing
    if (!player.gAnimationManager->isPlaying(player.currentAnimation))
    {
        player.gAnimationManager->playAnimation(player.currentAnimation);
    }
    player.gAnimationManager->update(deltaTime);

    // Check if landed
    if (player.isOnGround)
    {
        player.isJumping = false;
        if (wasWalking)
        {
            wasWalking = false;
            player.setState(state<Walk>());
        }else{
            player.setState(state<Idle>());
        }
        
        
    }
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

    
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
        
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);

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
    player.gWhipAnimationManager->playAnimation(whipLvl1StandingJumping);
    player.gAnimationManager->playAnimation(attackSimon);
    player.isAttacking = true;
    player.attackedFinished = false;
    player.whipState = player.gAnimationManager->getCurrentFrameIndex();
    player.hasToPressAgain = false;
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
        if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
            if ((keyReleased->scancode == KEY_RIGHT && player.dir == RIGHT) || 
                (keyReleased->scancode == KEY_LEFT && player.dir == LEFT)) {
                player.isWalking = false;
            }
        }
    }
    
}

void PlayerAttackIdleState::update(Player& player, float deltaTime)
{
    // Implementar logica (actualizar fisicas, animaciones, etc)
    player.currentAnimation = attackSimon;

    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
    
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    if (!player.gWhipAnimationManager->isPlaying(whipLvl1StandingJumping)){
    
        player.gWhipAnimationManager->playAnimation(whipLvl1StandingJumping);

    }
    player.gAnimationManager->update(deltaTime);
    player.gWhipAnimationManager->update(deltaTime*2.1);
    if (player.gAnimationManager->getCurrentFrameIndex() == 2 || player.gAnimationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    } else {
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    }
   
    
    
    if (player.gAnimationManager->isPlaying(player.currentAnimation) && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.gAnimationManager->setAnimationSpeed(1.0f); // 2x speed

        if (player.isWalking)
        {
            player.currentAnimation = walkSimon;
            player.setState(state<Walk>());
        }else{
            player.currentAnimation = idleSimon;
            player.setState(state<Idle>());
        }
        
        
    }
}

void PlayerAttackIdleState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
    window.draw(*player.whipSprite);
}

// --------------------------------------------------------------



// ---------------------------- ATTACK-JUMP ----------------------------

PlayerAttackJumpState::PlayerAttackJumpState() : PlayerState()
{
    printf("AttackingJ\n");
}

void PlayerAttackJumpState::init(Player& player)
{
    // Start the whip and attack animations
    player.gWhipAnimationManager->playAnimation(whipLvl1StandingJumping);
    player.gAnimationManager->playAnimation(attackSimon);

    // Store the horizontal speed if the player was walking
    if (player.isWalking) {
        player.horizontalSpeed = (player.dir == RIGHT) ? gMovementSpeed : -gMovementSpeed;
    } else {
        player.horizontalSpeed = 0.0f; // No horizontal movement if not walking
    }
}

void PlayerAttackJumpState::handleInput(Player& player, sf::Event event)
{
    // Nada, solo espera a que termine la animación de ataque
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == KEY_LEFT || KeyReleased->scancode == KEY_RIGHT)
        {
            player.isWalking = false;
        }
        if (KeyReleased->scancode == KEY_ATTACK)
        {
            player.hasToPressAgain = true;
        }
    }
}

void PlayerAttackJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = attackSimon;
   
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
    
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->setAnimationSpeed(2.0f); // 2x speed
    player.gWhipAnimationManager->setAnimationSpeed(2.0f); // 2x speed
    player.gAnimationManager->update(deltaTime);
    player.gWhipAnimationManager->update(deltaTime*2.1f);

    // Apply gravity
    player.verticalSpeed += GRAVITY * deltaTime;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    // Apply horizontal movement if the player was walking
    if (player.horizontalSpeed != 0.0f) {
        player.sprite->move({player.horizontalSpeed * deltaTime, 0.f});
    }
    
    if (player.gAnimationManager->getCurrentFrameIndex() == 2 || player.gAnimationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    } else {
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    }

    if (player.gAnimationManager->isPlaying(attackSimon) && player.gAnimationManager->isAnimationFinished() && 
        player.gWhipAnimationManager->isPlaying(whipLvl1StandingJumping) && player.gWhipAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        
        player.isJumping = false;
        player.gAnimationManager->setAnimationSpeed(1.0f); // 2x speed
        if(player.horizontalSpeed != 0.0f){
            player.currentAnimation = walkSimon;
            
            player.setState(state<Walk>());
        }
        else{
            player.currentAnimation = idleSimon;
            player.setState(state<Idle>());
        }
        
    }

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackJumpState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
    window.draw(*player.whipSprite);
}

// --------------------------------------------------------------



// ---------------------------- ATTACK-DUCK ----------------------------

PlayerAttackDuckState::PlayerAttackDuckState() : PlayerState()
{
    printf("AttackingDUCK\n");
}

void PlayerAttackDuckState::init(Player& player)
{
    if(!player.isAttacking){
        player.isAttacking = true;
    }

    if(!player.isDucking){
        player.isDucking = true;
    }
    player.gWhipAnimationManager->playAnimation(whipLvl1StandingJumping);
    player.gAnimationManager->playAnimation(attackSimon);
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
        if (KeyReleased->scancode == KEY_DOWN)
        {
            player.isDucking = false;
            player.setState(state<Idle>());
        }
        
   
    }
}

void PlayerAttackDuckState::update(Player& player, float deltaTime)
{
    
    player.currentAnimation = attackFloorSimon;
   
    if (!player.gAnimationManager->isPlaying(player.currentAnimation)){
    
        player.gAnimationManager->playAnimation(player.currentAnimation);

    }
    player.gAnimationManager->update(deltaTime);
    player.gWhipAnimationManager->update(deltaTime*2.1f);

    if (player.gAnimationManager->getCurrentFrameIndex() == 2 || player.gAnimationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y-1)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    } else {
        if (player.dir == RIGHT) {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whipSprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whipSprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    }

    if (player.gAnimationManager->isPlaying(attackFloorSimon) && player.gAnimationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.currentAnimation = duckSimon;
        player.gAnimationManager->setAnimationSpeed(1.0f); // 2x speed

        player.setState(state<Duck>());
    }

    // Implementar logica (actualizar fisicas, animaciones, etc)
}

void PlayerAttackDuckState::draw(Player& player, sf::RenderWindow &window)
{
    // Pintar en window (tener en cuenta animaciones, etc)
    window.draw(*player.sprite);
    window.draw(*player.whipSprite);
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
    window.draw(*player.whipSprite);
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



