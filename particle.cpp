#include <cmath>
#include <iostream>
#include <cstdlib>
#include <numbers>
#include "particle.h"


// BreakBlock constants
const float BB_PARTICLE_GRAVITY = 400.f; // Gravity acceleration in pixels per second squared
const float BB_PARTICLE_LIFETIME = 4.f;  // Particle lifetime in seconds

// Hit constants
const float HIT_PARTICLE_LIFETIME = 0.15f; // Particle lifetime in seconds

// Fire constants
const float FIRE_PARTICLE_LIFETIME = 0.15f; // Particle lifetime in seconds


// =================================================================================================
// ===================================== BREAK BLOCK PARTICLE ======================================
// =================================================================================================


BreakBlockParticle::BreakBlockParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_gravity(BB_PARTICLE_GRAVITY), m_lifetime(BB_PARTICLE_LIFETIME), m_alive(true)
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


// =================================================================================================
// ===================================== HIT PARTICLE ==============================================
// =================================================================================================


HitParticle::HitParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_lifetime(HIT_PARTICLE_LIFETIME), m_alive(true) {
    m_sprite = std::make_unique<sf::Sprite>(texture);
    m_sprite->setTextureRect(sf::IntRect({727, 477}, {8, 10}));
    m_sprite->setPosition(position);
}

void HitParticle::update(float dt) {
    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        m_alive = false;
    }
}

void HitParticle::draw(sf::RenderTarget& target) const {
    target.draw(*m_sprite);
}

bool HitParticle::isAlive() const {
    return m_alive;
}


// =================================================================================================
// ===================================== FIRE PARTICLE =============================================
// =================================================================================================

FireParticle::FireParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_lifetime(FIRE_PARTICLE_LIFETIME), m_alive(true) {
    m_sprite = std::make_unique<sf::Sprite>(texture);
    m_sprite->setTextureRect(sf::IntRect({362, 122}, {8, 16}));
    m_sprite->setPosition(position);

    m_animationManager = std::make_unique<AnimationManager>(*m_sprite);
    m_animation = {
        notRelevant,
        {
            {sf::IntRect({362, 122}, {8, 16}), FIRE_PARTICLE_LIFETIME/3.f},
            {sf::IntRect({371, 122}, {8, 16}), FIRE_PARTICLE_LIFETIME/3.f},
            {sf::IntRect({380, 122}, {8, 16}), FIRE_PARTICLE_LIFETIME/3.f},
        },
        true
    };

    m_animationManager->playAnimation(m_animation);
};

void FireParticle::update(float deltaTime) {
    m_lifetime -= deltaTime;

    if (m_lifetime <= 0.f) {
        m_alive = false;
    }

    m_animationManager->update(deltaTime);
}

void FireParticle::draw(sf::RenderTarget& target) const {
    target.draw(*m_sprite);
}

bool FireParticle::isAlive() const {
    return m_alive;
}