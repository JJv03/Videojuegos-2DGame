#pragma once

#include <iostream>
#include <memory>
#include "scythe.h"

std::shared_ptr<Scythe> createScythe(const sf::Vector2f &position, const sf::FloatRect &mapDims, float damage = 2.0f);