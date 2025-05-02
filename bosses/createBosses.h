#pragma once

#include <iostream>
#include "phantomBat.h"
#include "dracula.h"
#include "draculaSpirit.h"
#include "death.h"

// Factory functions for creating boss instances
PhantomBat *createPhantomBat(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims);
Dracula *createDracula(const sf::Vector2f &position, const size_t &level, const size_t &stage);
DraculaSpirit *createDraculaSpirit(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims);
Death *createDeath(const sf::Vector2f &position, const size_t &level, const size_t &stage, const sf::FloatRect mapDims);
