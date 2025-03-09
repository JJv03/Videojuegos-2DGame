#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Entity
{
public:
    sf::Sprite *sprite;
    std::vector<sf::FloatRect> hitboxes;
    sf::FloatRect activationZone; // ESTO SE MOVERA A LA CLASE ENEMIGO EN EL FUTURO
    bool playerWasNear = false;   // ESTO SE MOVERA A LA CLASE ENEMIGO EN EL FUTURO

    Entity() = default;
    Entity(const std::vector<sf::FloatRect> &_hitboxes, sf::Sprite &_sprite, sf::FloatRect _activationZone);
};