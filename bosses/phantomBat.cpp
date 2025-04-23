#include "phantomBat.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
PhantomBat::PhantomBat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = PBAT_SPEED;
    life = PBAT_LIFE;
    score = PBAT_SCORE;
    damage = PBAT_DAMAGE;

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(sleepPhantomBat, this->idlePhBatFrames);
    animationManager->addAnimation(flyPhantomBat, this->flyPhBatFrames);

    this->animationManager = animationManager;
    currentAnimation = sleepPhantomBat;
}

// Update phantomBat logic: handle spawning, movement, and deactivation
void PhantomBat::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    if (!isActive)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value() && isInBossFight)
            {
                isActive = true;
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {
        // MAQUINA DE ESTADOS DEL BOSS: PRIMERO ESTA UN TIEMPO QUIETO Y DESPUES EMPIZA A MOVERSE/ATACAR
        // PARA LA IA MEJORADA DIRÍA DE HACER DOS MAQUINAS DE ESTADOS Y QUE SE ELIJAN CON UNA VARAIBLE GLOBAL
        // ESTA EN REPOSO HASTA QUE SE BLOQUEA LA OPCION DE IR HACIA ATRÁS QUE ES CUANDO SE EMPIEZA A MOVER
        position = sprite->getGlobalBounds().position;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        // Wait 2 seconds and then go to center
        timer += deltaTime;
        if(starting){            
            if (timer >= sleepInterval) {
                timer = 0.0f;
                starting = false;
                currentAnimation = flyPhantomBat;
                goal = sf::Vector2f(map.x + size.x / 2, map.y + size.y / 2);
                // std::cout << "Map pos: " << map.x << " " << map.y << std::endl;
                // std::cout << "Map size: " << size.x << " " << size.y << std::endl;
                std::cout << "Goal tras dormir: " << goal.x << " " << goal.y << std::endl;
                getLinelSpeed();
            }
        }
        else{
            auto mode = configManager.getDifficulty();
            if (!mode.hard_mode){   // Normal mode
                if(goingToCenter){
                    if(timer >= moveInterval){
                        timer = 0.0f;
                        speed = sf::Vector2f(0, 0);
                        goingToCenter = false;
                        std::cout << "Pos en el centro: " << position.x << " " << position.y << std::endl;
                    }
                }
                else{
                    if (waiting) {
                        speed = sf::Vector2f(0, 0);
                        timer += deltaTime;
                        if (timer >= waitingInterval) {
                            // Decide moverse hacia el lado opuesto
                            
                            goal = (position.x > map.x + size.x / 2)
                                ? sf::Vector2f(position.x + 25.f, position.y)
                                : sf::Vector2f(position.x - 25.f, position.y);
                            std::cout << "GOAL: " << goal.x << " " << goal.y << std::endl;
                            getLinelSpeed();
                            waiting = false;
                            timer = 0.f;
                        }
                    }
                    else{
                        if(!attacking){ // moving left or right
                            if(timer >= moveInterval){
                                attacking = true;
                                timer = 0.f;
                                speed = sf::Vector2f(0, 0);
                                randomObjective();
                            }
                        }
                        else{   // attacking
                            if(timer >= moveInterval){
                                attacking = false;
                                waiting = true;
                                timer = 0.f;
                                speed = sf::Vector2f(0, 0);
                            }
                            else{
                                getDoubleSpeed();
                                doubleMoveTimer += deltaTime;
                            }
                        }
                    }
                }
            }

            else{                   // Enhanced AI mode

            }
        }
        updateAnimation(deltaTime);

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

void PhantomBat::randomObjective(){
    const float limitWidth = 75.0f;
    const float limitHeight = 125.0f;

    doubleMoveTimer = 0.f;
    startPosition = position;

    float minX = mapDims.position.x;
    float maxX = mapDims.position.x + mapDims.size.x - limitWidth;
    float minY = mapDims.position.y;
    float maxY = mapDims.position.y + mapDims.size.y - limitHeight;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    
    goal = sf::Vector2f(distX(gen), distY(gen)); 
    std::cout << "New goal: " << goal.x << " " << goal.y << std::endl;
}

void PhantomBat::getLinelSpeed(){
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;

    speed = sf::Vector2f(deltaX / moveInterval, -deltaY / moveInterval);
    std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void PhantomBat::getDoubleSpeed() {
    // U-shaped motion using a parabola: y = a*(x - h)^2 + k
    // In this case we simulate an interpolation between two points with easing
    float t = doubleMoveTimer / moveInterval;
    if (t > 1.f) t = 1.f;

    // componente horizontal: velocidad constante
    float totalDx   = goal.x - startPosition.x;
    float xVel      = totalDx / moveInterval;

    // componente vertical “lineal”
    float totalDy   = goal.y - startPosition.y;
    float yVelLin   = totalDy / moveInterval;

    // arco parabólico: 
    // f(t) = -4*H*(t-0.5)^2 + H    => parte vertical extra
    // f'(t) = d/dt = -8*H*(t - 0.5)
    float arcDeriv = -8.f * arcHeight * (t - 0.5f);
    float yVelArc  = arcDeriv / moveInterval;

    float yVel = yVelLin + yVelArc;

    speed = sf::Vector2f(xVel, -yVel);
    std::cout << "Double speed: " << speed.x << " " << speed.y << std::endl;
}

void PhantomBat::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sprite->getGlobalBounds().position);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sprite->getGlobalBounds().position);
        }
    }
}

// Render phantomBat and debug info (spawn zone)
void PhantomBat::draw(sf::RenderWindow &window)
{
    if (sprite)
    {
        Boss::draw(window);
    }

    sf::RectangleShape goalMarker;
    goalMarker.setSize(sf::Vector2f(10.f, 10.f));
    goalMarker.setFillColor(sf::Color::Red);
    goalMarker.setPosition(goal);
    goalMarker.setOrigin(sf::Vector2f(5.f, 5.f));

    window.draw(goalMarker);
}

// Update animation frame and flip sprite based on direction
void PhantomBat::updateAnimation(float deltaTime)
{
    // if (!isActive || !sprite)
    //     return;

    // // Flip sprite based on movement direction
    sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x < 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }

    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);
}

void PhantomBat::hello() const
{
    std::cout << "Soy PhantomBat" << std::endl;
}
