#include "player.h"
#include "globals.h"
#include <iostream>
#include "enemies/enemy.h"
#include "game.h"
#include "item.h"
#include "enemies/projectile.h"
#include <cmath>

Player::Player()
{
    activeState = std::make_unique<PlayerIdleState>();
    dir = RIGHT;

    acceptsInput = true;

    isOnGround = false;
    isOnStairs = false;
    hasToPressAgain = true;
    isAttacking = false;
    isWalking = false;
    isJumping = false;
    isDucking = false;
    isDead = false;
    isBeingHurt = false;
    attackedFinished = false;
    verticalSpeed = 0.0f;
    horizontalSpeed = 0.0f;
    isPressingUp = false;
    hasDied = false;
    deathRestart = false;
    autoWalkDistance = 0.f;
    upgradeWhip = false;
    isJumpStanding = false;
    restartJumpAnimation = true;

    lastHeight = 0.f;
    hasFallen = false;
 
    // Secondary weapon
    weaponIsActive = false;

    // Stats
    maxHealth = 16;
    health = maxHealth;
    lives = 3;
    score = 0;
    hearts = 5;

    // Interactions
    isInvulnerable = false;
    startInvulnerable = false;
    invulnerableTime = 3.0f; // 3 segs
    invulnerableTimeCounter = 0.0f;
    isPassingObject = false;
    blinkTimer = 0.0f;
    blinkInterval = 0.05f; // 0.1 segs
    visible = true;

    // Invisibility
    isInvisible = false;
    invisibilityTimeCounter = 0.0f;
    invisibilityTime = 5.0f;

    // Rosario
    activateRosario = false;

    // StopWatch
    isStopWatchActive = false;
    stopWatchTimeCounter = 0.0f;
    stopWatchTime = 5.0f; // 5 segs

    // Stairs
    isNearStair = false;
    isPositionedInStair = false;
    isStairUpRight = false;
    stairStepDistance = 0.f;
    stairStart = new StairTile();
}

void Player::handleInput(sf::Event event)
{
    getActiveState()->handleInput(*this, event);
}

void Player::update(float deltaTime, const sf::Vector2f &viewPosition, bool windowHasFocus)
{
    getActiveState()->update(*this, deltaTime, windowHasFocus);
    updateActivationZones(viewPosition);
    updateActiveSubWeapons(deltaTime, viewPosition);

    if(this->activateRosario && this->rosarioTimeCounter < this->invulnerableTime){
        this->rosarioTimeCounter += deltaTime;
    }
    else{
        this->activateRosario = false;
    }
    // If player is near stair, collisions will make it true
    this->isNearStair = false;
    this->stairStart = new StairTile();

    if(!this->isBeingHurt && !this->isDead){
        if (this->isInvulnerable && !this->isInvisible)
        {
            this->blinkTimer += deltaTime;
            if (this->blinkTimer >= this->blinkInterval) {
                this->visible = !this->visible; // Toggle visibility
                this->blinkTimer = 0.0f; // Reset the timer
                this->sprite->setColor(this->visible ? sf::Color::White : sf::Color(255, 255, 255, 128));
            }
            
            if (this->invulnerableTimeCounter >= this->invulnerableTime) {
                this->isInvulnerable = false;
                this->startInvulnerable = false;
                this->visible = true; // Ensure the player is visible after invulnerability ends
                this->invulnerableTimeCounter = 0.0f; // Reset the counter
                this->sprite->setColor(sf::Color::White);
            } else {
                this->invulnerableTimeCounter += deltaTime;
            }
        }
        else if (this->isInvulnerable && this->isInvisible)
        {
            std::cout << "Invisible and Invulnerable" << std::endl;
            if (this->invisibilityTimeCounter >= this->invisibilityTime) {
                this->isInvulnerable = false;
                this->startInvulnerable = false;
                this->isInvisible = false;
                this->visible = true; // Ensure the player is visible after invulnerability ends
                this->invisibilityTimeCounter = 0.0f; // Reset the counter
                this->sprite->setColor(sf::Color::White);
            } else {
                this->sprite->setColor(sf::Color(255, 255, 255, 128));
                this->invisibilityTimeCounter += deltaTime;
            }
        }
        
    }
}

void Player::draw(sf::RenderWindow &window)
{
    getActiveState()->draw(*this, window);
    /*for (auto& subW : activeSubWeapons) {
        window.draw(*subW.sprite);
    }*/
   if (this->weaponIsActive)
   {
        window.draw(*this->subWeapon.sprite);
   }
   
    
}

void Player::setState(PlayerStateRef newState)
{
    //newState->hello();

    this->activeState->end(*this);
    this->activeState = std::move(newState);
    this->activeState->init(*this);
}

PlayerStateRef &Player::getActiveState()
{
    return activeState;
}

void Player::updateActivationZones(const sf::Vector2f &viewPosition)
{
    // Size of the visible game view
    float activationWidth = gGameVisibleWorld_size_x;
    float activationHeight = gGameVisibleWorld_size_y;
    float deactivationWidth = activationWidth * 1.6f;
    float deactivationHeight = activationHeight * 1.6f;

    // Center of current view
    sf::Vector2f centerPos = {
        viewPosition.x + activationWidth / 2.0f,
        viewPosition.y + activationHeight / 2.0f};

    // Activation focused on the center of the view
    gPlayerActivationZone = sf::FloatRect(
        {centerPos.x - activationWidth / 2.0f,
         centerPos.y - activationHeight / 2.0f},
        {activationWidth, activationHeight});

    gPlayerDeactivationZone = sf::FloatRect(
        {centerPos.x - deactivationWidth / 2.0f,
         centerPos.y - deactivationHeight / 2.0f},
        {deactivationWidth, deactivationHeight});
}

PlayerPosition Player::getPlayerOffsetPosition()
{
    const sf::FloatRect playerBounds = this->sprite->getGlobalBounds();
    const sf::Vector2f playerCenter = {
        playerBounds.position.x + playerBounds.size.x / 2.f,
        playerBounds.position.y + playerBounds.size.y / 2.f};

    sf::Vector2f zoneCenter = {
        gPlayerActivationZone.position.x + gPlayerActivationZone.size.x / 2.0f,
        gPlayerActivationZone.position.y + gPlayerActivationZone.size.y / 2.0f};

    float toleranceX = gPlayerActivationZone.size.x * 0.05f;

    if (playerCenter.x < zoneCenter.x - toleranceX)
    {
        return PlayerPosition::LEFT;
    }
    else if (playerCenter.x > zoneCenter.x + toleranceX)
    {
        return PlayerPosition::RIGHT;
    }

    return PlayerPosition::CENTER;
}

std::vector<sf::FloatRect> Player::getBounds() const
{
    return std::vector<sf::FloatRect>({animationManager->getGlobalBounds()});
}

void Player::onCollision(Entity &other, Game &game)
{
    if (dynamic_cast<SolidTile *>(&other))
    {
        if(!this->isOnStairs){ // If Simon is on stairs, ignore collisions with solid tiles
            // std::cout << "Es un SolidTile\n";
            if (!this->onCollision_SolidTile(other))
            {
                this->isOnGround = false; // If Simon is not colliding with any solid tile
            }
        }
    }
    else if (Enemy *enemy = dynamic_cast<Enemy *>(&other))
    {   
        if(!this->isInvulnerable && !this->isDead){
            this->health = std::max(this->health - enemy->damage, 0.f);

            if (this->health > 0)
            {
                this->isInvulnerable = true;

                if(this->isOnStairs){
                    this->isBeingHurt = true;
                    this->setState(std::make_unique<PlayerHurtStairState>());
                } else {
                    this->isJumping = true;
                    this->verticalSpeed = -gPlayerJumpForce;
                    this->isOnGround = false;
                    this->setState(std::make_unique<PlayerHurtState>());
                }
            }
            else{
                this->setState(std::make_unique<PlayerDeadState>());
            }
        }
    }
    else if (Projectile *projectile = dynamic_cast<Projectile *>(&other))
    {   
        if(!this->isInvulnerable && !this->isDead){
            this->health = std::max(this->health - projectile->damage, 0.f);

            if (this->health > 0)
            {
                this->isInvulnerable = true;

                if(this->isOnStairs){
                    this->isBeingHurt = true;
                    this->setState(std::make_unique<PlayerHurtStairState>());
                } else {
                    this->isJumping = true;
                    this->verticalSpeed = -gPlayerJumpForce;
                    this->isOnGround = false;
                    this->setState(std::make_unique<PlayerHurtState>());
                }
            }
            else{
                this->setState(std::make_unique<PlayerDeadState>());
            }
        }
    }
    else if (DoorTile* doorTile = dynamic_cast<DoorTile*>(&other))
    {
        if(!this->isDead){
            game.activateDoorTile(doorTile->doorId);
        }
    }
    else if (StairTile* stairTile = dynamic_cast<StairTile*>(&other))
    {
        this->isNearStair = true;
        this->stairStart = stairTile;
    }    
    else if (dynamic_cast<Item*>(&other)){
        if(!this->isDead){
            this->onCollision_Item(other);
        }
    }
    else {
        //std::cout << "No se\n"; 
    }
}

bool Player::onCollision_SolidTile(Entity &solidTile)
{
    sf::FloatRect playerBounds = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    bool hasCollided = this->isOnGround;
    
    for (auto tileBound : tileBounds)
    {
        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBound))
        {
            const float overlapX = intersection->size.x;
            const float overlapY = intersection->size.y;

            bool simonIsTouchingGround = false;
            if (overlapY < 7.66f) {
                simonIsTouchingGround = ((playerBounds.position.y + playerBounds.size.y) < (tileBound.position.y + tileBound.size.y));
            }

            if (overlapX < overlapY && (! simonIsTouchingGround)) // Horizontal collision
            {
                if ((playerBounds.position.x + playerBounds.size.x * 0.5f) < (tileBound.position.x + tileBound.size.x * 0.5f))
                {
                    
                    //std::cout << "Right collision" << std::endl;
                    this->sprite->move({-overlapX, 0.f});
                    playerBounds.position.x -= overlapX;
                }
                else
                {
                    //std::cout << "Left collision" << std::endl;
                    this->sprite->move({overlapX, 0.f});
                    playerBounds.position.x += overlapX;
                }
            }
            else // Vertical collision
            {
                if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBound.position.y + tileBound.size.y * 0.5f))
                { // Simon's feet are collisioning with the tile
                    
                    if (this->verticalSpeed >= 0.0f)
                    //std::cout << "Floor collision" << std::endl;

                    // if (this->verticalSpeed >= 0.0f)        // CUANDO ESTÉN TODO CON HITBOXES BUENAS
                    { // If player is NOT going up

                        // Option 1: adjust overlapedY and make it be .15f
                        // float theoreticallyCorrectPositionY = this->sprite->getPosition().y - overlapY;
                        // float targetPositionY = static_cast<int>(theoreticallyCorrectPositionY) + gSimonFeetCollisionNewHeight;
                        // float moveY = theoreticallyCorrectPositionY - targetPositionY;

                        // Option 2: read the tile and place the player on position.y of it, regardless of anything else
                        float targetPositionY = tileBound.position.y - playerBounds.size.y + gSimonFeetCollisionNewHeight;
                        float moveY = targetPositionY - playerBounds.position.y;

                        this->sprite->move({0.f, moveY});
                        playerBounds.position.y += moveY;
                        this->verticalSpeed = 0.0f; // (For security) Simon stops falling
                        this->isOnGround = true;    // Set Simon to be on ground

                        if(tileBound.position.y - this->lastHeight > 32.f){
                            this->hasFallen = true;
                        }

                        this->lastHeight = tileBound.position.y;
                        hasCollided = true;
                    }
                }
                else // Simon's head is collisioning with the tile
                {
                    if (overlapY >= 1.f){
                        //std::cout << overlapY << ", " << overlapX << std::endl;
                        //std::cout << "Ceiling collision" << std::endl;
                        this->sprite->move({0.f, overlapY});
                        playerBounds.position.y += overlapY;
                        this->verticalSpeed = 0.0f; // (For security) Simon starts falling
                    }
                }
            }
        }
    }
    

    /*
    for (auto tileBound : tileBounds){
        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBound)){
            const float overlapX = intersection->size.x;
            const float overlapY = intersection->size.y;

             // Checks the direction in which the tile is intersecting with the player
            sf::Vector2f playerCenter = {
                playerBounds.position.x + playerBounds.size.x / 2.f,
                playerBounds.position.y + playerBounds.size.y / 2.f
            };

            sf::Vector2f tileCenter = {
                tileBound.position.x + tileBound.size.x / 2.f,
                tileBound.position.y + tileBound.size.y / 2.f
            };

            float deltaX = playerCenter.x - tileCenter.x;
            float deltaY = playerCenter.y - tileCenter.y;

            if (overlapX > 0 && overlapY > 0)
            {
                if(deltaY < 0 && overlapX > 1.f) // Player is above the tile
                {
                    //std::cout << "Floor collision" << std::endl;                 
                    float targetPositionY = tileBound.position.y - playerBounds.size.y + 1.f; // Ajuste pequeño para asegurarse de que Simón quede encima
                    float moveY = targetPositionY - playerBounds.position.y;
                    this->sprite->move({0.f, moveY});
                    playerBounds.position.y += moveY;

                    this->verticalSpeed = 0.f;  // Detener la caída (Simón está en el suelo)
                    this->isOnGround = true;    // Establecer que Simón está en el suelo
                    hasCollided = true;
                }
                else
                {
                    // Horizontal collision
                    if (overlapX < overlapY)
                    {

                        if (deltaX < 0)  // Right collision (Player -> Tile)
                        {
                            //std::cout << "Right collision" << std::endl;                 

                            this->sprite->move({-overlapX, 0.f});
                            playerBounds.position.x -= overlapX;
                        }
                        else // Left collision (Tile <- Player)
                        {
                            //std::cout << "Left collision" << std::endl;                 

                            this->sprite->move({overlapX, 0.f});
                            playerBounds.position.x += overlapX;
                        }
                    }
                    // Vertical collision
                    else
                    {
                        if (deltaY > 0) // Ceiling collision (Floor collision already checked)
                        {    
                            //std::cout << "Ceiling collision" << std::endl; 
                            this->sprite->move({0.f, overlapY});  // Empujamos a Simón hacia abajo
                            playerBounds.position.y += overlapY;

                            this->verticalSpeed = 0.f;
                        }               
                            
                    }
                }
            }
        }
    }
    */

    return hasCollided;
}

void Player::onCollision_Item(Entity &entityItem)
{
    Item* item = dynamic_cast<Item*>(&entityItem);
    if (item == nullptr) return; // Impossible

    ItemType itemType = item->getType();

    if (isSubweaponItem(itemType))
    {
        this->subWeaponType = itemType;
    }
    else if (itemType == ItemType::MORNING_STAR)
    {
        //std::cout << "MORNING STAR" << std::endl;
        this->upgradeWhip = true;
        this->isInvulnerable = true;
        this->setState(std::make_unique<PlayerWhipUpgradeState>());
        if (this->whip.whipLvl < 6 )
        {
            this->whip.whipLvl++;
        }
        if (this->whip.whipDmg < 3)
        {
            this->whip.whipDmg++;
        }
        
    }
    else if (itemType == ItemType::SMALL_HEART)
    {
        this->hearts += 1;
        /*this->invisibilityTimeCounter = 0.0f; // Reset the counter
        this->isInvisible = true;
        this->isInvulnerable = true;*/
    }
    else if (itemType == ItemType::LARGE_HEART)
    {
        this->hearts += 5;
        /*this->invisibilityTimeCounter = 0.0f; // Reset the counter
        this->isInvisible = true;
        this->isInvulnerable = true;*/
    }
    else if (isScoringItem(itemType))
    {
        this->score += getItemScore(itemType);
    }
    else if (itemType == ItemType::INVISIBILITY_POTION) {
        this->invisibilityTimeCounter = 0.0f; // Reset the counter
        this->isInvisible = true;
        this->isInvulnerable = true;
    }
    else if (itemType == ItemType::PORK_CHOP) {
        this->health += 6;
        if (this->health > this->maxHealth)
            this->health = this->maxHealth;
    }
    // else if (itemType == ItemType::DOUBLE_SHOT)
    // {
    //     this->subWeaponType = ItemType::DOUBLE_SHOT;
    // }
    // else if (itemType == ItemType::TRIPLE_SHOT)
    // {
    //     this->subWeaponType = ItemType::TRIPLE_SHOT;
    // }
    else if (itemType == ItemType::MAGIC_CRYSTAL) {
        return;
    }
    else if (itemType == ItemType::ONEUP)
    {
        this->lives += 1;
    }
    else if (itemType == ItemType::ROSARY)
    {
        this->activateRosario = true;
        this->isInvulnerable = true;
    }
    
}

void Player::hello() const {
    std::cout << "Soy Player" << std::endl;
}

void Player::updateActiveSubWeapons(float deltaTime, const sf::Vector2f &viewPosition) {
    if (!this->weaponIsActive)
    {
        return;
    }
    
    if(!this->subWeapon.intersected){
        if (this->subWeapon.type == ItemType::AXE)
        {
            this->subWeapon.sprite->move((this->subWeapon.direction == RIGHT) ? sf::Vector2f(this->subWeapon.horizontalSpeed*deltaTime, 0.f) : sf::Vector2f(-this->subWeapon.horizontalSpeed*deltaTime, 0.f)); 
            this->subWeapon.verticalSpeed += gPlayerGravity * deltaTime * 3.5f;
            this->subWeapon.sprite->move(sf::Vector2f(0.f, this->subWeapon.verticalSpeed * deltaTime));
            if (this->subWeapon.animationManager && !this->subWeapon.animationManager->isPlaying(axeThrowing)) {
                this->subWeapon.animationManager->playAnimation(axeThrowing);
            }
            this->subWeapon.animationManager->update(deltaTime);

        }else if (this->subWeapon.type == ItemType::FIRE_BOMB){
            if (!this->subWeapon.intersectedBomb){
                this->subWeapon.sprite->move((this->subWeapon.direction == RIGHT) ? sf::Vector2f(this->subWeapon.horizontalSpeed*deltaTime, 0.f) : sf::Vector2f(-this->subWeapon.horizontalSpeed*deltaTime, 0.f)); 
                this->subWeapon.verticalSpeed += gPlayerGravity * deltaTime;
                this->subWeapon.sprite->move(sf::Vector2f(0.f, this->subWeapon.verticalSpeed * deltaTime));
                // Check if it collides, if so, explode
                
                this->subWeapon.isExploding = false;

            }
            
            else
            {
                //std::cout << "Fire bomb exploding" << std::endl;
                if (this->subWeapon.animationManager && !this->subWeapon.animationManager->isPlaying(fireBombThrowing)) {
                    this->subWeapon.animationManager->playAnimation(fireBombThrowing);
                }
                this->subWeapon.animationManager->update(deltaTime);
                if (this->subWeapon.animationManager->isAnimationFinished()){
                    //std::cout << "Fire bomb finished exploding" << std::endl;
                    this->subWeapon.isExploding = true;
                }
                
            }
            
            

        }else if(this->subWeapon.type == ItemType::BOOMERANG)
        {
            if ((this->subWeapon.sprite->getPosition().x + 20 >= viewPosition.x + gGameVisibleWorld_size_x || 
                    this->subWeapon.sprite->getPosition().x -10  <= viewPosition.x  ) && !this->subWeapon.changedDirection) { 
                this->subWeapon.changedDirection = true;
                this->subWeapon.horizontalSpeed = -this->subWeapon.horizontalSpeed;
            }
            this->subWeapon.sprite->move((this->subWeapon.direction == RIGHT) ? sf::Vector2f(this->subWeapon.horizontalSpeed*deltaTime, 0.f) : sf::Vector2f(-this->subWeapon.horizontalSpeed*deltaTime, 0.f)); 
            if (this->subWeapon.animationManager && !this->subWeapon.animationManager->isPlaying(boomerangThrowing)){
                this->subWeapon.animationManager->playAnimation(boomerangThrowing);
            }
            this->subWeapon.animationManager->update(deltaTime);

            // Hanlde boomerang colision with player
            if (isIntersecting(this->subWeapon.sprite->getGlobalBounds(), this->sprite->getGlobalBounds()) && this->subWeapon.changedDirection) {
                //std::cout << "Boomerang colision with player" << std::endl;
                this->subWeapon.intersected = true;
                
            }
            
            
        }
        else if (this->subWeapon.type == ItemType::STOPWATCH){
            if (this->stopWatchTimeCounter >= this->stopWatchTime) {
                this->isStopWatchActive = false;
                this->stopWatchTimeCounter = 0.0f; // Reset the counter
                this->subWeapon.intersected = true;
            } else {
                this->stopWatchTimeCounter += deltaTime;
            }

            
        }
        else if(this->subWeapon.type == ItemType::DAGGER)
        { // Dagger
            if (this->subWeapon.intersected)
            {
                this->subWeapon.animationManager->playAnimation(subweaponNoAttack);
            }
            else{
                this->subWeapon.sprite->move((this->subWeapon.direction == RIGHT) ? sf::Vector2f(this->subWeapon.horizontalSpeed*deltaTime, 0.f) : sf::Vector2f(-this->subWeapon.horizontalSpeed*deltaTime, 0.f)); 
                if (this->subWeapon.animationManager && !this->subWeapon.animationManager->isPlaying(daggerThrowing)){
                    this->subWeapon.animationManager->playAnimation(daggerThrowing);
                }
                this->subWeapon.animationManager->update(deltaTime);
            }
            
            
        }
        else{
            std::cout << "Subweapon type not recognized" << std::endl;
        }
    
    
    }

    bool isOutOfBounds =    this->subWeapon.sprite->getPosition().x >= viewPosition.x + gGameVisibleWorld_size_x || 
                            this->subWeapon.sprite->getPosition().x <= viewPosition.x ||
                            this->subWeapon.sprite->getPosition().y >= viewPosition.y + gGameVisibleWorld_size_y + 30.f;

    

    if (isOutOfBounds || this->subWeapon.intersected || this->subWeapon.isExploding)  {
        
        //this->subWeapon.sprite->setPosition({ -100.f,0.f}); 
        this->weaponIsActive = false;
        this->subWeapon.horizontalSpeed = 0.f;
        this->subWeapon.verticalSpeed = 0.f;
        this->subWeapon.isExploding = false;
        this->subWeapon.intersected = false;
        this->subWeapon.isExploding = false;
        this->subWeapon.intersectedBomb = false;
        this->subWeapon.changedDirection = false;
        this->subWeapon.animationManager->playAnimation(subweaponNoAttack);
        this->subWeapon.collisionedEntities.clear(); 
        return;
    }

    
}


// ----------------------------- WHIP -----------------------------
Whip::Whip()
{
    whipState = 0;
    whipFrames = 6;
    whipDmg = 1;
    whipLvl = 1;
}

std::vector<sf::FloatRect> Whip::getBounds() const
{
    if(this->animationManager->getCurrentFrameIndex() == 2 || this->animationManager->getCurrentFrameIndex() == 3){
        return std::vector<sf::FloatRect>({animationManager->getGlobalBounds()});
    }

    return std::vector<sf::FloatRect>({sf::FloatRect()});
}

void Whip::onCollision(Entity &other, Game &game)
{
}

void Whip::hello() const {
    std::cout << "Soy Whip" << std::endl;
}


// ----------------------------- SUBWEAPON -----------------------------
SubWeapon::SubWeapon()
{
    type = ItemType::NONE;
    subDamage = 3.0f;
}

std::vector<sf::FloatRect> SubWeapon::getBounds() const
{
    if(this->type != ItemType::NONE && !this->animationManager->isPlaying(subweaponNoAttack)){
        return std::vector<sf::FloatRect>({animationManager->getGlobalBounds()});
    }
    return std::vector<sf::FloatRect>();
}

void SubWeapon::onCollision(Entity &other, Game &game)
{
    
    if (dynamic_cast<Enemy *>(&other) && this->type == ItemType::DAGGER)
    {
        //std::cout << "SubWeapon colision" << std::endl;
        this->intersected = true;
        
        
    }
    else if (dynamic_cast<SolidTile *>(&other) && this->type == ItemType::FIRE_BOMB && !this->intersectedBomb)
    {
        //std::cout << "SubWeapon colision solid" << std::endl;
        if (this->onCollision_SolidTile(other))
        {
            this->intersectedBomb = true;
        }

    }
    
 
}

bool SubWeapon::onCollision_SolidTile(Entity &solidTile) {
    sf::FloatRect bombBounds = this->sprite->getGlobalBounds();
    std::vector<sf::FloatRect> tileBounds = solidTile.getBounds();

    bool hasCollided = false;

    for (auto tileBound : tileBounds) {
        if (const std::optional<sf::FloatRect> intersection = bombBounds.findIntersection(tileBound)) {
            const float overlapX = intersection->size.x;
            const float overlapY = intersection->size.y;

            // For bombs, we only care about first solid tile collision
            if (overlapX > 0 && overlapY > 0) {
                // Adjust bomb position to be exactly at collision point
                if (overlapX < overlapY) { // Horizontal collision
                    if ((bombBounds.position.x + bombBounds.size.x * 0.5f) < 
                        (tileBound.position.x + tileBound.size.x * 0.5f)) {
                        // Right-side collision
                        this->sprite->move({-overlapX, 0.f});
                    } else {
                        // Left-side collision
                        this->sprite->move({overlapX, 0.f});
                    }
                } else { // Vertical collision
                    if ((bombBounds.position.y + bombBounds.size.y * 0.5f) < 
                        (tileBound.position.y + tileBound.size.y * 0.5f)) {
                        // Top collision (floor)
                        float moveY = (tileBound.position.y - bombBounds.size.y) - bombBounds.position.y;
                        this->sprite->move({0.f, moveY});
                    } else {
                        // Bottom collision (ceiling)
                        this->sprite->move({0.f, overlapY});
                    }
                }
                
                hasCollided = true;
                break; // Only need one collision for bombs
            }
        }
    }

    return hasCollided;
}

void SubWeapon::hello() const {
    std::cout << "Soy SubWeapon" << std::endl;
}
