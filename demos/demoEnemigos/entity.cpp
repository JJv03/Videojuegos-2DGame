#include "entity.h"

Entity::Entity(const std::vector<sf::FloatRect> &_hitboxes, sf::Sprite &_sprite, sf::FloatRect _activationZone)
    : hitboxes(_hitboxes), sprite(&_sprite), activationZone(_activationZone) {}