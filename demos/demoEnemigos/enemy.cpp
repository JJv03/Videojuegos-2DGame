#include "enemy.h"

Enemy::Enemy(sf::Sprite &_sprite, std::vector<sf::FloatRect> &_hitboxes, sf::FloatRect _activationZone)
    : Entity(_sprite, _hitboxes), activationZone(_activationZone) {}
