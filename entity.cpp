#include "entity.h"

Entity::Entity(std::vector<sf::FloatRect> &_hitboxes)
    : hitboxes(_hitboxes) {}

    
EntitySprite::EntitySprite(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : Entity(_hitboxes), sprite(_sprite) {}


std::vector<sf::FloatRect> Entity::getBounds() const{
    return hitboxes;
}
