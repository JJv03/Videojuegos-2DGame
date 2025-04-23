#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Particle {
public:
    virtual ~Particle() = default;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual bool isAlive() const = 0;
};


class BreakBlockParticle : public Particle {
public:
    BreakBlockParticle(const sf::Texture& texture, sf::Vector2f position);

    void update(float dt) override;
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
