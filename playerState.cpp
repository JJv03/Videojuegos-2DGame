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
using AutoWalk = PlayerAutoWalkState;
using Jump = PlayerJumpState;
using Duck = PlayerDuckState;
using StairIdle = PlayerStairIdleState;
using StairWalk = PlayerStairWalkState;
using AttackIdle = PlayerAttackIdleState;
using AttackJump = PlayerAttackJumpState;
using AttackDuck = PlayerAttackDuckState;
using AttackStairs = PlayerAttackStairState;
using AttackSecondary = PlayerAttackSecondaryState;
using AttackJumpSecondary = PlayerAttackJumpSecondaryState;
using Hurt = PlayerHurtState;
using HurtStair = PlayerHurtStairState;
using Invulnerable = PlayerInvulnerableState;
using Dead = PlayerDeadState;
using WhipUpgrade = PlayerWhipUpgradeState;
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

animationID whipLevelAnimation(int level){
    if (level== 1) {
        return whipLvl1StandingJumping;
    } else if (level == 2) {
        return whipLvl2StandingJumping;
    } else if (level == 3) {
        return whipLvl3C1StandingJumping;
    } else if (level == 4) {
        return whipLvl3C2StandingJumping;
    } else if (level == 5) {
        return whipLvl3C3StandingJumping;
    } else  {
        return whipLvl3C4StandingJumping;
    } 
}

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
    player.isJumpStanding = false;
    player.restartJumpAnimation = true;
}

void PlayerIdleState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){

        
        if (keyPressed->scancode == controls.up) {
            if (player.isNearStair && (player.stairStart->type == StairTile::Type::BOTTOM_LEFT ||
                                       player.stairStart->type == StairTile::Type::BOTTOM_RIGHT))
            {
                player.autoWalkDistance = player.stairStart->hitboxes[0].position.x - player.getBounds()[0].position.x;
                player.setState(state<AutoWalk>());
            }
        }

        if (keyPressed->scancode == controls.down) {
            if (player.isNearStair && (player.stairStart->type == StairTile::Type::TOP_LEFT ||
                                       player.stairStart->type == StairTile::Type::TOP_RIGHT))
            {
                player.autoWalkDistance = player.stairStart->hitboxes[0].position.x - player.getBounds()[0].position.x;
                player.setState(state<AutoWalk>());

            } else {
                player.isDucking = true;
                player.setState(state<Duck>());
            } 
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
            //if(player.hearts>0 && !player.weaponIsActive && player.subWeaponType != ItemType::NONE && player.subWeaponType != ItemType::STOPWATCH ){
            if(player.hearts>0 && !player.weaponIsActive){// && player.subWeaponType != ItemType::NONE){ // Depuracion
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackSecondary>());
                if (player.hearts>3  && player.subWeaponType == ItemType::STOPWATCH){
                    player.hearts -= 3;
                }
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
    if (player.upgradeWhip){
        player.setState(state<WhipUpgrade>());
    }
    
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
            player.startInvulnerable = false;
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

        if (keyPressed->scancode == controls.up) {
            if (player.isNearStair && (player.stairStart->type == StairTile::Type::BOTTOM_LEFT ||
                                       player.stairStart->type == StairTile::Type::BOTTOM_RIGHT))
            {
                player.autoWalkDistance = player.stairStart->hitboxes[0].position.x - player.getBounds()[0].position.x;
                player.setState(state<AutoWalk>());
            }
        }

        if (keyPressed->scancode == controls.down) {
            if (player.isNearStair && (player.stairStart->type == StairTile::Type::TOP_LEFT ||
                                       player.stairStart->type == StairTile::Type::TOP_RIGHT))
            {
                player.autoWalkDistance = player.stairStart->hitboxes[0].position.x - player.getBounds()[0].position.x;
                player.setState(state<AutoWalk>());

            } else {
                player.isWalking = false;
                player.isDucking = true;
                player.setState(state<Duck>());
            } 
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
            //if(player.hearts>0 && !player.weaponIsActive && player.subWeaponType != ItemType::NONE && player.subWeaponType != ItemType::STOPWATCH ){
            if(player.hearts>0 && !player.weaponIsActive){// && player.subWeaponType != ItemType::NONE){ // Depuracion
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackSecondary>());
                if (player.hearts>3  && player.subWeaponType == ItemType::STOPWATCH){
                    player.hearts -= 3;
                }
            }
            
        }
    }
    
    if(const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()){
        if ((keyReleased->scancode == controls.right && player.dir == RIGHT) || 
            (keyReleased->scancode == controls.left && player.dir == LEFT)) {
            player.isWalking = false;
            player.setState(state<Idle>());
        }
        if (keyReleased->scancode == controls.attack) {
            player.hasToPressAgain = true;
        }
    }
}

void PlayerWalkState::update(Player& player, float deltaTime)
{
    if (player.upgradeWhip){
        player.setState(state<WhipUpgrade>());
    }
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
            player.startInvulnerable = false;
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


// ---------------------------- AUTO-WALK ----------------------------

PlayerAutoWalkState::PlayerAutoWalkState() : PlayerState()
{
}

void PlayerAutoWalkState::init(Player& player)
{
}

void PlayerAutoWalkState::handleInput(Player& player, sf::Event event)
{
    
}

void PlayerAutoWalkState::update(Player& player, float deltaTime)
{   
    float move = deltaTime * gPlayerMovementSpeed;

    if(abs(player.autoWalkDistance) < 1.f) {
        move = abs(player.autoWalkDistance);
    }

    // Player going right
    if(player.autoWalkDistance > 0.f)
    {
        player.sprite->move({move, 0.f});
        player.dir = RIGHT;
        player.sprite->setScale({-1.f, 1.f});
        player.autoWalkDistance -= move;

        if(player.autoWalkDistance <= 0.f){
            if(player.isNearStair){
                player.isPositionedInStair = true;

                if(player.stairStart->type == StairTile::Type::BOTTOM_RIGHT ||
                   player.stairStart->type == StairTile::Type::TOP_RIGHT){

                    player.dir = LEFT;
                    player.sprite->setScale({1.f, 1.f});
                }

                player.setState(state<StairWalk>());

            } else {
                player.setState(state<Idle>());
            }
        }
    }    
    // Player going left
    else {
        player.sprite->move({-move, 0.f});
        player.dir = LEFT;
        player.sprite->setScale({1.f, 1.f});
        player.autoWalkDistance += move;

        if(player.autoWalkDistance >= 0.f){
            if(player.isNearStair){
                player.isPositionedInStair = true;

                if(player.stairStart->type == StairTile::Type::BOTTOM_LEFT||
                   player.stairStart->type == StairTile::Type::TOP_LEFT){

                    player.dir = RIGHT;
                    player.sprite->setScale({-1.f, 1.f});
                }

                player.setState(state<StairWalk>());
            } else {
                player.setState(state<Idle>());
            }
        }
    }
    
    player.currentAnimation = walkSimon;
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);
}

void PlayerAutoWalkState::draw(Player& player, sf::RenderWindow &window)
{
    if (player.visible)
    {
        window.draw(*player.sprite);
    }
}

void PlayerAutoWalkState::end(Player& player)
{
    player.autoWalkDistance = 0.f;
}

void PlayerAutoWalkState::hello(){
    std::cout << "PLAYER STATE: Auto Walk" << std::endl;
}


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
            //if(player.hearts>0 && !player.weaponIsActive && player.subWeaponType != ItemType::NONE && player.subWeaponType != ItemType::STOPWATCH ){
            if(player.hearts>0 && !player.weaponIsActive){// && player.subWeaponType != ItemType::NONE){ // Depuracion
                player.isAttacking = true;
                player.hasToPressAgain = false;
                player.hearts -= 1;
                player.setState(state<AttackJumpSecondary>());
                if (player.hearts>3  && player.subWeaponType == ItemType::STOPWATCH){
                    player.hearts -= 3;
                }
            }
            
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

void PlayerJumpState::update(Player& player, float deltaTime)
{
    if(player.isJumpStanding){
        player.currentAnimation = idleSimon;
    } else {
        player.currentAnimation = jumpSimon;
    }

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
            player.startInvulnerable = false;
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

    if(player.animationManager->isPlaying(jumpSimon) && player.animationManager->getCurrentFrameIndex() == 1){
        player.isJumpStanding = true;
    }


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
    if (player.upgradeWhip){
        player.setState(state<WhipUpgrade>());
    }
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
            player.startInvulnerable = false;
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
}

void PlayerDuckState::hello(){
    std::cout << "PLAYER STATE: Duck" << std::endl;
}

// --------------------------------------------------------------



// ---------------------------- STAIRS ----------------------------

PlayerStairIdleState::PlayerStairIdleState() : PlayerState()
{
}

void PlayerStairIdleState::init(Player& player)
{
    player.isOnStairs = true;
    player.stairStepDistance = 0.f;

    if(player.isPositionedInStair){ // If player just arrived to the stair
        player.isStairUpRight = (player.stairStart->type == StairTile::Type::BOTTOM_LEFT ||
                                 player.stairStart->type == StairTile::Type::TOP_RIGHT);
        player.isPositionedInStair = false;
    }
}

void PlayerStairIdleState::handleInput(Player& player, sf::Event event)
{
    
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        // TODO: FALTA ATTACK STAIRS
        /*
        if (keyPressed->scancode == controls.attack) {
            player.isAttacking = true;
            player.setState(state<AttackStairs>());
        }
        */  
    }
}

void PlayerStairIdleState::update(Player& player, float deltaTime)
{
    auto controls = configManager.getControls();

    if (sf::Keyboard::isKeyPressed(controls.right)) {
        player.dir = RIGHT;
        player.isWalking = true;
        player.setState(state<StairWalk>());
    }
    
    if (sf::Keyboard::isKeyPressed(controls.left)) {
        player.dir = LEFT;
        player.isWalking = true;
        player.setState(state<StairWalk>());
    }

    if (sf::Keyboard::isKeyPressed(controls.up)) {
        player.isWalking = true;

        if(player.isStairUpRight){
            player.dir = RIGHT;
            player.setState(state<StairWalk>());

        } else {
            player.dir = LEFT;
            player.setState(state<StairWalk>());
        }
    }

    if (sf::Keyboard::isKeyPressed(controls.down)) {
        player.isWalking = true;

        if(player.isStairUpRight){
            player.dir = LEFT;
            player.setState(state<StairWalk>());

        } else {
            player.dir = RIGHT;
            player.setState(state<StairWalk>());
        }
    }

    if(player.isStairUpRight){
        if(player.dir == LEFT){
            player.currentAnimation = stairDescendIdleSimon;
        } else {
            player.currentAnimation = stairAscendIdleSimon;
        }

    } else {
        if(player.dir == RIGHT){
            player.currentAnimation = stairDescendIdleSimon;
        } else {
            player.currentAnimation = stairAscendIdleSimon;
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
            player.startInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }
    player.animationManager->update(deltaTime);

    if(player.isBeingHurt){
        player.setState(state<HurtStair>());
    }

    if(player.isNearStair && 
        (abs(player.sprite->getGlobalBounds().position.x - player.stairStart->hitboxes[0].position.x) < 0.01f)){
        player.stairStepDistance = 0.f;
        player.isDucking = false;
        player.isOnStairs = false;
        player.setState(state<Idle>());
    }
}

void PlayerStairIdleState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
}

void PlayerStairIdleState::end(Player& player)
{
}

void PlayerStairIdleState::hello(){
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
    player.isOnStairs = true;
    player.stairStepDistance = 8.f;
    player.isWalking = true;

    if(player.isPositionedInStair){ // If player just arrived to the stair
        player.isStairUpRight = (player.stairStart->type == StairTile::Type::BOTTOM_LEFT ||
                                 player.stairStart->type == StairTile::Type::TOP_RIGHT);
        player.isPositionedInStair = false;

        player.sprite->move({0.f, -1.f}); // Stairs are a bit taller than the ground
    }
}

void PlayerStairWalkState::handleInput(Player& player, sf::Event event)
{
}

void PlayerStairWalkState::update(Player& player, float deltaTime)
{
    /*
    if(!player.isOnStairs){
        player.setState(state<Idle>());

    } else if(!player.isWalking){
        player.setState(state<StairIdle>());
    }
    */


   
    if(player.dir == RIGHT){
        player.sprite->setScale(sf::Vector2f(-1.f, 1.f));
    } else {
        player.sprite->setScale(sf::Vector2f(1.f, 1.f));
    }

    float move = deltaTime * gPlayerStairSpeed;

    if(player.stairStepDistance < 1.f) {
        move = player.stairStepDistance;
    }
    
    if(player.isStairUpRight){
        if(player.dir == LEFT){
            player.sprite->move({-move, move});
            player.currentAnimation = stairDescendWalkSimon;
        } else {
            player.sprite->move({move, -move});
            player.currentAnimation = stairAscendWalkSimon;
        }

    } else {
        if(player.dir == RIGHT){
            player.sprite->move({move, move});
            player.currentAnimation = stairDescendWalkSimon;
        } else {
            player.sprite->move({-move, -move});
            player.currentAnimation = stairAscendWalkSimon;
        }
    }

    player.stairStepDistance = max(0.f, player.stairStepDistance - move);

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
            player.startInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }

    if (!player.animationManager->isPlaying(player.currentAnimation)){
        
        player.animationManager->playAnimation(player.currentAnimation);

    }

    player.animationManager->update(deltaTime);

    if (player.animationManager->isPlaying(player.currentAnimation) &&
        player.animationManager->isAnimationFinished() &&
        player.stairStepDistance == 0.f)
    {
        player.isWalking = false;
        //player.animationManager->setAnimationSpeed(1.0f); // 2x speed
        player.setState(state<StairIdle>());
    }

    if(player.isNearStair && 
        (abs(player.sprite->getGlobalBounds().position.x - player.stairStart->hitboxes[0].position.x) < 0.01f)){
        player.stairStepDistance = 0.f;
        player.isDucking = false;
        player.isOnStairs = false;
        player.isWalking = false;
        player.setState(state<Idle>());
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
    }
    */
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

        player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));
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

    if(player.restartJumpAnimation){
        if (!player.animationManager->isPlaying(player.currentAnimation)){
            player.animationManager->playAnimation(player.currentAnimation);
        }
    
        if (!player.whip.animationManager->isPlaying(whipLevelAnimation(player.whip.whipLvl))){
            player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));
        }
    }
    
    player.animationManager->update(deltaTime*1.5f);
    player.whip.animationManager->update(deltaTime*1.5f);

    
    if(player.restartJumpAnimation){
        if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
            int xoffset= 0;
            if(player.whip.whipLvl <3){
                xoffset = 24;
            }
            else{
                xoffset = 41;
            }
            if (player.dir == RIGHT) {
                player.whip.sprite->setPosition(
                    sf::Vector2f(player.sprite->getPosition().x + xoffset, // Adjust X offset
                                player.sprite->getPosition().y+2)  // Adjust Y offset
                );
                player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
            } else {
                player.whip.sprite->setPosition(
                    sf::Vector2f(player.sprite->getPosition().x - xoffset, // Adjust X offset
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
            player.startInvulnerable = false;
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
        player.sprite->setColor(sf::Color::White);
        player.whip.sprite->setColor(sf::Color::White);
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
    player.restartJumpAnimation = true;
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
    player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));
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
    if (!player.whip.animationManager->isPlaying(whipLevelAnimation(player.whip.whipLvl))){
    
        player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));

    }
    //player.animationManager->setAnimationSpeed(2.0f); // 2x speed
    //player.whip.animationManager->setAnimationSpeed(2.0f); // 2x speed
    player.animationManager->update(deltaTime*1.5f);
    player.whip.animationManager->update(deltaTime*1.5f);

    // Apply gravity
    player.verticalSpeed += gPlayerGravity * deltaTime;
    player.sprite->move({0.f, player.verticalSpeed * deltaTime});

    
    if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
        int xoffset= 0;
        if(player.whip.whipLvl <3){
            xoffset = 24;
        }
        else{
            xoffset = 41;
        }
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + xoffset, // Adjust X offset
                             player.sprite->getPosition().y+4)  // Adjust Y offset
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); // Flip whip to face right
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - xoffset, // Adjust X offset
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
            player.startInvulnerable = false;
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
        player.restartJumpAnimation = false;
        player.setState(state<AttackIdle>());
    }

    if (player.animationManager->isPlaying(attackSimon) && player.animationManager->isAnimationFinished())
    {
        player.isAttacking = false;
        player.attackedFinished = true;
        player.sprite->setColor(sf::Color::White);
        player.whip.sprite->setColor(sf::Color::White);
        player.whip.animationManager->playAnimation(whipNoAttack);
        player.isJumpStanding = true;
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

    player.isAttacking = true;
    
    player.isDucking = true;
    
    player.hasToPressAgain = false;
    player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));
    player.animationManager->playAnimation(attackFloorSimon);
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
    if (!player.whip.animationManager->isPlaying(whipLevelAnimation(player.whip.whipLvl))){
    
        player.whip.animationManager->playAnimation(whipLevelAnimation(player.whip.whipLvl));
    }
    
    player.animationManager->update(deltaTime*1.5f);
    player.whip.animationManager->update(deltaTime*1.5f);

    if (player.whip.animationManager->getCurrentFrameIndex() == 2 || player.whip.animationManager->getCurrentFrameIndex() == 3) {
        
        int xoffset= 0;
        if(player.whip.whipLvl <3){
            xoffset = 24;
        }
        else{
            xoffset = 41;
        }
        if (player.dir == RIGHT) {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x + xoffset, 
                             player.sprite->getPosition().y + 4)  
            );
            player.whip.sprite->setScale(sf::Vector2f(-1.f, 1.f)); 
        } else {
            player.whip.sprite->setPosition(
                sf::Vector2f(player.sprite->getPosition().x - xoffset, 
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
            player.startInvulnerable = false;
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
        player.sprite->setColor(sf::Color::White);
        player.whip.sprite->setColor(sf::Color::White);
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
        player.setState(state<StairIdle>());
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
            player.startInvulnerable = false;
            player.visible = true; // Ensure the player is visible after invulnerability ends
            player.invulnerableTimeCounter = 0.0f; // Reset the counter
            player.sprite->setColor(sf::Color::White);
            player.whip.sprite->setColor(sf::Color::White);
        } else {
            player.invulnerableTimeCounter += deltaTime;
        }
    }

}

void PlayerAttackStairState::draw(Player& player, sf::RenderWindow &window)
{
    window.draw(*player.sprite);
    window.draw(*player.whip.sprite);
}

void PlayerAttackStairState::end(Player& player)
{
    player.whip.animationManager->playAnimation(whipNoAttack);
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
    player.isJumpStanding = false;
    player.whip.animationManager->playAnimation(whipNoAttack);
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

    
    if (!player.startInvulnerable && player.isOnGround){
        player.startInvulnerable = true;
    }
    
    if(player.startInvulnerable){
        player.startInvulnerable = false;
        player.blinkTimer = 0.0f; 
        player.visible = true; 
        player.isJumping = false;
        player.isBeingHurt = false;
        player.invulnerableTimeCounter = 0.0f; 
        
        if (player.upgradeWhip){
            player.setState(state<WhipUpgrade>());
        }
        else{
            player.setState(state<Idle>());
        }
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

// ---------------------------- HURT STAIR ----------------------------

PlayerHurtStairState::PlayerHurtStairState() : PlayerState()
{
}

void PlayerHurtStairState::init(Player& player)
{
    player.startInvulnerable = true;
    player.isBeingHurt = true; 
    player.isInvulnerable = true;
    player.isOnStairs = true;
}

void PlayerHurtStairState::handleInput(Player& player, sf::Event event)
{
}

void PlayerHurtStairState::update(Player& player, float deltaTime)
{
    player.blinkTimer = 0.0f; 
    player.visible = true; 
    player.isJumping = false;
    player.isBeingHurt = false;
    player.invulnerableTimeCounter = 0.0f;
    player.setState(state<StairIdle>()); 
}

void PlayerHurtStairState::draw(Player& player, sf::RenderWindow &window)
{
    if(player.visible) {
        window.draw(*player.sprite);
    }
    
}

void PlayerHurtStairState::end(Player& player)
{
    player.startInvulnerable = false;
    player.isBeingHurt = false;
}

void PlayerHurtStairState::hello(){
    std::cout << "PLAYER STATE: Hurt Stair" << std::endl;
}

// --------------------------------------------------------------


// ---------------------------- DEAD ----------------------------

PlayerDeadState::PlayerDeadState() : PlayerState()
{
}

void PlayerDeadState::init(Player& player)
{
    player.currentAnimation = deathSimon;
    player.whip.animationManager->playAnimation(whipNoAttack);
    player.animationManager->playAnimation(player.currentAnimation);
    // Initial position adjustment for first frame
    player.isDead = true;
    player.whip.whipDmg = 1;
    player.whip.whipLvl = 1;
    player.isInvulnerable = true;
    player.deathRestart = true;
    player.isOnStairs = false;
    player.health = 0;
    player.isJumpStanding = false;
    
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
    player.startInvulnerable = false;
    player.invulnerableTimeCounter = player.invulnerableTime + 1;
    player.isDead = false;
    player.hasDied = false;
    //player.sprite->setColor(sf::Color::White);
    //player.whip.sprite->setColor(sf::Color::White);
    player.sprite->setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    player.sprite->setColor(sf::Color::White); 
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

   
    if (player.animationManager->getCurrentFrameIndex() == 2 && !player.weaponIsActive) {
        

        player.subWeapon.sprite->setScale(player.dir == RIGHT ? sf::Vector2f{1.f, 1.f} : sf::Vector2f{-1.f, 1.f});
        sf::Vector2f spawnPos = player.sprite->getPosition();
        spawnPos.y -= 30.f; // Adjust the Y position to be above Simon's head
        player.subWeapon.direction = player.dir;
        player.subWeapon.type = ItemType::DAGGER; // Depuracion
        //player.subWeapon.type = player.subWeaponType;
        
       if (player.subWeapon.type == ItemType::AXE) {
        player.subWeapon.verticalSpeed = -450.f; 
        player.subWeapon.horizontalSpeed = 150.f;
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(axeThrowing, player.subWeapon.axeFrames, true); 
        player.subWeapon.animationManager->playAnimation(axeThrowing); 

    } else if (player.subWeapon.type == ItemType::DAGGER) {
        player.subWeapon.verticalSpeed = 0.f;
        player.subWeapon.horizontalSpeed = 250.f;
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(daggerThrowing, player.subWeapon.daggerFrames, false); 
        player.subWeapon.animationManager->playAnimation(daggerThrowing); 
    } else if (player.subWeapon.type == ItemType::BOOMERANG) {
        //player.hearts++; // Depuracion
        player.subWeapon.horizontalSpeed = 100.f; 
        player.subWeapon.verticalSpeed = 0.f; 
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(boomerangThrowing, player.subWeapon.boomerangFrames, true); 
        player.subWeapon.animationManager->playAnimation(boomerangThrowing);

    } else if (player.subWeapon.type == ItemType::FIRE_BOMB) {
        player.subWeapon.verticalSpeed = -250.f;
        player.subWeapon.horizontalSpeed = 150.f;
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(fireBombThrowing, player.subWeapon.firebombFrames, false); 
        player.subWeapon.animationManager->playAnimation(fireBombThrowing);
        
    } else if (player.subWeapon.type == ItemType::STOPWATCH) {
        player.hearts -= 4; 
        // Flag stop time
    } 

        
        player.activeSubWeapons.push_back(std::move(player.subWeapon));
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
    if (player.animationManager->getCurrentFrameIndex() == 2 && !player.weaponIsActive) {
        

        player.subWeapon.sprite->setScale(player.dir == RIGHT ? sf::Vector2f{1.f, 1.f} : sf::Vector2f{-1.f, 1.f});
        sf::Vector2f spawnPos = player.sprite->getPosition();
        spawnPos.y -= 30.f; // Adjust the Y position to be above Simon's head
        player.subWeapon.direction = player.dir;
        player.subWeapon.type = ItemType::DAGGER; // Depuracion
        //player.subWeapon.type = player.subWeaponType;
        
       if (player.subWeapon.type == ItemType::AXE) {
        player.subWeapon.verticalSpeed = -450.f; 
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.horizontalSpeed = 150.f;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(axeThrowing, player.subWeapon.axeFrames, true); 
        player.subWeapon.animationManager->playAnimation(axeThrowing); 

    } else if (player.subWeapon.type == ItemType::DAGGER) {
        player.subWeapon.verticalSpeed = 0.f;
        player.subWeapon.horizontalSpeed = 350.f;
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(daggerThrowing, player.subWeapon.daggerFrames, false); 
        player.subWeapon.animationManager->playAnimation(daggerThrowing); 

    } else if (player.subWeapon.type == ItemType::BOOMERANG) {
        player.subWeapon.horizontalSpeed = 100.f; 
        player.subWeapon.verticalSpeed = 0.f; 
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(boomerangThrowing, player.subWeapon.boomerangFrames, true); 
        player.subWeapon.animationManager->playAnimation(boomerangThrowing);

    } else if (player.subWeapon.type == ItemType::FIRE_BOMB) {
        player.subWeapon.verticalSpeed = -250.f;
        player.subWeapon.horizontalSpeed = 150.f;
        player.subWeapon.changedDirection = false;
        player.subWeapon.intersected = false;
        player.subWeapon.animationManager = new AnimationManager(*player.subWeapon.sprite);
        player.subWeapon.sprite->setPosition(spawnPos);
        player.subWeapon.animationManager->addAnimation(fireBombThrowing, player.subWeapon.firebombFrames, false); 
        player.subWeapon.animationManager->playAnimation(fireBombThrowing);
        
    } else if (player.subWeapon.type == ItemType::STOPWATCH) {
        player.hearts -= 4; 
        // Flag stop time
    } 

        
        player.activeSubWeapons.push_back(std::move(player.subWeapon));
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

// ---------------------------- WHIP UPGRADE ANIMATION ----------------------------
PlayerWhipUpgradeState::PlayerWhipUpgradeState() : PlayerState()
{
}

void PlayerWhipUpgradeState::init(Player& player)
{
     player.visible = true;
     player.isInvulnerable = true;
}

void PlayerWhipUpgradeState::handleInput(Player& player, sf::Event event)
{
    
    
}

void PlayerWhipUpgradeState::update(Player& player, float deltaTime)
{
    //std::cout << "UPGRADE WHIP" << std::endl;
    player.currentAnimation = whipUpgrade;
    
    if (!player.animationManager->isPlaying(player.currentAnimation)){
        player.animationManager->playAnimation(player.currentAnimation);
    }
    if (!player.animationManager->isAnimationFinished())
    {
        player.animationManager->update(deltaTime);
    }
    else{
        player.upgradeWhip = false;
        player.isInvulnerable = false;
        player.sprite->setColor(sf::Color::White);
        player.whip.sprite->setColor(sf::Color::White);
        player.setState(state<Idle>());
        //std::cout << "UPGRADE WHIP END" << std::endl;
    }
    
    
    

}

void PlayerWhipUpgradeState::draw(Player& player, sf::RenderWindow &window)
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

void PlayerWhipUpgradeState::end(Player& player)
{

}

void PlayerWhipUpgradeState::hello(){
    std::cout << "PLAYER STATE: Whip upgrade" << std::endl;
}