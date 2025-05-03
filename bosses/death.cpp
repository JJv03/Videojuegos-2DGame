#include "death.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
Death::Death(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = DEATH_SPEED;
    life = DEATH_LIFE;
    score = DEATH_SCORE;
    damage = DEATH_DAMAGE;

    gKilledBoss = false;
    goingDown = true;
    attacking = false;
    moving = false;

    up = true;
    right = false;
    dead = false;

    starting = true;
    timer = 0.f;
    doubleMoveTimer = 0.f;
}

// Update Death logic: handle spawning, movement, and deactivation
void Death::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
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
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {   
        // std::cout << "GOAL: " << goal.x << " " << goal.y << std::endl;
        position = sprite->getGlobalBounds().position;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        // sf::Vector2f map = mapDims.position;
        // sf::Vector2f size = mapDims.size;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;

        timer += deltaTime;
        if(starting){
            if (timer >= sleepInterval) {
                timer = 0.0f;
                starting = false;
                goingDown = true;
                goal = sf::Vector2f(210, 46);
                // std::cout << "Map pos: " << map.x << " " << map.y << std::endl;
                // std::cout << "Map size: " << size.x << " " << size.y << std::endl;
                // std::cout << "Goal tras dormir: " << goal.x << " " << goal.y << std::endl;
                getLinelSpeed(moveInterval);
            }
        }
        else{
            if(goingDown){
                std::cout << "Abajo" << std::endl;
                if (timer >= moveInterval) {    // Arrived to the goal
                    timer = 0;
                    speed = sf::Vector2f(0, 0);
                    goingDown = false;
                    attacking = true;
                }
            }
            else{
                if(attacking){
                    // ATTACK LOGIC
                    std::cout << "Attack" << std::endl;
                    if (timer >= attackInterval) {    // Attack timer
                        timer = 0;
                        attacking = false;
                        moving = true;
                        selectObjective();
                    }
                }
                else{
                    if(moving){
                        std::cout << "Moving" << std::endl;
                        if (timer >= moveInterval) {    // Move timer
                            timer = 0;
                            moving = false;
                            speed = sf::Vector2f(0, 0);
                        }
                        else{
                            getDoubleSpeed();
                            doubleMoveTimer += deltaTime;
                        }
                    }
                    else{
                        std::cout << "Waiting" << std::endl;
                        if(timer >= waitInterval){
                            timer = 0;
                            attacking = true;
                        }
                    }
                }
            }
        }

        // POSICIÓN DE SIMON PARA SABER DIRECCIÓN EN LA QUE MIRAR
        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }
    }
}

void Death::enhancedAI(bool isOn, const int playerDir, const sf::FloatRect &playerBounds){
    if(isOn && !triedAI){
        triedAI = true;     // To prevent permatrying
        int chance = rand() % 3;
        if(chance == 0){ // Meter factor cercanía del jugador
            // std::cout << "Luckyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy" << std::endl;
            enhancedActivated = true;
            enhancedTimer = 0;
        }
    }
    // In case Enhanced AI mode activate add this possible state (it has a prob to happen, not always). Attack with all scythe
}

void Death::getLinelSpeed(float timeToMove){
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;

    speed = sf::Vector2f(deltaX / timeToMove, -deltaY / timeToMove);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void Death::getDoubleSpeed() {
    float t = doubleMoveTimer / moveInterval;
    if (t > 1.f) t = 1.f;

    // horizontal component: constant velocity
    float totalDx   = goal.x - startPosition.x;
    float xVel      = totalDx / moveInterval;

    // “linear” vertical component
    float totalDy   = goal.y - startPosition.y;
    float yVelLin   = totalDy / moveInterval;

    // Parabolic arch (inverted U): 
    // f(t) = 4*H*(t-0.5)^2 - H => Extra vertical part
    // f'(t) = d/dt = 8*H*(t - 0.5)
    float arcDeriv = 8.f * arcHeight * (t - 0.5f);
    float yVelArc  = arcDeriv / moveInterval;

    float yVel = yVelLin + yVelArc;

    speed = sf::Vector2f(xVel, -yVel);
    // std::cout << "Double speed: " << speed.x << " " << speed.y << std::endl;
}

void Death::selectObjective(){
    doubleMoveTimer = 0.f;
    startPosition = position;
    if(up){
        goal = sf::Vector2f(25, 105);
        up = false;
        right = false;
    }
    else{
        if(right){
            goal = sf::Vector2f(25, 105);
            right = false;
        }
        else{
            goal = sf::Vector2f(198, 105);
            right = true;
        }
    }
}

void Death::onCollision(Entity &other, Game &game)
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

// Render Death and debug info (spawn zone)
void Death::draw(sf::RenderWindow &window)
{
    if (sprite && isActive && isInBossFight)
    {
        Boss::draw(window);
    }

    if(gDrawHitboxes){
        sf::RectangleShape goalMarker;
        goalMarker.setSize(sf::Vector2f(10.f, 10.f));
        goalMarker.setFillColor(sf::Color::Red);
        goalMarker.setPosition(goal);
        goalMarker.setOrigin(sf::Vector2f(5.f, 5.f));

        window.draw(goalMarker);
    }
}

void Death::resetPosition()
{
    Boss::resetPosition();

    speed = DEATH_SPEED;
    life = DEATH_LIFE;

    currentAnimation = sleepPhantomBat;
    dead = false;
}

void Death::hello() const
{
    std::cout << "Soy Death" << std::endl;
}
