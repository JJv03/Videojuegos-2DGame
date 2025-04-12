#pragma once

#include <iostream>
#include "zombie.h"
#include "zombieSpawner.h"
#include "leopard.h"
#include "bat.h"

// Factory functions for creating enemy instances
Zombie* createZombie(const sf::Vector2f &position);
Leopard* createLeopard(const sf::Vector2f &position, const size_t &level, const size_t &stage);
Bat* createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage);