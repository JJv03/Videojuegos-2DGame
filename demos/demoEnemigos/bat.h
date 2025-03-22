#pragma once

#include "enemy.h"

class Bat : public Enemy
{
private:
    const sf::Vector2f BAT_SPEED = {-75.0f, 0.0f};

    const float BAT_LIFE = 1.0f;
    const float BAT_SCORE = 200.0f;
    const float BAT_DAMAGE = 2.0f;

    const float ANIM_FRAME_TIME = 0.2f;
    const int TOTAL_FRAMES = 2;

    const float OSCILLATION_SPEED = 2.5f;
    const float OSCILLATION_AMPLITUDE = 0.5f;
    float spawnTime = 0.0f;

    // Variables del spawner
    sf::FloatRect spawnZone;

    bool spawnerActive{false};
    bool batToSpawn{false};

    float batSpawnTimers = 0.0f;

    float animTimer = 0.0f;
    int currentFrame = 0;

public:
    Bat() = default;
    Bat(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position, const sf::Vector2f &zoneSize);

    static Bat createBat(const sf::Vector2f &position, const sf::Vector2f &zoneSize);

    void resetPosition() override;
    void movePositionToBorder(const sf::FloatRect &playerActivationZone);
    void draw(sf::RenderWindow &window, bool debugDraw) override;

    void update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone);
    void checkCollisions(const sf::FloatRect simonBounds, const sf::FloatRect &weaponBounds,
                         const std::vector<sf::FloatRect> &boundsList, const bool playerIsAtacking, const float playerDamage);

    void updateAnimation(float deltaTime);
};