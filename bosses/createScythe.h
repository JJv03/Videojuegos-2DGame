#pragma once

#include <iostream>
#include <memory>
#include "scythe.h"

std::shared_ptr<Scythe> createScythe(const sf::Vector2f &position, const sf::Vector2f &velocity, float damage = 2.0f);