#include "scythe.h"
#include "../globals.h"
#include <iostream>

Scythe::Scythe(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
                       const sf::Vector2f &position, const sf::FloatRect &mapDims, float _damage)
    : EntitySprite(_sprite, _hitboxes), mapDims(mapDims), damage(_damage){
        velocity = sf::Vector2f(0, 0);
        
        AnimationManager *animationManager = new AnimationManager(*this->sprite, this);

        if (!animationManager)
        {
            std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
        }

        animationManager->addAnimation(scythe, this->normalFrames);
        animationManager->addAnimation(gamePad, this->gamePadFrames);

        this->animationManager = animationManager;

        if(gCustomSkins){
            currentAnimation = gamePad;
        }
        else{
            currentAnimation = scythe;
        }
        animationManager->playAnimation(currentAnimation);
    }

void Scythe::update(float deltaTime, const sf::FloatRect &deactivationZone)
{
    if (!isActive || !sprite)
    {
        return;
    }

    timer += deltaTime;
    if(timer >= timeToMove){
        velocity = sf::Vector2f(0, 0);
    }

    animationManager->update(deltaTime);

    position = sprite->getGlobalBounds().position;

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

void Scythe::onCollision(Entity &other, Game &game)
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

void Scythe::planPosSpeed(const sf::Vector2f goal){
    timer = 0;
    float deltaX = goal.x - position.x;
    float deltaY = goal.y - position.y;

    velocity = sf::Vector2f(deltaX / timeToMove, -deltaY / timeToMove);
    // std::cout << "Speed: " << speed.x << " " << speed.y << std::endl;
}

void Scythe::reset()
{
    isActive = false;
}

void Scythe::draw(sf::RenderWindow &window)
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