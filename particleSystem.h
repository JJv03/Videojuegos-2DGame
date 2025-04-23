#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "particle.h"


class ParticleSystem {
public:
    ParticleSystem() = default;

    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    template<typename T, typename... Args>
    void spawn(Args&&... args) {
        particles.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

private:
    std::vector<std::unique_ptr<Particle>> particles;
};