#pragma once

#include <iostream>
#include <memory>
#include "bandage.h"

std::shared_ptr<Bandage> createBandage(const sf::Vector2f &position, const sf::FloatRect &mapDims, float damage = 2.0f);