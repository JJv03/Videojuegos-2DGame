#include "entity.h"

Entity::Entity(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes)
    : sprite(&_sprite), hitboxes(_hitboxes) {}