#pragma once

#include <iostream>
#include "zombie.h"
#include "zombieSpawner.h"
#include "leopard.h"
#include "bat.h"

Zombie createZombie(const sf::Vector2f &position);
Leopard createLeopard(const sf::Vector2f &position);
Bat createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize);