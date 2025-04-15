#include "entity.h"
#include <iostream>

Entity::Entity(std::vector<sf::FloatRect> &_hitboxes)
    : hitboxes(_hitboxes) {}

    
EntitySprite::EntitySprite(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Entity(_hitboxes), sprite(_sprite) {}

void Entity::hello() const {
    std::cout << "Soy Entity" << std::endl;
}

std::vector<sf::FloatRect> Entity::getBounds() const{
    return hitboxes;
}

std::vector<sf::FloatRect> EntitySprite::getBounds() const{
    return hitboxes;
    //return std::vector<sf::FloatRect>({sprite.get()->getGlobalBounds()});
}