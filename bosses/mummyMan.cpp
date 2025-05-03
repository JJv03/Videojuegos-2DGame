#include "mummyMan.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize mummyMan with sprite, hitboxes, position, and game level/stage
MummyMan::MummyMan(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = MUMMY_SPEED;
    life = MUMMY_LIFE;
    score = MUMMY_SCORE;
    damage = MUMMY_DAMAGE;

    gKilledBoss = false;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(mummyIdle, this->idleMummyFrames);
    animationManager->addAnimation(mummyWalk, this->walkMummyFrames);
    animationManager->addAnimation(mummyAttack, this->attackkMummyFrames,false);

    this->animationManager = animationManager;
    if (position.x < 1500.f)
    {
        sprite->setScale(sf::Vector2f(1.f, 1.f));
        facingRight = -1;
    }
    else{
        sprite->setScale(sf::Vector2f(-1.f, 1.f));
        facingRight = 1;
    }
    
    currentAnimation = mummyIdle;
}

// Update phantomBat logic: handle spawning, movement, and deactivation
void MummyMan::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
    const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds,const Player &player)
{
    // SPAWN LOGIC
    if (!isActive && !dead)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                maxLife = life;
                this->currentState = MummyState::IDLE;
                break;
            }
        }
    }

    int chance = rand() % 4;
    int speedP = (rand()%10)+1;
    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {   
        sf::Vector2f playerPos(player.sprite->getGlobalBounds().position.x + player.sprite->getGlobalBounds().size.x / 2, player.sprite->getGlobalBounds().position.y + player.sprite->getGlobalBounds().size.y / 2);
        position = sprite->getGlobalBounds().position;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        //float distance = playerPos.x - draculaSpiritPos.x;
        if(started){
            if (playerPos.x < position.x) {
                if(firstTime){
                    firstTime = false;
                    sprite->move(sf::Vector2f(-16.f, 0.f));
                }
                sprite->setScale(sf::Vector2f(1.f, 1.f)); // Negative X scale flips the sprite
                facingRight = -1;
            } 
            else {
                if(firstTime){
                    firstTime = false;
                    sprite->move(sf::Vector2f(16.f, 0.f));
                }
                sprite->setScale(sf::Vector2f(-1.f, 1.f));
                facingRight = 1;
            }
        }
        /**/
       switch (currentState)
       {
            case MummyState::WALK:
                    currentAnimation = mummyWalk;
                    sprite->move(sf::Vector2f(facingRight * deltaTime * speed.x, 0.f));
                    if (chance == 0 || attackWaitingCounter >= attackWaitingTime)
                    {
                        this->currentState = MummyState::ATTACK;
                        attackWaitingCounter = 0.f;
                    }
                    else{
                        attackWaitingCounter +=deltaTime;
                    }
                    
                /* code */
                break;
                case MummyState::ATTACK:
                    currentAnimation = mummyAttack;
                    if (animationManager->isAnimationFinished())
                    {
                        speed.x = MUMMY_SPEED.x * (speedP/10.f);
                        this->currentState = MummyState::WALK;
                    }
                    
                
                /* code */
                break;
            default: //IDLE
                    if(counterStarting>=time2Start){
                        this->currentState = MummyState::WALK;
                        counterStarting = 0.f;
                        started = true;
                        firstTime = true;
                    }
                    else{
                        counterStarting +=deltaTime;

                    }
        break;
       }
       

      
    }
    sf::FloatRect spriteBounds = sprite->getGlobalBounds();
    hitboxes[0] = spriteBounds;
    updateAnimation(deltaTime);
}




void MummyMan::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    currentBossLife = life;
}

// Render phantomBat and debug info (spawn zone)
void MummyMan::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Boss::draw(window);
    }
}

// Update animation frame and flip sprite based on direction
void MummyMan::updateAnimation(float deltaTime)
{
    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);
}

void MummyMan::resetPosition()
{
    Boss::resetPosition();

    speed = MUMMY_SPEED;
    life = MUMMY_LIFE;

    currentAnimation = mummyIdle;
    dead = false;
    starting = true;
    attacking = false;
}

void MummyMan::hello() const
{
    std::cout << "Soy MummyMan" << std::endl;
}
