#include "crow.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Initialize crow with stats and vision field
Crow::Crow(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = {0.0f, 0.0f};
    life = CROW_LIFE;
    score = CROW_SCORE;
    damage = CROW_DAMAGE;

    // Set initial detection area
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }

    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);
    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Leopard AnimationManager!" << std::endl;
    }

    animationManager->addAnimation(idleCrow, this->idleCrowFrames);
    animationManager->addAnimation(flyingCrow, this->flyCrowFrames);

    // animationManager->addAnimation(invulnerableSimon,this->invulnerableFrames,false);
    this->currentAnimation = idleCrow;
    animationManager->playAnimation(idleCrow);

    this->animationManager = animationManager;
    playerDetected = false;
    hasRedirected = false;
    isPlayerRight = false;
    currentState = State::MOVING;
    prevState = State::MOVING;
    playerDetected = false;
    startingMoving = false;
    attackPositioning = false;
    goal = sf::Vector2f(0, 0);
}

// Update vision field based on current position
void Crow::updateVisionField()
{
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

// Main update loop
void Crow::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                     const sf::Vector2f &playerPos, const std::vector<sf::FloatRect> &simonBounds, const sf::FloatRect &mapBounds)
{
    playerPosition = playerPos;

    // SPAWN LOGIC
    bool enemyInsideActivationZone = false;
    bool enemyInsideDeactivationZone = false;

    for (const auto &hitbox : hitboxes)
    {
        if (playerActivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideActivationZone = true;
        }
        if (playerDeactivationZone.findIntersection(hitbox).has_value())
        {
            enemyInsideDeactivationZone = true;
        }
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
        needsPlayerToLeaveZone = true;
    }

    isPlayerRight = sprite->getGlobalBounds().position.x <= playerPos.x;

    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }
        position = sprite->getGlobalBounds().position;

        updateVisionField();

        for (auto &hitbox : simonBounds)
        {
            if (visionField.findIntersection(hitbox).has_value())
            {
                playerDetected = true;
                currentAnimation = flyingCrow;
                break;
            }
        }

        if(playerDetected){
            switch(currentState){
                case State::MOVING:
                    // std::cout << "Moving" << std::endl;
                    timerMove += deltaTime;
                    if(!startingMoving){
                        startingMoving = true;
                        getRandomGoal(playerPos);
                        getLinelSpeed(interval);
                    }
                    if(timerMove >= interval){
                        timerMove = 0;
                        prevState = currentState;
                        currentState = State::WAITING;
                    }

                    break;
                
                case State::WAITING:
                    // std::cout << "Waiting" << std::endl;
                    speed = sf::Vector2f(0, 0);
                    timerWait += deltaTime;
                    if(timerWait >= waitInterval){
                        timerWait = 0;
                        if(prevState == State::MOVING){
                            prevState = currentState;
                            currentState = State::POSITIONING;
                        }
                        else if(prevState == State::POSITIONING){
                            prevState = currentState;
                            currentState = State::ATTACK;
                        }
                    }
                    break;
                
                case State::POSITIONING:
                    // std::cout << "Positioning" << std::endl;
                    timerPositioning += deltaTime;
                    if(!startingPositioning){
                        startingPositioning = true;
                        getPlayerGoal(playerPos);
                        getLinelSpeed(interval);
                    }
                    if(timerPositioning >= interval){
                        timerPositioning = 0;
                        prevState = currentState;
                        currentState = State::WAITING;
                    }

                    break;

                case State::ATTACK:
                    // std::cout << "Moving" << std::endl;
                    if(!attackPositioning){
                        attackPositioning = true;
                        if(isPlayerRight){
                            speed = sf::Vector2f(55, 0);
                        }
                        else{
                            speed = sf::Vector2f(-55, 0);
                        }
                        origDirRight = isPlayerRight;
                    }
                    else{
                        if(!hasRedirected){
                            if(std::abs(position.x - playerPos.x) >= 45 && isPlayerRight != origDirRight){
                                hasRedirected = true;
                                speed = sf::Vector2f(-speed.x, 0);
                            }
                        }
                    }
                    break;
            }
        }
    }

    if(isPlayerRight){
        sprite->setScale({-1.f, 1.f});
    } else {
        sprite->setScale({1.f, 1.f});
    }
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
        sprite->move({0.f, speed.y * deltaTime});
        for (auto &hitbox : hitboxes)
        {
            hitbox.position.y += speed.y * deltaTime;
        }
    }
    
    updateAnimation(deltaTime);
}

void Crow::getLinelSpeed(float timeToMove){
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;
    std::cout << "Goal y: " << goal.y << " Pos y: " << position.y << std::endl;

    speed = sf::Vector2f(deltaX / timeToMove, deltaY / timeToMove);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void Crow::getRandomGoal(const sf::Vector2f &playerPos){
    // Obtener una distancia aleatoria entre 20 y 70
    float distance = 20 + std::rand() % (70 - 20 + 1);;

    // Player is right, lets go to left
    float goalX = isPlayerRight ? (playerPos.x - distance) : (playerPos.x + distance);

    // Generar un desplazamiento vertical aleatorio entre -75 y -15
    float verticalOffset = -15 - std::rand() % (61); // 60 = -15 - (-75)

    // Aplicar el offset vertical desde la posición actual del cuervo
    float goalY = position.y > 100 ? position.y + verticalOffset : position.y - verticalOffset;

    // Asignar la posición de destino
    goal = sf::Vector2f(goalX, goalY);
}

void Crow::getPlayerGoal(const sf::Vector2f &playerPos){
    // Obtener una distancia aleatoria entre 50 y 120
    float distance = 50 + std::rand() % (120 - 50 + 1);

    // Player is right, lets go to left
    float goalX = isPlayerRight ? (playerPos.x - distance) : (playerPos.x + distance);

    // Calcular Y como la posición del jugador + un valor aleatorio entre 4 y 9
    float yOffset = 4 + std::rand() % (9 - 4 + 1);
    float goalY = playerPos.y + yOffset;

    std::cout << "POS: " << position.x << " " << position.y << std::endl;
    std::cout << "GOAL: " << goalX << " " << goalY << std::endl;

    // Asignar la posición de destino
    goal = sf::Vector2f(goalX, goalY);
}

void Crow::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive || !sprite)
        return;

    else if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            game.particleSystem.spawnFireParticle(sprite->getGlobalBounds().position);
            resetPosition();
            needsPlayerToLeaveZone = true;
        }
    }
    else if (MiscellaneousTile *miscTile = dynamic_cast<MiscellaneousTile *>(&other))
    {
        if (!miscTile->isDestroyed && miscTile->isCollidable())
        {
            onCollision_MiscTile(miscTile->getBounds()[0]);
        }
    }
}

// Reset to initial state
void Crow::resetPosition()
{
    Enemy::resetPosition();

    speed = {0.0f, 0.0f};
    life = CROW_LIFE;

    sprite->setScale({1.0f, 1.0f});

    currentAnimation = idleCrow;

    playerDetected = false;
    hasRedirected = false;
    isPlayerRight = false;
    currentState = State::MOVING;
    prevState = State::MOVING;
    playerDetected = false;
    startingMoving = false;
    startingPositioning = false;
    attackPositioning = false;
    origDirRight = false;
    timerMove = 0.f;
    timerWait = 0.f;
    timerPositioning = 0.f;
}

// Render with optional debug visuals
void Crow::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        Enemy::draw(window);

        if (gDrawHitboxes)
        {
            // Draw vision field
            sf::RectangleShape visionRect;
            visionRect.setPosition(visionField.position);
            visionRect.setSize(visionField.size);
            visionRect.setFillColor(sf::Color(0, 255, 0, 50));
            visionRect.setOutlineColor(sf::Color(0, 255, 0));
            visionRect.setOutlineThickness(1.0f);
            window.draw(visionRect);
        }
    }
}

// Update animation frame and direction
void Crow::updateAnimation(float deltaTime)
{
    if (!isActive || !sprite) return;

    if(!animationManager->isPlaying(currentAnimation)){
        animationManager->playAnimation(currentAnimation);
    }
    animationManager->update(deltaTime);
}

void Crow::hello() const
{
    std::cout << "Soy Crow" << std::endl;
}