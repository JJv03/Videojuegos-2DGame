#pragma once

#include <iostream>
#include <memory>
#include "projectile.h"

enum class ProjectileType
{
    FISHMAN,
    ROCK,
};

std::shared_ptr<Projectile> createProjectile(const sf::Vector2f &position, const sf::Vector2f &velocity, ProjectileType type = ProjectileType::FISHMAN, float damage = 1.0f);