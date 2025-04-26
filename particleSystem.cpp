#include "particleSystem.h"
#include <iostream>
#include "globals.h"


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

void ParticleSystem::clear() {
    particles.clear();
}

void ParticleSystem::spawnBreakBlockParticle(sf::Vector2f position) {
    spawn<BreakBlockParticle>(gTextures.at("miscTile_1"), position);
}

void ParticleSystem::spawnHitParticle(sf::Vector2f position) {
    spawn<HitParticle>(gTextures.at("simon"), position);
}

void ParticleSystem::spawnFireParticle(sf::Vector2f position, bool addOffset) {
    if (addOffset) {
        position += sf::Vector2f{5.f, 0.f};
    }
    spawn<FireParticle>(gTextures.at("enemy"), position);
}

void ParticleSystem::spawnBigFireParticle(sf::Vector2f position, bool addOffset) {
    if (addOffset) {
        position += sf::Vector2f{5.f, 0.f};
    }
    spawn<BigFireParticle>(gTextures.at("boss"), position);
}

void ParticleSystem::spawnPointsParticle(sf::Vector2f position, int points) {
    spawn<PointsParticle>(gTextures.at("miscTile_1"), position, points);
}