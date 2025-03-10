#include "entity.h"

Entity::Entity(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes)
    : sprite(_sprite), hitboxes(_hitboxes) {}