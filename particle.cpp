#include <cmath>
#include <iostream>
#include <cstdlib>
#include <numbers>
#include "particle.h"


BreakBlockParticle::BreakBlockParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_gravity(100.f), m_lifetime(4.f), m_alive(true)
{
    for (int i = 0; i < 3; ++i) {
        // Random direction in angle (-45° to 45°) and speed
        float angle = (static_cast<float>(rand()) / RAND_MAX) *
                        (std::numbers::pi / 2.f) - (std::numbers::pi / 4.f);    // [-π/4, π/4]

        float speed = 80.f + static_cast<float>(rand() % 60);   // between 80 and 140

        sf::Vector2f velocity = {
            std::cos(angle) * speed / 4.f,
            -std::abs(std::sin(angle) * speed)
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