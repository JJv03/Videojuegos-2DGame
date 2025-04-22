#pragma once

#include <SFML/Graphics.hpp>
#include "../entity.h"
#include "../tilemap.h"
#include "../player.h"

class Boss : public EntitySprite
{
private:
    const float GRAVITY = 750.0f; // Gravity force

public:
    configManager &configManager;
    sf::Vector2f speed = {-75.0f, 0.0f}; // Default movement speed (left direction)
    float score = 100.0f;                // Points awarded when defeated
    float damage = 2.0f;                 // Damage dealt to player on collision
    float life = 1.0f;                   // Boss health

    bool isActive = false; // Whether the boss is currently active/visible

    Boss() = default;
    Boss(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes);

    virtual void applyGravity(float deltaTime);

    virtual void draw(sf::RenderWindow &window);

    void onCollision_SolidTile(Entity &solidTile);

    bool applyDamage(float damageAmount, Player &player);

    void hello() const override;
};