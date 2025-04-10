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

    virtual std::vector<sf::FloatRect> getBounds() const;
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