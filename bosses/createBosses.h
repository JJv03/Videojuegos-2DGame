#pragma once

#include <iostream>
#include "phantomBat.h"

// Factory functions for creating boss instances
PhantomBat *createPhantomBatSpawner(const sf::Vector2f &position, const size_t &level, const size_t &stage);
