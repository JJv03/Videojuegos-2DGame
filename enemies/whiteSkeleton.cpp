#include "whiteSkeleton.h"
#include "../game.h"
#include <iostream>
#include "createBone.h"

static std::random_device rd;       // we only want 1 instance of random_device
static std::mt19937 rng(rd());


const sf::Vector2f WhiteSkeleton::WHITE_SKELETON_SPEED = {85.0f, 0.0f};
const float WhiteSkeleton::WHITE_SKELETON_LIFE = 1.0f;
const float WhiteSkeleton::WHITE_SKELETON_SCORE = 400.0f;
const float WhiteSkeleton::WHITE_SKELETON_DAMAGE = 3.0f;
const float WhiteSkeleton::BONE_DAMAGE = 3.f;
const float WhiteSkeleton::DISTANCE_TO_PLAYER = 2.f * 32.f;
const float WhiteSkeleton::DISTANCE_TO_PLAYER_TO_JUMP = 32.f;
const int WhiteSkeleton::JUMP_CHANCE = 5;
const sf::Vector2f WhiteSkeleton::WHITE_SKELETON_JUMP_SPEED = {85.0, 300.0f};



WhiteSkeleton::WhiteSkeleton(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = WHITE_SKELETON_SPEED;
    life = WHITE_SKELETON_LIFE;
    score = WHITE_SKELETON_SCORE;
    damage = WHITE_SKELETON_DAMAGE;
 
    walkTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;

    atTheEdge = false;
    isPlayerRight = false;
    attackLow = false;
    startingJump = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Skeleton AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(whiteSkeletonWalk, this->walkFrames);

    this->animationManager = animationManager;
    currentAnimation = whiteSkeletonWalk;

    walksThisRound = 2;
    attacksThisRound = 1;

    currentState = State::IDLE;
    prevState = State::IDLE;
}

void WhiteSkeleton::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
            const sf::Vector2f &playerPos, const sf::FloatRect &mapBounds){

    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    if (playerActivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideActivationZone = true;
    }
    if (playerDeactivationZone.findIntersection(sprite->getGlobalBounds()).has_value())
    {
        enemyInsideDeactivationZone = true;
    }

    // If the player is outside the deactivation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideDeactivationZone)
    {
        needsPlayerToLeaveZone = false;
    }
    
    // We only activate if the player is in the area, the enemy is not active and the player has previously moved away
    if (enemyInsideActivationZone && !isActive && !needsPlayerToLeaveZone)
    {
        isActive = true;
    }

    // Deactivates if the enemy is active and has left the deactivation zone
    if (isActive && !enemyInsideDeactivationZone)
    {
        isActive = false;
        resetPosition();
    }
    
    if (isActive)
    {   
        sf::Vector2f position = sprite->getGlobalBounds().position;
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }
        
        if (currentState != State::JUMP) {
            isPlayerRight = sprite->getGlobalBounds().position.x <= playerPos.x;
        }

        if(isPlayerRight){
            sprite->setScale({-1.f, 1.f});
        } else {
            sprite->setScale({1.f, 1.f});
        }
        
        float distance = 0.f;
        
        distance = abs(position.x - playerPos.x);   

        switch(currentState){
            case State::IDLE:
                speed = sf::Vector2f(0.f, 0.f);
                idleTimeCounter += deltaTime;
                if (idleTimeCounter >= IDLE_TIME)
                {
                   
                    idleTimeCounter = 0.f;
                    if(walksThisRound <= 0)
                    {
                        walksThisRound = getNumberOfWalks();        // Reset walks for next round
                        attacksThisRound = getNumberOfAttacks();    // Set attacks for this round
                        currentState = State::ATTACKING;
                    } 
                    else if(atTheEdge)
                    {
                        bool jumpAway = (prevState == State::WALKINGAWAY) && shouldJump();
                        bool jumpClose = (prevState == State::WALKINGCLOSE) && shouldJump();
                                         //&& (distance >= DISTANCE_TO_PLAYER_TO_JUMP);
                        if (jumpAway || jumpClose){
                            currentState = State::JUMP;
                            startingJump = true;
                        }
                        else if(prevState == State::WALKINGAWAY){
                            currentState = State::WALKINGCLOSE;
                        }
                        else if(prevState == State::WALKINGCLOSE){
                            currentState = State::WALKINGAWAY;
                        }
                        else{
                            currentState = State::IDLE;
                        }
                        atTheEdge = false;
                    }
                    else
                    {
                        if(distance < DISTANCE_TO_PLAYER + 8){    // 32/4, 1/4 of a tile
                            currentState = State::WALKINGAWAY;
                        }
                        else{
                            currentState = State::WALKINGCLOSE;
                        }
                    }
                }
                break;
  
            case State::WALKINGAWAY:
                if(isPlayerRight){
                    speed = sf::Vector2f(-WHITE_SKELETON_SPEED.x, WHITE_SKELETON_SPEED.y);
                }
                else{
                    speed = sf::Vector2f(WHITE_SKELETON_SPEED.x, WHITE_SKELETON_SPEED.y);
                }
                
                walkTimeCounter += deltaTime;
                if(walkTimeCounter >= WALK_TIME || atTheEdge){
                    if(walksThisRound)
                    walkTimeCounter = 0.f;
                    prevState = currentState;
                    walksThisRound -= 1;
                    currentState = State::IDLE;
                }
                break;

            case State::WALKINGCLOSE:
                if(isPlayerRight){
                    speed = sf::Vector2f(WHITE_SKELETON_SPEED.x, WHITE_SKELETON_SPEED.y);
                }
                else{
                    speed = sf::Vector2f(-WHITE_SKELETON_SPEED.x, WHITE_SKELETON_SPEED.y);
                }
                
                walkTimeCounter += deltaTime;
                if(walkTimeCounter >= WALK_TIME || atTheEdge){
                    walkTimeCounter = 0.f;
                    prevState = currentState;
                    walksThisRound -= 1;
                    currentState = State::IDLE;
                }

                break;

            case State::JUMP: {
                float directionChange = (prevState == State::WALKINGCLOSE) ? -1.f : 1.f;

                if (startingJump) {
                    isOnGround = false;
                    if(isPlayerRight){
                        speed = sf::Vector2f(-WHITE_SKELETON_JUMP_SPEED.x * directionChange, WHITE_SKELETON_JUMP_SPEED.y);
                    }
                    else{
                        speed = sf::Vector2f(WHITE_SKELETON_JUMP_SPEED.x * directionChange, WHITE_SKELETON_JUMP_SPEED.y);
                    }
                    startingJump = false;
                }
                else {
                    float current_speed_x = speed.x;
                    if(isPlayerRight){
                        speed = sf::Vector2f(current_speed_x, speed.y);
                    }
                    else{
                        speed = sf::Vector2f(current_speed_x, speed.y);
                    }
                }
                
                if(isOnGround){
                    prevState = currentState;
                    currentState = State::IDLE;
                }
                break;
            }    
            case State::ATTACKING:
                speed = sf::Vector2f(0.f, 0.f);

                attackTimeCounter += deltaTime;
                if(attackTimeCounter >= ATTACK_TIME)
                {
                    attackTimeCounter = 0.f;
                    if(attacksThisRound > 0)
                    {
                        attacksThisRound -= 1;
                        attack(mapBounds);
                    }
                    else
                    {
                        currentState = State::IDLE;
                    }
                }
                walksThisRound = getNumberOfWalks();
                

                break;

            default:
                break;
        }

        applyGravity(deltaTime);
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }

        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }

        updateAnimation(deltaTime);

        isOnGround = false;
    }

    
    for(auto& bone : bones){
        if(bone && bone->sprite && bone->getActive()){
            bone->update(deltaTime);
        }
    }
}


void WhiteSkeleton::draw(sf::RenderWindow &window){
    if (sprite && isActive)
    {
        Enemy::draw(window);
    }
    for(auto& bone : bones){
        if(bone && bone->sprite && bone->getActive()){
            bone->draw(window);
        }
    }
}

void WhiteSkeleton::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);

        if (isOnGround && Enemy::checkForLedge(other))
        {
            atTheEdge = true;
        }
    }

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetSkeleton();
            needsPlayerToLeaveZone = true;
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetSkeleton();
            needsPlayerToLeaveZone = true;
        }
    }
}

void WhiteSkeleton::updateAnimation(float deltaTime){
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}

void WhiteSkeleton::resetSkeleton(){
    Enemy::resetPosition();

    speed = WHITE_SKELETON_SPEED;
    life = WHITE_SKELETON_LIFE;
    score = WHITE_SKELETON_SCORE;
    damage = WHITE_SKELETON_DAMAGE;
    
    walkTimeCounter = 0.f;
    idleTimeCounter = 0.f;
    attackTimeCounter = 0.f;

    atTheEdge = false;
    isPlayerRight = false;
    attackLow = false;
    startingJump = false;

    walksThisRound = 2;
    attacksThisRound = 1;

    currentAnimation = whiteSkeletonWalk;

    currentState = State::IDLE;
    prevState = State::IDLE;

    sprite->setScale({1.0f, 1.0f});
}
    
void WhiteSkeleton::resetPosition(){
    resetSkeleton();

    for(auto& b : bones){
        if(b) b->reset();
    }

}


void WhiteSkeleton::attack(const sf::FloatRect& mapBounds){
    sf::Vector2f bonePos = sprite->getGlobalBounds().position;

    if (isPlayerRight)
    { 
        bonePos.x += 8.f;
    }
    else
    {
        bonePos.x -= 8.f;
    }

    for(auto& bone : bones){
        if(!bone || !bone->getActive()){
            float verticalSpeed = 0.f;
            if(attackLow){
                verticalSpeed = getLowBoneSpeed();
            }
            else{
                verticalSpeed = getHighBoneSpeed();
            }

            bone = createBone(bonePos, mapBounds, isPlayerRight, verticalSpeed, BONE_DAMAGE);
            bone->setActive(true);
            break;
        }
    }

    attackLow = !attackLow; // Alternate for next attack
}

int WhiteSkeleton::getNumberOfWalks() {
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return (dis(rng) < 0.5f) ? 2 : 3;
}

int WhiteSkeleton::getNumberOfAttacks() {
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    float prob = dis(rng);
    if(prob < 0.7){
        return 1;
    } else if (prob < 0.9){
        return 2;
    } else {
        return 3;
    }
}


float WhiteSkeleton::getLowBoneSpeed(){
    std::uniform_real_distribution<float> dis(140.f, 210.f);
    return dis(rng);
}

float WhiteSkeleton::getHighBoneSpeed(){
    std::uniform_real_distribution<float> dis(235.f, 250.f);
    return dis(rng);
}

bool WhiteSkeleton::shouldJump() {
    std::uniform_int_distribution<int> dist(0, JUMP_CHANCE - 1);
    int randValue = dist(rng);
    return randValue == 0;  // 1/JUMP_CHANCE possibility of jumping
}

bool WhiteSkeleton::checkMapBoundaries(const sf::FloatRect &mapBounds)
{
    sf::FloatRect enemyBounds = sprite->getGlobalBounds();

    if (enemyBounds.position.x <= mapBounds.position.x ||
        enemyBounds.position.x + enemyBounds.size.x >= mapBounds.position.x + mapBounds.size.x ||
        enemyBounds.position.y + enemyBounds.size.y >= mapBounds.position.y + mapBounds.size.y)
    {
        isActive = false;
        return true;
    }

    return false;
}

void WhiteSkeleton::hello() const{
    std::cout << "Hello from WhiteSkeleton!" << std::endl;
}