#include "bone.h"
#include "../game.h"

Bone::Bone(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes,
               const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float verticalSpeed, float _damage)
    : EntitySprite(_sprite, _hitboxes), mapDims(mapDims), facingRight(facingRight), damage(_damage){
        
        velocity = sf::Vector2f(0, 0);

        velocity.y = verticalSpeed;

        if(facingRight) velocity.x = 45.f;
        else velocity.x = -45.f;
    
    AnimationManager *animationManager = new AnimationManager(*this->sprite, this);

    if (!animationManager)
    {
        std::cerr << "Error: Failed to initialize Bone AnimationManager!" << std::endl;
    }

    if(facingRight) animationManager->addAnimation(boneFlipping, this->rightFlipFrames);
    else animationManager->addAnimation(boneFlipping, this->leftFlipFrames);

    this->animationManager = animationManager;

    currentAnimation = boneFlipping;
    animationManager->playAnimation(currentAnimation);
}

void Bone::update(float deltaTime){
    if (!isActive || !sprite)
    {
        return;
    }

    position = sprite->getGlobalBounds().position;

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

    bool isInsideMap = false;
    for (const auto &hitbox : hitboxes)
    {
        if (mapDims.findIntersection(hitbox).has_value())
        {
            isInsideMap = true;
            break;
        }
    }

    if (!isInsideMap)
    {
        isActive = false;
    }

    velocity.y -= gPlayerGravity * deltaTime;
}

void Bone::onCollision(Entity &other, Game &game, const sf::FloatRect& intersectionRect){
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
        game.particleSystem.spawnHitParticle(position);
        isActive = false;
    }
    else if (dynamic_cast<SubWeapon *>(&other))
    {
        game.particleSystem.spawnHitParticle(position);
        isActive = false;
    }
}

void Bone::reset(){
    isActive = false;
}

void Bone::draw(sf::RenderWindow &window){
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