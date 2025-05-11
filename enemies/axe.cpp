#include "axe.h"
#include "../game.h"
#include "../globals.h"
#include <iostream>

Axe::Axe(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
                       const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float _damage)
    : EntitySprite(_sprite, _hitboxes), mapDims(mapDims), facingRight(facingRight), damage(_damage){
        velocity = sf::Vector2f(0, 0);
        
        AnimationManager *animationManager = new AnimationManager(*this->sprite, this);

        if (!animationManager)
        {
            std::cerr << "Error: Failed to initialize Axe AnimationManager!" << std::endl;
        }

        animationManager->addAnimation(axeThrowing, this->normalFrames);

        this->animationManager = animationManager;
        changedSpeed = false;

        currentAnimation = axeThrowing;
        animationManager->playAnimation(currentAnimation);
    }

void Axe::update(float deltaTime, const sf::FloatRect &deactivationZone)
{
    if (!isActive || !sprite)
    {
        return;
    }

    position = sprite->getGlobalBounds().position;

    if(timer == 0){
        if(facingRight){
            planPosSpeed(sf::Vector2f(position.x + maxDist, position.y));
        }
        else{
            planPosSpeed(sf::Vector2f(position.x - maxDist, position.y));
        }
    }

    timer += deltaTime;
    if(timer >= timeToMove && !changedSpeed){
        velocity = sf::Vector2f(-velocity.x, 0);
        changedSpeed = true;
    }

    animationManager->update(deltaTime);

    if (velocity.x != 0)
    {
        sprite->move({velocity.x * deltaTime, 0.f});
        for (auto &hitbox : hitboxes)
        {
            hitbox.position.x += velocity.x * deltaTime;
        }
    }
    if (velocity.y != 0)
    {
        sprite->move({0.f, -velocity.y * deltaTime});
        for (auto &hitbox : hitboxes)
        {
            hitbox.position.y -= velocity.y * deltaTime;
        }
    }

    // Check if projectile is outside deactivation zone
    bool isInsideZone = false;
    for (const auto &hitbox : hitboxes)
    {
        if (deactivationZone.findIntersection(hitbox).has_value())
        {
            isInsideZone = true;
            break;
        }
    }

    if (!isInsideZone)
    {
        isActive = false;
    }
}

void Axe::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect)
{
    if (!isActive)
    {
        return;
    }

    // Handle collision with player
    if (dynamic_cast<Player *>(&other))
    {
        isActive = false;
    }
    else if (dynamic_cast<AxeMan *>(&other))
    {
        isActive = false;
    }
    // Handle collision with weapons
    else if (dynamic_cast<Whip *>(&other))
    {
        isActive = false;
    }
    else if (dynamic_cast<SubWeapon *>(&other))
    {
        isActive = false;
    }
}

void Axe::planPosSpeed(const sf::Vector2f goal){
    timer = 0;
    float deltaX = goal.x - position.x;

    velocity = sf::Vector2f(deltaX / timeToMove, 0);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void Axe::reset()
{
    isActive = false;
}

void Axe::draw(sf::RenderWindow &window)
{
    if (sprite && isActive)
    {
        window.draw(*sprite);

        if (gDrawHitboxes) // Only draw hitboxes in debug mode
        {
            for (const auto &hitbox : hitboxes)
            {
                sf::RectangleShape hitboxShape({hitbox.size.x, hitbox.size.y});
                hitboxShape.setPosition({hitbox.position.x, hitbox.position.y});
                hitboxShape.setFillColor(sf::Color::Transparent);
                hitboxShape.setOutlineColor(sf::Color::Red);
                hitboxShape.setOutlineThickness(1.0f);
                window.draw(hitboxShape);
            }
        }
    }
}