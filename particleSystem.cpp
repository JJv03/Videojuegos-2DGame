#include "particleSystem.h"
#include <iostream>

void ParticleSystem::update(float dt) {
    for (auto it = particles.begin(); it != particles.end(); ) {
        (*it)->update(dt);
        if (!(*it)->isAlive()) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderTarget& target) const {
    for (const auto& particle : particles) {
        particle->draw(target);
    }
}
