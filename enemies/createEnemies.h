#pragma once

#include <iostream>
#include "zombie.h"
#include "zombieSpawner.h"
#include "leopard.h"
#include "bat.h"
#include "fishMan.h"
#include "ghost.h"
#include "medusa.h"
#include "cannon.h"
#include "axeMan.h"

// Factory functions for creating enemy instances
Zombie *createZombie(const sf::Vector2f &position);
Leopard *createLeopard(const sf::Vector2f &position, const size_t &level, const size_t &stage);
Bat *createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage);
FishMan *createFishManSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage, std::mt19937 &rngReference);
Ghost *createGhost(const sf::Vector2f &position, const size_t &level, const size_t &stage);
Medusa *createMedusaSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize, const size_t &level, const size_t &stage);
Cannon *createCannon(const sf::Vector2f &position, const size_t &level, const size_t &stage);
AxeMan *createAxeMan(const sf::Vector2f &position, const size_t &level, const size_t &stage);