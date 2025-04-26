#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include "animationManager.h"


class Particle {
public:
    virtual ~Particle() = default;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual bool isAlive() const = 0;
};

// =================================================================================================
// ===================================== BREAK BLOCK PARTICLE ======================================
// =================================================================================================

class BreakBlockParticle : public Particle {
public:
    BreakBlockParticle(const sf::Texture& texture, sf::Vector2f position);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    bool isAlive() const override;

private:
    struct Debris {
        sf::Sprite sprite;
        sf::Vector2f velocity;
    };

    std::vector<Debris> m_debris;
    float m_gravity;
    float m_lifetime;
    bool m_alive;
};


// =================================================================================================
// ===================================== HIT PARTICLE ==============================================
// =================================================================================================

class HitParticle : public Particle {
public:
    HitParticle(const sf::Texture& texture, sf::Vector2f position);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    bool isAlive() const override;

private:
    float m_lifetime;
    bool m_alive;
    std::unique_ptr<sf::Sprite> m_sprite;
};


// =================================================================================================
// ===================================== FIRE PARTICLE =============================================
// =================================================================================================

class FireParticle : public Particle {
public:
    FireParticle(const sf::Texture& texture, sf::Vector2f position);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    bool isAlive() const override;

private:
    float m_lifetime;
    bool m_alive;
    std::unique_ptr<sf::Sprite> m_sprite;
    std::unique_ptr<AnimationManager> m_animationManager{nullptr};
    AnimationManager::Animation m_animation;
};


// =================================================================================================
// ================================== BIG FIRE PARTICLE ============================================
// =================================================================================================

class BigFireParticle : public Particle {
public:
    BigFireParticle(const sf::Texture& texture, sf::Vector2f position);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    bool isAlive() const override;

private:
    float m_lifetime;
    bool m_alive;
    std::unique_ptr<sf::Sprite> m_sprite;
    std::unique_ptr<AnimationManager> m_animationManager{nullptr};
    AnimationManager::Animation m_animation;
};


// =================================================================================================
// ===================================== POINTS PARTICLE ===========================================
// =================================================================================================

extern std::unordered_map<std::string, sf::IntRect> pointsTextRect;


class PointsParticle : public Particle {
public:
    PointsParticle(const sf::Texture& texture, sf::Vector2f position, int points);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    bool isAlive() const override;

private:
    float m_lifetime;
    bool m_alive;
    std::vector<std::unique_ptr<sf::Sprite>> m_sprites;

    // Returns the first two digits of a number as a string
    std::string getFirstTwoDigits(int digit);

    // Returns 'true' if the digits for the index 'index' are "00"
    bool isDoubleZeroDigits(int digit, int index, const std::vector<int>& digits);
};