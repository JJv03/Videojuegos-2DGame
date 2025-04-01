#include "playerState.h"
#include <iostream>
using namespace std;

template <typename T>
std::unique_ptr<T> state() {
    return std::make_unique<T>();
}
constexpr float GRAVITY { 450.0f };       
constexpr float JUMP_FORCE { 205.0f };    

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
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
           
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
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if (keyReleased->scancode == KEY_ATTACK) {
            player.hasToPressAgain = true;
        }
    }
}

void PlayerIdleState::update(Player& player, float deltaTime)
{
    if(sf::Keyboard::isKeyPressed(KEY_RIGHT)){
        player.dir = RIGHT;
        player.isWalking = true;
        player.setState(state<Walk>());
    }

    if(sf::Keyboard::isKeyPressed(KEY_LEFT)){
        player.dir = LEFT;
        player.isWalking = true;
        player.setState(state<Walk>());
    }

    if (!player.isOnGround)
    {
        player.sprite->move({0.f,GRAVITY*deltaTime});
    }
    player.currentAnimation = idleSimon;
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);
    }
    player.animationManager->update(deltaTime);
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
    
    window.draw(*player.sprite);
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
    
    if (!player.isOnGround)
    {
        player.sprite->move({0.f,GRAVITY*deltaTime});
    }
    
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
}

void PlayerWalkState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
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
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.attackedFinished = false;
            player.hasToPressAgain = true;
            player.setState(state<AttackJump>());
        }

    }
    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        if (keyReleased->scancode == KEY_LEFT || keyReleased->scancode == KEY_RIGHT)
        {
            //player.horizontalSpeed = 0.0f;
            //player.isWalking = false;
        }
        
    }
}

void PlayerJumpState::update(Player& player, float deltaTime)
{
    player.currentAnimation = jumpSimon;

    player.verticalSpeed += GRAVITY * deltaTime* 1.2f;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    
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
    window.draw(*player.sprite);
}

void PlayerJumpState::end(Player& player)
{
    
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

    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT && player.dir == LEFT) {
            player.dir = RIGHT;
            player.sprite->setScale({-1.f, 1.f});
        }
        
        if (keyPressed->scancode == KEY_LEFT && player.dir == RIGHT) {
            player.dir = LEFT;
            player.sprite->setScale({1.f, 1.f});
        }

        if (keyPressed->scancode == KEY_ATTACK) {
            player.isAttacking = true;
            player.hasToPressAgain = false;
            player.attackedFinished = false;
            player.setState(state<AttackDuck>());
        }
          
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if (keyReleased->scancode == KEY_DOWN){
            player.isDucking = false;
            player.setState(state<Idle>());
        }       
        if (keyReleased->scancode == KEY_ATTACK) {
            player.hasToPressAgain = true;
        }
    }    
}

void PlayerDuckState::update(Player& player, float deltaTime)
{
    if (!player.isOnGround)
    {
        player.sprite->move({0.f,GRAVITY*deltaTime*deltaTime});
    }

    player.currentAnimation = duckSimon;
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);
}

void PlayerDuckState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
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


        if (keyPressed->scancode == KEY_ATTACK) {
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
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {
            player.dir = RIGHT;
        }
        
        if (keyPressed->scancode == KEY_LEFT) {
            player.dir = LEFT;
        }


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
    
    player.whip.animationManager->playAnimation(whipLvl1StandingJumping);
    player.animationManager->playAnimation(attackSimon);
    player.isAttacking = true;
    player.attackedFinished = false;
    player.hasToPressAgain = false;
}

void PlayerAttackIdleState::handleInput(Player& player, sf::Event event)
{
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
   
    
    
    if (player.animationManager->isPlaying(player.currentAnimation) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        //player.animationManager->setAnimationSpeed(1.0f); // 2x speed

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
    window.draw(*player.sprite);
    window.draw(*player.whip.sprite);
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
    player.verticalSpeed += GRAVITY * deltaTime;
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
    if (player.animationManager->isPlaying(attackSimon) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        
        player.isJumping = false;
        
        player.currentAnimation = idleSimon;
        player.setState(state<Idle>());
    }

}

void PlayerAttackJumpState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.whip.sprite);
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
    if (const auto* KeyReleased = event.getIf<sf::Event::KeyReleased>())
    {
        
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
                             player.sprite->getPosition().y + 4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - 24, // Adjust X offset
                             player.sprite->getPosition().y + 4)  // Adjust Y offset
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

    if (player.animationManager->isPlaying(attackFloorSimon) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.currentAnimation = duckSimon;
        player.setState(state<Duck>());
    }

}

void PlayerAttackDuckState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.whip.sprite);
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

}

void PlayerHurtState::handleInput(Player& player, sf::Event event)
{
}

void PlayerHurtState::update(Player& player, float deltaTime)
{
    if(!player.isBeingHurt){
        player.setState(state<Idle>());
    }

}

void PlayerHurtState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
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

}

void PlayerDeadState::handleInput(Player& player, sf::Event event)
{
}

void PlayerDeadState::update(Player& player, float deltaTime)
{
}

void PlayerDeadState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
}

void PlayerDeadState::end(Player& player)
{
    
}

void PlayerDeadState::hello(){
    std::cout << "PLAYER STATE: Dead" << std::endl;
}

// --------------------------------------------------------------



