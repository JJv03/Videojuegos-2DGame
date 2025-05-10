#pragma once

#include <iostream>
#include <memory>
#include "bone.h"

std::shared_ptr<Bone> createBone(const sf::Vector2f &position, const sf::FloatRect &mapDims, bool facingRight, float verticalSpeed, float damage);