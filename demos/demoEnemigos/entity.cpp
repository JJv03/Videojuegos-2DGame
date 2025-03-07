#include "entity.h"

Entity::Entity(const std::vector<sf::FloatRect> &_hitboxes, sf::Sprite &_sprite) : hitboxes(_hitboxes), sprite(&_sprite) {}
