#include "leopard.h"
#include <iostream>
#include <cmath>
#include "../game.h"

// Initialize leopard with stats and vision field
Leopard::Leopard(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const int &level, const int &stage)
    : Enemy(_sprite, _hitboxes), level(level), stage(stage)
{
    speed = {0.0f, 0.0f};
    life = LEOPARD_LIFE;
    score = LEOPARD_SCORE;
    damage = LEOPARD_DAMAGE;

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

    animationManager->addAnimation(idleLeopard, this->idleLeopardFrames);
    animationManager->addAnimation(jumpLeopard, this->jumpLeopardFrames);
    animationManager->addAnimation(walkLeopard, this->walkLeopardFrames);
    animationManager->addAnimation(noAnimation, this->emptyFrames);

    // animationManager->addAnimation(invulnerableSimon,this->invulnerableFrames,false);
    this->currentAnimation = flyBat;
    animationManager->playAnimation(flyBat);

    this->animationManager = animationManager;
}

// Update vision field based on current position
void Leopard::updateVisionField()
{
    if (!hitboxes.empty())
    {
        visionField = sf::FloatRect(
            {hitboxes[0].position.x - VISION_RANGE, hitboxes[0].position.y - VISION_RANGE},
            {hitboxes[0].size.x + (VISION_RANGE * 2), hitboxes[0].size.y + (VISION_RANGE * 2)});
    }
}

// Jump action
void Leopard::jump()
{
    if (isOnGround)
    {
        speed.y = LEOPARD_JUMP_SPEED;
        isOnGround = false;
    }
}

// Main update loop
void Leopard::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
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

    // If the player is outside the activation zone, the enemy is allowed to reactivate in the future.
    if (!enemyInsideActivationZone)
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

    // MOVEMENT AI LOGIC
    if (isActive)
    {
        if (checkMapBoundaries(mapBounds))
        {
            return;
        }

        updateVisionField();

        playerDetected = false;
        for (auto &hitbox : simonBounds)
        {
            if (visionField.findIntersection(hitbox).has_value())
            {
                playerDetected = true;
                break;
            }
        }

        if (playerDetected && speed.x == 0) // Chase player if detected
        {
            float directionToPlayer = (playerPosition.x > sprite->getPosition().x) ? 1.0f : -1.0f;
            speed.x = std::abs(LEOPARD_SPEED.x) * directionToPlayer;
        }
        else if (!playerDetected && !hasRedirected && speed.x != 0 && isOnGround) // Continue towards last known position
        {
            float directionToLastPlayerPos = (playerPosition.x > sprite->getPosition().x) ? 1.0f : -1.0f;
            speed.x = std::abs(LEOPARD_SPEED.x) * directionToLastPlayerPos;
            hasRedirected = true;
        }

        applyGravity(deltaTime);

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

        updateAnimation(deltaTime);

        
        // Right before checkCollisions
        isOnGround = false;
    }
}

void Leopard::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (dynamic_cast<SolidTile *>(&other))
    {
        onCollision_SolidTile(other);

        if (isOnGround && Enemy::checkForLedge(other))
        {
            jump();
        }
    }
    else if (MiscellaneousTile *miscTile = dynamic_cast<MiscellaneousTile *>(&other))
    {
        if (!miscTile->isDestroyed && miscTile->isCollidable())
        {
            onCollision_MiscTile(miscTile->getBounds()[0]);
        }
    }
    else if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::DEFAULT_ENEMIES, sprite->getGlobalBounds().position);
            resetPosition();
        }
    }
}

// Reset to initial state
void Leopard::resetPosition()
{
    Enemy::resetPosition();

    speed = {0.0f, 0.0f};
    life = LEOPARD_LIFE;

    sprite->setScale({1.0f, 1.0f});

    playerDetected = false;
    hasRedirected = false;
}

// Render with optional debug visuals
void Leopard::draw(sf::RenderWindow &window)
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
void Leopard::updateAnimation(float deltaTime)
{
    if(isActive){
        if (!isOnGround) {
            currentAnimation = jumpLeopard;
        } else if (speed.x != 0){
            currentAnimation = walkLeopard;
        } else {
            currentAnimation = idleLeopard;
        }
    } else {
        currentAnimation = noAnimation;
    }

    if(!animationManager->isPlaying(currentAnimation))
    {
        animationManager->playAnimation(currentAnimation);
    }

    animationManager->update(deltaTime);

    
    // Flip sprite based on movement direction
    sf::Vector2f currentSpeed = speed;

    if (currentSpeed.x <= 0)
    {
        sprite->setScale({1.0f, 1.0f});
    }
    else if (currentSpeed.x > 0)
    {
        sprite->setScale({-1.0f, 1.0f});
    }
}

void Leopard::hello() const
{
    std::cout << "Soy Leopard" << std::endl;
}