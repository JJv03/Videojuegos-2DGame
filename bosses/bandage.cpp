#include "bandage.h"
#include "../globals.h"
#include <iostream>

Bandage::Bandage(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
                       const sf::Vector2f &position, const sf::FloatRect &mapDims, float _damage)
    : EntitySprite(_sprite, _hitboxes), mapDims(mapDims), damage(_damage){
        //velocity = sf::Vector2f(0, 0);
        curveTime = 0.0f;
        AnimationManager *animationManager = new AnimationManager(*this->sprite, this);

        if (!animationManager)
        {
            std::cerr << "Error: Failed to initialize Fishman AnimationManager!" << std::endl;
        }

        animationManager->addAnimation(bandageAttack, this->normalFrames);

        this->animationManager = animationManager;

        currentAnimation = bandageAttack;
        animationManager->playAnimation(currentAnimation);
    }

void Bandage::update(float deltaTime, const sf::FloatRect &deactivationZone)
{
    if (!isActive || !sprite)
    {
        return;
    }

    animationManager->update(deltaTime);

    // Move the projectile
    this->curveTime += deltaTime;
    if (this->curveTime>0.55f)
    {
        velocity.y = velocity.y * -1.f;
        curveTime = 0.0f;
    }
    
    sf::Vector2f movement = velocity * deltaTime;
     sprite->move(movement);
    // Update hitboxes
    for (auto &hitbox : hitboxes)
    {
        hitbox.position.x += movement.x;
        hitbox.position.y += movement.y;
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

void Bandage::onCollision(Entity &other, Game &game)
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


void Bandage::reset()
{
    isActive = false;
}

void Bandage::draw(sf::RenderWindow &window)
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