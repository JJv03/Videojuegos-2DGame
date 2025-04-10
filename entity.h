#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Entity
{
public:
    std::vector<sf::FloatRect> hitboxes;

    Entity() = default;
    virtual ~Entity() = default;
    Entity(std::vector<sf::FloatRect> &_hitboxes);

    virtual sf::FloatRect getBounds() const = 0;
    virtual void onCollision(Entity& other) = 0;
};


class EntitySprite : public Entity
{
public:
    std::shared_ptr<sf::Sprite> sprite;

    EntitySprite() = default;
    EntitySprite(std::shared_ptr<sf::Sprite> sprite, std::vector<sf::FloatRect> &_hitboxes);

    virtual ~EntitySprite() = default;
};