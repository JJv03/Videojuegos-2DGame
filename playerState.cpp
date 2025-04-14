#include "playerState.h"
#include <iostream>
#include "globals.h"

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
using AttackSecondary = PlayerAttackSecondaryState;
using AttackJumpSecondary = PlayerAttackJumpSecondaryState;
using Hurt = PlayerHurtState;
using Invulnerable = PlayerInvulnerableState;
using Dead = PlayerDeadState;
// constexpr auto KEY_RIGHT = sf::Keyboard::Scancode::Right;
// constexpr auto KEY_LEFT = sf::Keyboard::Scancode::Left;
// constexpr auto KEY_DOWN = sf::Keyboard::Scancode::Down;
// constexpr auto KEY_UP = sf::Keyboard::Scancode::Up;
// constexpr auto KEY_JUMP = sf::Keyboard::Scancode::X;
// constexpr auto KEY_ATTACK = sf::Keyboard::Scancode::Z;

// Keys 4 hurt and dead just to animate and see the logic, after collitions with enemies are done change it
//constexpr auto KEY_HURT = sf::Keyboard::Scancode::H;
//constexpr auto KEY_DEAD = sf::Keyboard::Scancode::D;
//constexpr auto KEY_REVIVE = sf::Keyboard::Scancode::R;
PlayerState::PlayerState():configManager(configManager::getInstance()){}

// ---------------------------- IDLE ----------------------------

PlayerIdleState::PlayerIdleState() : PlayerState()
{
}

void PlayerIdleState::init(Player& player)
{
    player.isDucking = false;
    player.isJumping = false;
    player.isWalking = false;
    player.isAttacking = false;
    player.isOnStairs = false;
    player.attackedFinished = false;
    player.isBeingHurt = false;
    player.isDead = false;  
    player.isOnGround = false;  
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.down) {
            player.isDucking = true;
            player.setState(state<Duck>());
        }
    
        if (keyPressed->scancode == controls.jump) {
            player.isJumping = true;
            player.verticalSpeed = -gPlayerJumpForce;
            player.isOnGround = false;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == controls.attack && player.hasToPressAgain) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
            player.setState(state<AttackIdle>());
        }

        if(keyPressed->scancode == controls.useSubWeapon && player.hasToPressAgain){
            if(player.hearts>0){// && player.subWeaponType != ItemType::NONE){
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackSecondary>());
            }
        }

        
    }
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if (keyReleased->scancode == controls.attack) {
            player.hasToPressAgain = true;
        }
        if(keyReleased->scancode == controls.up){
            player.isPressingUp = false;
        }
    }
}

void PlayerIdleState::update(Player& player, float deltaTime)
{
    auto controls = configManager.getControls();
    if(sf::Keyboard::isKeyPressed(controls.right)){
        player.dir = RIGHT;
        player.isWalking = true;
        player.setState(state<Walk>());
    }

    if(sf::Keyboard::isKeyPressed(controls.left)){
        player.dir = LEFT;
        player.isWalking = true;
        player.setState(state<Walk>());
    }

    player.sprite->move({0.f, gPlayerGravity * deltaTime});

    player.currentAnimation = idleSimon;
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);
    }
    player.animationManager->update(deltaTime);

    // Invulnerabilidad
    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
}

void PlayerIdleState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.dir == RIGHT)
    {
        player.sprite->setScale({-1.f, 1.f});
    }
    else
    {
        player.sprite->setScale({1.f, 1.f});
    }
    if (player.visible)
    {
        window.draw(*player.sprite);
    }
    
    
}

void PlayerIdleState::end(Player& player)
{

}

void PlayerIdleState::hello(){
    std::cout << "PLAYER STATE: Idle" << std::endl;
}


// --------------------------------------------------------------


// ---------------------------- WALK ----------------------------

PlayerWalkState::PlayerWalkState() : PlayerState()
{
}

void PlayerWalkState::init(Player& player)
{
}

void PlayerWalkState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.right && player.dir == LEFT) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == controls.left && player.dir == RIGHT) {
            player.dir = LEFT;
        }

        if (keyPressed->scancode == controls.down) {
            player.isWalking = false;
            player.isDucking = true;
            player.setState(state<Duck>());
        }
    
        if (keyPressed->scancode == controls.jump) {
            player.isJumping = true;
            player.isOnGround = false;
            player.verticalSpeed = -gPlayerJumpForce;
            player.setState(state<Jump>());
        }
    
        if (keyPressed->scancode == controls.attack && player.hasToPressAgain) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
            player.setState(state<AttackIdle>());
        }

        if(keyPressed->scancode == controls.useSubWeapon && player.hasToPressAgain){
            if(player.hearts>0){// && player.subWeaponType != ItemType::NONE){
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackSecondary>());
            }
        }
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if ((keyReleased->scancode == controls.right && player.dir == RIGHT) || 
            (keyReleased->scancode == controls.left && player.dir == LEFT)) {
            player.isWalking = false;
            player.setState(state<Idle>());
        }
    }
}

void PlayerWalkState::update(Player& player, float deltaTime)
{
    player.sprite->move({0.f,gPlayerGravity*deltaTime});
    
    if(player.dir == RIGHT){
        player.sprite->move({deltaTime * gPlayerMovementSpeed, 0.f});
        player.sprite->setScale({-1.f, 1.f});
    } else {
        player.sprite->move({-1.f * deltaTime * gPlayerMovementSpeed, 0.f});
        player.sprite->setScale({1.f, 1.f});
    }
    player.currentAnimation = walkSimon;
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);

    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.visible)
    {
        window.draw(*player.sprite);
    }
    
}

void PlayerWalkState::end(Player& player)
{
    
}

void PlayerWalkState::hello(){
    std::cout << "PLAYER STATE: Walk" << std::endl;
}

// --------------------------------------------------------------


// ---------------------------- JUMP ----------------------------

PlayerJumpState::PlayerJumpState() : PlayerState()
{
}

void PlayerJumpState::init(Player& player)
{
    if (player.isWalking) {
        player.horizontalSpeed = (player.dir == RIGHT) ? gPlayerMovementSpeed : -gPlayerMovementSpeed;
    } else {
        player.horizontalSpeed = 0.0f; // No horizontal movement if not walking
    }
}

void PlayerJumpState::handleInput(Player& player, sf::Event event)
{
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.attack) {
            player.isAttacking = true;
            player.attackedFinished = false;
            player.hasToPressAgain = true;
            player.setState(state<AttackJump>());
        }

        if(keyPressed->scancode == controls.useSubWeapon && player.hasToPressAgain){
            if(player.hearts>0){// && player.subWeaponType != ItemType::NONE){
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackJumpSecondary>());
            }
        }

    }
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = jumpSimon;

    player.verticalSpeed += gPlayerGravity * deltaTime* 1.2f;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    // If Simon was walking before jumping, move in the x direction
    if (player.isWalking)
    {
        if (player.dir == RIGHT)
        {
            player.horizontalSpeed = gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime, 0.f});
        }
        else
        {
            player.horizontalSpeed = -gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime , 0.f});
        }
    }

    // Play jump animation if not already playing
    if (!player.animationManager->isPlaying(player.currentAnimation))
    {
        player.animationManager->playAnimation(player.currentAnimation);
    }
    player.animationManager->update(deltaTime);

    // Check if landed
    if (player.isOnGround)
    {
        player.isJumping = false;
        player.setState(state<Idle>());
    }
}


void PlayerJumpState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.visible)
    {
        window.draw(*player.sprite);
    }
}

void PlayerJumpState::end(Player& player)
{
    player.sprite->move({0.f,-8.0f});
}

void PlayerJumpState::hello(){
    std::cout << "PLAYER STATE: Jump" << std::endl;
}


// --------------------------------------------------------------


// ---------------------------- DUCK ----------------------------

PlayerDuckState::PlayerDuckState() : PlayerState()
{
}

void PlayerDuckState::init(Player& player)
{
    player.sprite->move({0.f,8.0f});
    player.isDucking = true;
}

void PlayerDuckState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.right && player.dir == LEFT) {
            player.dir = RIGHT;
            player.sprite->setScale({-1.f, 1.f});
        }
        
        if (keyPressed->scancode == controls.left && player.dir == RIGHT) {
            player.dir = LEFT;
            player.sprite->setScale({1.f, 1.f});
        }

        if (keyPressed->scancode == controls.attack) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
            player.attackedFinished = false;
            player.setState(state<AttackDuck>());
        }
          
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if (keyReleased->scancode == controls.down){
            player.isDucking = false;
            player.setState(state<Idle>());
        }       
        if (keyReleased->scancode == controls.attack) {
            player.hasToPressAgain = true;
        }
    }    
}

void PlayerDuckState::update(Player& player, float deltaTime)
{
    if (!player.isOnGround)
    {
        player.sprite->move({0.f,gPlayerGravity*deltaTime*deltaTime});
    }

    player.currentAnimation = duckSimon;
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);
    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
}

void PlayerDuckState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.visible)
    {
        window.draw(*player.sprite);
    }
}

void PlayerDuckState::end(Player& player)
{
    player.sprite->move({0.f,-8.0f});
}

void PlayerDuckState::hello(){
    std::cout << "PLAYER STATE: Duck" << std::endl;
}

// --------------------------------------------------------------



// ---------------------------- STAIRS ----------------------------

PlayerStairState::PlayerStairState() : PlayerState()
{
}

void PlayerStairState::init(Player& player)
{
    player.isOnStairs = true;   
}

void PlayerStairState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.right) {
            player.dir = RIGHT;
            player.isWalking = true;
            player.setState(state<StairsWalk>());
        }
        
        if (keyPressed->scancode == controls.left) {
            player.dir = LEFT;
            player.isWalking = true;
            player.setState(state<StairsWalk>());
        }


        if (keyPressed->scancode == controls.attack) {
            player.isAttacking = true;
            player.setState(state<AttackStairs>());
        }
          
    }
}

void PlayerStairState::update(Player& player, float deltaTime)
{
}

void PlayerStairState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
}

void PlayerStairState::end(Player& player)
{
    
}

void PlayerStairState::hello(){
    std::cout << "PLAYER STATE: Stairs" << std::endl;
}

// --------------------------------------------------------------



// ---------------------------- STAIRS-WALK ----------------------------

PlayerStairWalkState::PlayerStairWalkState() : PlayerState()
{
}

void PlayerStairWalkState::init(Player& player)
{
    player.isDucking = true;
}

void PlayerStairWalkState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.right) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == controls.left) {
            player.dir = LEFT;
        }


        if (keyPressed->scancode == controls.attack) {
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
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);
    if (player.isAttacking && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
    }*/
    
}

void PlayerStairWalkState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
}

void PlayerStairWalkState::end(Player& player)
{
    
}

void PlayerStairWalkState::hello(){
    std::cout << "PLAYER STATE: Stair walk" << std::endl;
}

// --------------------------------------------------------------




// ---------------------------- ATTACK-IDLE ----------------------------

PlayerAttackIdleState::PlayerAttackIdleState() : PlayerState()
{
}

void PlayerAttackIdleState::init(Player& player)
{
    player.isAttacking = true;
    
    if (!player.animationManager->isPlaying(attackSimon) || player.animationManager->isAnimationFinished()){
        player.animationManager->playAnimation(attackSimon);
        player.whip.animationManager->playAnimation(whipLvl1StandingJumping);
    }

    player.isAttacking = true;
    player.attackedFinished = false;
    player.hasToPressAgain = false;
}

void PlayerAttackIdleState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == controls.attack)
        {
            player.hasToPressAgain = true;
        }
        if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
            if ((keyReleased->scancode == controls.right && player.dir == RIGHT) || 
                (keyReleased->scancode == controls.left && player.dir == LEFT)) {
                player.isWalking = false;
            }
        }
    }
    
}

void PlayerAttackIdleState::update(Player& player, float deltaTime)
{
    player.currentAnimation = attackSimon;

    if (!player.animationManager->isPlaying(player.currentAnimation)){

        player.animationManager->playAnimation(player.currentAnimation);
    }

    if (!player.whip.animationManager->isPlaying(whipLvl1StandingJumping)){
    
        player.whip.animationManager->playAnimation(whipLvl1StandingJumping);

    }
    player.animationManager->update(deltaTime);
    player.whip.animationManager->update(deltaTime);
    if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, // Adjust X offset
                             player.sprite->getPosition().y+4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 24, // Adjust X offset
                             player.sprite->getPosition().y+4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    } else {
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    }
   
    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
            player.whip.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    if (player.animationManager->isPlaying(player.currentAnimation) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        //player.animationManager->setAnimationSpeed(1.0f); // 2x speed

        player.currentAnimation = idleSimon;
        player.whip.animationManager->playAnimation(whipNoAttack);
        player.setState(state<Idle>());
    }
}

void PlayerAttackIdleState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.visible)
    {
        window.draw(*player.sprite);
        window.draw(*player.whip.sprite);
    }
    
    
}

void PlayerAttackIdleState::end(Player& player)
{
}

void PlayerAttackIdleState::hello(){
    std::cout << "PLAYER STATE: Attack Idle" << std::endl;
}


// --------------------------------------------------------------



// ---------------------------- ATTACK-JUMP ----------------------------

PlayerAttackJumpState::PlayerAttackJumpState() : PlayerState()
{
}

void PlayerAttackJumpState::init(Player& player)
{
    // Start the whip and attack animations
    player.whip.animationManager->playAnimation(whipLvl1StandingJumping);
    player.animationManager->playAnimation(attackSimon);
    player.isAttacking = true;
    player.attackedFinished = false;
    player.hasToPressAgain = false;
    // Store the horizontal speed if the player was walking
    if (player.isWalking) {
        player.horizontalSpeed = (player.dir == RIGHT) ? gPlayerMovementSpeed : -gPlayerMovementSpeed;
    } else {
        player.horizontalSpeed = 0.0f; // No horizontal movement if not walking
    }
}

void PlayerAttackJumpState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == controls.attack)
        {
            player.hasToPressAgain = true;
        }
    }
}

void PlayerAttackJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = attackSimon;
   
    if (!player.animationManager->isPlaying(player.currentAnimation)){
    
        player.animationManager->playAnimation(player.currentAnimation);

    }
    if (!player.whip.animationManager->isPlaying(whipLvl1StandingJumping)){
    
        player.whip.animationManager->playAnimation(whipLvl1StandingJumping);

    }
    //player.animationManager->setAnimationSpeed(2.0f); // 2x speed
    //player.whip.animationManager->setAnimationSpeed(2.0f); // 2x speed
    player.animationManager->update(deltaTime);
    player.whip.animationManager->update(deltaTime);

    // Apply gravity
    player.verticalSpeed += gPlayerGravity * deltaTime;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    
    if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, // Adjust X offset
                             player.sprite->getPosition().y+4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 24, // Adjust X offset
                             player.sprite->getPosition().y+4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    } else {
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, // Adjust X offset
                             player.sprite->getPosition().y)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); // Flip whip to face left
        }
    }

    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
            player.whip.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    // If Simon was walking before jumping, move in the x direction
    if (player.isWalking)
    {
        if (player.dir == RIGHT)
        {
            player.horizontalSpeed = gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime, 0.f});
        }
        else
        {
            player.horizontalSpeed = -gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime , 0.f});
        }
    }

    
            
    if(player.isOnGround){
        player.isJumping = false;
        player.setState(state<AttackIdle>());
    }

    if (player.animationManager->isPlaying(attackSimon) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.whip.animationManager->playAnimation(whipNoAttack);
        player.setState(state<Jump>());
    }

}

void PlayerAttackJumpState::draw(Player& player, sf::RenderWindow &window)
{
    if(player.visible){
        window.draw(*player.sprite);
        window.draw(*player.whip.sprite);
    }
    
}

void PlayerAttackJumpState::end(Player& player)
{
}

void PlayerAttackJumpState::hello(){
    std::cout << "PLAYER STATE: Attack Jump" << std::endl;
}


// --------------------------------------------------------------



// ---------------------------- ATTACK-DUCK ----------------------------

PlayerAttackDuckState::PlayerAttackDuckState() : PlayerState()
{
}

void PlayerAttackDuckState::init(Player& player)
{
    player.sprite->move({0.f,8.0f});

    player.isAttacking = true;
    
    player.isDucking = true;
    
    player.hasToPressAgain = false;
    player.whip.animationManager->playAnimation(whipLvl1StandingJumping);
    player.animationManager->playAnimation(attackSimon);
}

void PlayerAttackDuckState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        
        if (KeyReleased->scancode == controls.down)
        {
            player.isDucking = false;
            player.whip.animationManager->playAnimation(whipNoAttack);
            player.setState(state<Idle>());
        }
    }
}

void PlayerAttackDuckState::update(Player& player, float deltaTime)
{   
    player.currentAnimation = attackFloorSimon;
   
    if (!player.animationManager->isPlaying(player.currentAnimation)){
    
        player.animationManager->playAnimation(player.currentAnimation);

    }
    if (!player.whip.animationManager->isPlaying(whipLvl1StandingJumping)){
    
        player.whip.animationManager->playAnimation(whipLvl1StandingJumping);

    }
    player.animationManager->update(deltaTime);
    player.whip.animationManager->update(deltaTime);

    if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
        
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 24, 
                             player.sprite->getPosition().y + 4)  
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); 
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 24, 
                             player.sprite->getPosition().y + 4)  
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); 
        }
    } else {
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 16, 
                             player.sprite->getPosition().y)  
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); 
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + 16, 
                             player.sprite->getPosition().y)  
            );
            player.whip.sprite->setScale(sf::Vector2f(1.f, 1.f)); 
        }
    }

    if (player.isInvulnerable)
    {
        player.blinkTimer += deltaTime;
        if (player.blinkTimer >= player.blinkInterval) {
            player.visible = !player.visible; // Toggle visibility
            player.blinkTimer = 0.0f; // Reset the timer
            player.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
            player.whip.sprite->setColor(player.visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
        }
        
        if (player.invulnerableTimeCounter >= player.invulnerableTime) {
            player.isInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    if (player.animationManager->isPlaying(attackFloorSimon) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.currentAnimation = duckSimon;
        player.whip.animationManager->playAnimation(whipNoAttack);
        player.setState(state<Duck>());
    }

}

void PlayerAttackDuckState::draw(Player& player, sf::RenderWindow &window)
{
    if(player.visible){
        window.draw(*player.sprite);
        window.draw(*player.whip.sprite);
    }
}
void PlayerAttackDuckState::end(Player& player)
{
    player.sprite->move({0.f,-8.0f});
}

void PlayerAttackDuckState::hello(){
    std::cout << "PLAYER STATE: Attack Duck" << std::endl;
}


// --------------------------------------------------------------


// ---------------------------- ATTACK-STAIR ----------------------------

PlayerAttackStairState::PlayerAttackStairState() : PlayerState()
{
}

void PlayerAttackStairState::init(Player& player)
{
    player.isAttacking = true;
    player.isOnStairs = true;
    
}

void PlayerAttackStairState::handleInput(Player& player, sf::Event event)
{
}

void PlayerAttackStairState::update(Player& player, float deltaTime)
{
    if(!player.isAttacking){
        player.whip.animationManager->playAnimation(whipNoAttack);
        player.setState(state<Stairs>());
    }

}

void PlayerAttackStairState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.whip.sprite);
}

void PlayerAttackStairState::end(Player& player)
{
}

void PlayerAttackStairState::hello(){
    std::cout << "PLAYER STATE: Attack stairs" << std::endl;
}

// --------------------------------------------------------------


// ---------------------------- HURT ----------------------------

PlayerHurtState::PlayerHurtState() : PlayerState()
{
}

void PlayerHurtState::init(Player& player)
{

    player.isBeingHurt = true; 
    player.isInvulnerable = true;
}

void PlayerHurtState::handleInput(Player& player, sf::Event event)
{
}

void PlayerHurtState::update(Player& player, float deltaTime)
{
    player.currentAnimation = hurtSimon;

    player.verticalSpeed += gPlayerGravity * deltaTime * 1.2f;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    
    if (player.dir == RIGHT)
    {
        player.horizontalSpeed = -gPlayerMovementSpeed*0.8f;
        player.sprite->move({player.horizontalSpeed* deltaTime, 0.f});
    }
    else
    {
        player.horizontalSpeed = gPlayerMovementSpeed*0.8f;
        player.sprite->move({player.horizontalSpeed* deltaTime , 0.f});
    }
    
    if (!player.animationManager->isPlaying(player.currentAnimation))
    {
        player.animationManager->playAnimation(player.currentAnimation);
    }
    player.animationManager->update(deltaTime);

    
    if (player.isOnGround)
    {
        player.blinkTimer = 0.0f; 
        player.visible = true; 
        player.isJumping = false;
        player.isBeingHurt = false;
        player.invulnerableTimeCounter = 0.0f; 
        player.setState(state<Idle>());
    }   

}

void PlayerHurtState::draw(Player& player, sf::RenderWindow &window)
{
    if(player.visible) {
        window.draw(*player.sprite);
    }
    
}

void PlayerHurtState::end(Player& player)
{
    
}

void PlayerHurtState::hello(){
    std::cout << "PLAYER STATE: Hurt" << std::endl;
}

// --------------------------------------------------------------


// ---------------------------- DEAD ----------------------------

PlayerDeadState::PlayerDeadState() : PlayerState()
{
}

void PlayerDeadState::init(Player& player)
{
    player.currentAnimation = deathSimon;
    player.animationManager->playAnimation(player.currentAnimation);
    // Initial position adjustment for first frame
    player.sprite->move(sf::Vector2f(0.f, 8.f));
    player.isDead = true;
    player.isInvulnerable = true;
    player.deathRestart = true;
}

void PlayerDeadState::handleInput(Player& player, sf::Event event)
{
}

void PlayerDeadState::update(Player& player, float deltaTime)
{
    if(!player.hasDied){
        player.currentAnimation=deathSimon;
        if (!player.animationManager->isPlaying(player.currentAnimation))
        {
            player.animationManager->playAnimation(player.currentAnimation);
        }
        
        
        if (player.animationManager->getCurrentFrameIndex()==0)
        {
            
        }
        else{
            // Second frame - lower by additional 8 pixels (total 16 from original)
            player.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x, player.sprite->getPosition().y+8.f)
            );
        }
        player.animationManager->update(deltaTime);
    }
}

void PlayerDeadState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
}

void PlayerDeadState::end(Player& player)
{
    player.isInvulnerable = false;
    player.invulnerableTimeCounter = player.invulnerableTime + 1;
    player.isDead = false;
    player.hasDied = false;
}

void PlayerDeadState::hello(){
    std::cout << "PLAYER STATE: Dead" << std::endl;
}

// --------------------------------------------------------------

// ---------------------------- ATTACK SECONDARY ----------------------------

PlayerAttackSecondaryState::PlayerAttackSecondaryState() : PlayerState()
{
}

void PlayerAttackSecondaryState::init(Player& player)
{
    if (!player.animationManager->isPlaying(attackSimon) || player.animationManager->isAnimationFinished()){
        player.animationManager->playAnimation(attackSimon);
    }

    player.isAttacking = true;
    player.attackedFinished = false;
    player.hasToPressAgain = false;
}

void PlayerAttackSecondaryState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == controls.useSubWeapon)
        {
            player.hasToPressAgain = true;
        }
        
    }
    
}

void PlayerAttackSecondaryState::update(Player& player, float deltaTime)
{   
    player.currentAnimation = attackSimon;

    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        player.animationManager->playAnimation(player.currentAnimation);
    }

    player.animationManager->update(deltaTime);

   
    if (player.animationManager->getCurrentFrameIndex() == 4 && !player.weaponIsActive) {
        SubWeapon secundaria;
        //secundaria.type = player.subWeaponType;
        secundaria.type = ItemType::DAGGER;
        secundaria.animationManager = player.subWeapon.animationManager;
        secundaria.sprite = std::make_shared<sf::Sprite>(*player.subWeapon.sprite);
        secundaria.direction = player.dir;
        sf::Vector2f spawnPos = player.sprite->getPosition();
        if (player.dir == RIGHT) {
            secundaria.sprite->setScale({1.f, 1.f}); 
        } else {
            secundaria.sprite->setScale({-1.f, 1.f});
        }
        spawnPos.y -= 30.f; 

        if (secundaria.type == ItemType::AXE) {
            secundaria.verticalSpeed = -450.f; 
            secundaria.animationManager->playAnimation(axeThrowing); 
        } else if (secundaria.type == ItemType::DAGGER) {
            secundaria.verticalSpeed = 0.f;
            secundaria.animationManager->playAnimation(daggerThrowing);
        } else if (secundaria.type == ItemType::BOOMERANG) {
            secundaria.horizontalSpeed = 250.f; 
            secundaria.animationManager->playAnimation(boomerangThrowing);
        } else if (secundaria.type == ItemType::FIRE_BOMB) {
            secundaria.verticalSpeed = -250.f;
            secundaria.animationManager->playAnimation(fireBombThrowing);
        } else if (secundaria.type == ItemType::STOPWATCH) {
            player.hearts -= 4; 
        } 

        secundaria.sprite->setPosition(spawnPos);
        player.activeSubWeapons.push_back(secundaria);
        player.weaponIsActive = true;
    }
    
    if (player.animationManager->isPlaying(player.currentAnimation) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.currentAnimation = idleSimon;
        player.setState(state<Idle>());
    }
}

void PlayerAttackSecondaryState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.subWeapon.sprite);
}

void PlayerAttackSecondaryState::end(Player& player)
{
    
}

void PlayerAttackSecondaryState::hello(){
    std::cout << "PLAYER STATE: Attack Secondary" << std::endl;
}

// --------------------------------------------------------------


// ---------------------------- ATTACK JUMP SECONDARY ----------------------------

PlayerAttackJumpSecondaryState::PlayerAttackJumpSecondaryState() : PlayerState()
{
}

void PlayerAttackJumpSecondaryState::init(Player& player)
{
    if (!player.animationManager->isPlaying(attackSimon) || player.animationManager->isAnimationFinished()){
        player.animationManager->playAnimation(attackSimon);
    }
    if (player.isWalking) {
        player.horizontalSpeed = (player.dir == RIGHT) ? gPlayerMovementSpeed : -gPlayerMovementSpeed;
    } else {
        player.horizontalSpeed = 0.0f; // No horizontal movement if not walking
    }

    player.isAttacking = true;
    player.attackedFinished = false;
    player.hasToPressAgain = false;
}

void PlayerAttackJumpSecondaryState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (KeyReleased->scancode == controls.useSubWeapon)
        {
            player.hasToPressAgain = true;
        }
        
    }
    
}

void PlayerAttackJumpSecondaryState::update(Player& player, float deltaTime)
{   
    player.currentAnimation = attackSimon;

    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        player.animationManager->playAnimation(player.currentAnimation);
    }

    player.animationManager->update(deltaTime);
    player.verticalSpeed += gPlayerGravity * deltaTime;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    if (player.isWalking)
    {
        if (player.dir == RIGHT)
        {
            player.horizontalSpeed = gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime, 0.f});
        }
        else
        {
            player.horizontalSpeed = -gPlayerMovementSpeed;
            player.sprite->move({player.horizontalSpeed* deltaTime , 0.f});
        }
    }
   
    if (player.animationManager->getCurrentFrameIndex() == 4 && !player.weaponIsActive) {
        SubWeapon secundaria;
        //secundaria.type = player.subWeaponType;
        secundaria.type = ItemType::DAGGER;
        secundaria.animationManager = player.subWeapon.animationManager;
        secundaria.sprite = std::make_shared<sf::Sprite>(*player.subWeapon.sprite);
        secundaria.direction = player.dir;
        sf::Vector2f spawnPos = player.sprite->getPosition();
        if (player.dir == RIGHT) {
            secundaria.sprite->setScale({1.f, 1.f}); 
        } else {
            secundaria.sprite->setScale({-1.f, 1.f});
        }
        spawnPos.y -= 30.f; 

        if (secundaria.type == ItemType::AXE) {
            secundaria.verticalSpeed = -450.f; 
            secundaria.animationManager->playAnimation(axeThrowing); 
        } else if (secundaria.type == ItemType::DAGGER) {
            secundaria.verticalSpeed = 0.f;
            secundaria.animationManager->playAnimation(daggerThrowing);
        } else if (secundaria.type == ItemType::BOOMERANG) {
            secundaria.horizontalSpeed = 250.f; 
            secundaria.animationManager->playAnimation(boomerangThrowing);
        } else if (secundaria.type == ItemType::FIRE_BOMB) {
            secundaria.verticalSpeed = -250.f;
            secundaria.animationManager->playAnimation(fireBombThrowing);
        } else if (secundaria.type == ItemType::STOPWATCH) {
            player.hearts -= 4; 
        } 

        secundaria.sprite->setPosition(spawnPos);
        player.activeSubWeapons.push_back(secundaria);
        player.weaponIsActive = true;
    }

    
    
    if (player.animationManager->isPlaying(player.currentAnimation) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.currentAnimation = jumpSimon;
        player.setState(state<Jump>());
    }
}

void PlayerAttackJumpSecondaryState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.subWeapon.sprite);
}

void PlayerAttackJumpSecondaryState::end(Player& player)
{
    
}

void PlayerAttackJumpSecondaryState::hello(){
    std::cout << "PLAYER STATE: Attack Secondary" << std::endl;
}

// --------------------------------------------------------------