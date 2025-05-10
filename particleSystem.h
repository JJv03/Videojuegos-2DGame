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
    void clear();

    template<typename T, typename... Args>
    void spawn(Args&&... args) {
        particles.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void spawnBreakBlockParticle(sf::Vector2f position);
    void spawnHitParticle(sf::Vector2f position);
    void spawnFireParticle(sf::Vector2f position, bool addOffset = true);
    void spawnBigFireParticle(sf::Vector2f position, bool addOffset = true);
    void spawnReallyBigFireParticle(sf::Vector2f position, bool addOffset = true);
    void spawnPointsParticle(sf::Vector2f position, int points);
    void spawnWaterSplashParticle(sf::Vector2f position);

private:
    std::vector<std::unique_ptr<Particle>> particles;
};