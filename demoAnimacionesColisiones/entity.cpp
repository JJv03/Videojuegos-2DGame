#include "entity.h"


Entity::Entity(const sf::FloatRect& _hitbox, sf::Sprite& _sprite) :
                hitbox(_hitbox), sprite(&_sprite) {}
