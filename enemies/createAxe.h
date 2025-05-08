#pragma once

#include <iostream>
#include <memory>
#include "axe.h"

std::shared_ptr<Axe> createAxe(const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float damage = 2.0f);