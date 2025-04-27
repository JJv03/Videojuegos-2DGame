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

// Big fire constants
const float BIG_FIRE_PARTICLE_LIFETIME = 0.9f; // Particle lifetime in seconds

// Points constants
const float POINTS_PARTICLE_LIFETIME = 0.5f; // Particle lifetime in seconds


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

void BreakBlockParticle::update(float deltaTime) {
    for (auto& d : m_debris) {
        d.velocity.y += m_gravity * deltaTime;
        d.sprite.move(d.velocity * deltaTime);
    }

    m_lifetime -= deltaTime;
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

void HitParticle::update(float deltaTime) {
    m_lifetime -= deltaTime;
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


// =================================================================================================
// ================================== BIG FIRE PARTICLE ============================================
// =================================================================================================

BigFireParticle::BigFireParticle(const sf::Texture& texture, sf::Vector2f position)
    : m_lifetime(BIG_FIRE_PARTICLE_LIFETIME), m_alive(true) {
    m_sprite = std::make_unique<sf::Sprite>(texture);
    m_sprite->setTextureRect(sf::IntRect({362, 122}, {8, 16}));
    m_sprite->setPosition(position);

    m_animationManager = std::make_unique<AnimationManager>(*m_sprite);
    m_animation = {
        notRelevant,
        {
            {sf::IntRect({32, 243}, {24, 32}), BIG_FIRE_PARTICLE_LIFETIME / (3.f*2.f)},
            {sf::IntRect({57, 243}, {24, 32}), BIG_FIRE_PARTICLE_LIFETIME / (3.f*2.f)},
            {sf::IntRect({82, 243}, {24, 32}), BIG_FIRE_PARTICLE_LIFETIME / (3.f*2.f)}
        },
        true
    };

    m_animationManager->playAnimation(m_animation);
};

void BigFireParticle::update(float deltaTime) {
    m_lifetime -= deltaTime;

    if (m_lifetime <= 0.f) {
        m_alive = false;
    }

    m_animationManager->update(deltaTime);
}

void BigFireParticle::draw(sf::RenderTarget& target) const {
    target.draw(*m_sprite);
}

bool BigFireParticle::isAlive() const {
    return m_alive;
}


// =================================================================================================
// ===================================== POINTS PARTICLE ===========================================
// =================================================================================================

// 14900, 127 time ==> 16170  ==> 10pts por segundo ==> 4s~
// 16170, 72 cora ==> 23370 ==> 100pts por cora && 10 cora / segundo

std::unordered_map<std::string, sf::IntRect> pointsTextRect = {
    {"10", sf::IntRect({1, 69}, {8, 8})},
    {"20", sf::IntRect({10, 69}, {8, 8})},
    {"40", sf::IntRect({19, 69}, {8, 8})},
    {"70", sf::IntRect({28, 69}, {8, 8})},
    {"0", sf::IntRect({37, 69}, {8, 8})},
    {"00", sf::IntRect({46, 69}, {8, 8})}
};

std::string PointsParticle::getFirstTwoDigits(int digit) {
    // Handle the first two digits as special cases: 10, 20, 40, 70
    if (digit == 1) return "10";
    else if (digit == 2) return "20";
    else if (digit == 4) return "40";
    else if (digit == 7) return "70";
    return "";
}

bool PointsParticle::isDoubleZeroDigits(int digit, int index, const std::vector<int>& digits) {
    // If the digit is '0'  AND  not the last one  AND  the following digit is a '0'
    if (digit == 0 && index >= 1 && digits[index - 1] == 0) return true;
    return false;
}

PointsParticle::PointsParticle(const sf::Texture& texture, sf::Vector2f position, int points)
    : m_lifetime(POINTS_PARTICLE_LIFETIME), m_alive(true) {

    // First, we need to convert the points to a vector of digits
    std::vector<int> digits;
    if (points == 0) {
        digits.push_back(0);
    } else {
        while (points > 0) {
            digits.push_back(points % 10);
            points /= 10;
        }
    }

    // Second, we fill the vector of sprites for each digit
    int xOffset = 0;                // x position for the current sprite
    bool firstTwoDigits = true;     // To track the first two digits
    for (int i = digits.size() - 1; i >= 0; --i) {      // Backwards to get the correct order
        int digit = digits[i];
        sf::IntRect texRect;
        
        if (firstTwoDigits) {   // First two digits are treated as a special case
            std::string firstTwoDigitsStr = getFirstTwoDigits(digit);
            if (firstTwoDigitsStr.empty())
                throw std::invalid_argument("Texture for digits \"" + std::to_string(digit) + "\" not found.");
            
            i -= 1;     // Skip the next digit because we are treating two digits as one
            firstTwoDigits = false;
            texRect = pointsTextRect[firstTwoDigitsStr];         
        }
        else {    // Rest of the digits are zeros
            if (isDoubleZeroDigits(digit, i, digits)) {
                texRect = pointsTextRect["00"];
                i -= 1;     // Skip the next digit
            }
            else {
                texRect = pointsTextRect[std::to_string(digit)];
            }
        }

        m_sprites.push_back(std::make_unique<sf::Sprite>(texture));
        m_sprites.back()->setTextureRect(texRect);
        m_sprites.back()->setPosition(sf::Vector2f{position.x + xOffset, position.y});

        xOffset += texRect.size.x;
    }
}

void PointsParticle::update(float deltaTime) {
    m_lifetime -= deltaTime;

    if (m_lifetime <= 0.f) {
        m_alive = false;
    }
}

void PointsParticle::draw(sf::RenderTarget& target) const {
    for (const auto& sprite : m_sprites) {
        target.draw(*sprite);
    }
}

bool PointsParticle::isAlive() const {
    return m_alive;
}