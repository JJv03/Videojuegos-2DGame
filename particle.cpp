#include <cmath>
#include <iostream>
#include <cstdlib>
#include <numbers>
#include "particle.h"


const float PARTICLE_GRAVITY = 300.f; // Gravity acceleration in pixels per second squared
const float PARTICLE_LIFETIME = 4.f; // Particle lifetime in seconds


BreakBlockParticle::BreakBlockParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_gravity(PARTICLE_GRAVITY), m_lifetime(PARTICLE_LIFETIME), m_alive(true)
{
    const float angle_in_deg[] = {105.f, 75.f, 55.f };  // angle in which the debris will be thrown
    const float sign[] = {-1.f, 1.f, 1.f };             // sign of the horizontal velocity of the debris
    const float speed = 140.f;
    const float max_angle_variation = 5.f; // 5 degrees of random variation

    for (int i = 0; i < 3; ++i) {
        float angle_variation = (static_cast<float>(rand()) / RAND_MAX) * max_angle_variation;
        float angle_in_radians = (angle_in_deg[i] + angle_variation) * std::numbers::pi / 360.f;

        sf::Vector2f velocity = {
            sign[i] * std::cos(angle_in_radians) * speed / 4.f,
            -std::abs(std::sin(angle_in_radians) * speed)
        };

        Debris d {
            sf::Sprite(texture),
            velocity
        };
        d.sprite.setTextureRect(sf::IntRect({56, 69}, {7, 8}));
        d.sprite.setPosition(position);

        m_debris.push_back(d);
    }
}

void BreakBlockParticle::update(float dt) {
    for (auto& d : m_debris) {
        d.velocity.y += m_gravity * dt;
        d.sprite.move(d.velocity * dt);
    }

    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        m_alive = false;
    }
}

void BreakBlockParticle::draw(sf::RenderTarget& target) const {
    for (const auto& d : m_debris) {
        target.draw(d.sprite);
    }
}

bool BreakBlockParticle::isAlive() const {
    return m_alive;
}