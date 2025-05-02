#include "death.h"
#include "../game.h"
#include <iostream>
#include <cmath>

// Constructor: Initialize phantomBat with sprite, hitboxes, position, and game level/stage
Death::Death(std::shared_ptr<sf::Sprite> _sprite, std::vector<sf::FloatRect> &_hitboxes, const sf::Vector2f &position,
                       const int &level, const int &stage, const sf::FloatRect &mapDims) : Boss(_sprite, _hitboxes), level(level), stage(stage), position(position), mapDims(mapDims)
{
    speed = DEATH_SPEED;
    life = DEATH_LIFE;
    score = DEATH_SCORE;
    damage = DEATH_DAMAGE;

    gKilledBoss = false;
}

// Update Death logic: handle spawning, movement, and deactivation
void Death::update(float deltaTime, const sf::FloatRect &playerActivationZone, const sf::FloatRect &playerDeactivationZone,
                        const float playerDir, const sf::FloatRect &playerBounds, const sf::FloatRect &mapBounds)
{
    // SPAWN LOGIC
    if (!isActive && !dead)
    {
        for (const auto &hitbox : hitboxes)
        {
            if (playerActivationZone.findIntersection(hitbox).has_value())
            {
                isActive = true;
                maxLife = life;
                break;
            }
        }
    }

    // MOVEMENT LOGIC
    if (isActive && isInBossFight)
    {   
        // std::cout << "GOAL: " << goal.x << " " << goal.y << std::endl;
        position = sprite->getGlobalBounds().position;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;
        sf::Vector2f map = mapDims.position;
        sf::Vector2f size = mapDims.size;
        // std::cout << "Pos: " << pos.x << " " << pos.y << std::endl;

        // NO SEGÚN SPEED SINO POSICIÓN DE SIMON
        if (speed.x != 0)
        {
            sprite->move({speed.x * deltaTime, 0.f});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.x += speed.x * deltaTime;
            }
        }
        if (speed.y != 0)
        {
            sprite->move({0.f, -speed.y * deltaTime});
            for (auto &hitbox : hitboxes)
            {
                hitbox.position.y -= speed.y * deltaTime;
            }
        }
    }
}

void Death::enhancedAI(bool isOn, const int playerDir, const sf::FloatRect &playerBounds){
    if(isOn && !triedAI){
        triedAI = true;     // To prevent permatrying
        int chance = rand() % 3;
        if(chance == 0){ // Meter factor cercanía del jugador
            // std::cout << "Luckyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy" << std::endl;
            enhancedActivated = true;
            enhancedTimer = 0;
        }
    }
    // In case Enhanced AI mode activate add this possible state (it has a prob to happen, not always). Attack with all scythe
}

void Death::onCollision(Entity &other, Game &game)
{
    if (!isActive || !sprite)
        return;

    if (Whip *whip = dynamic_cast<Whip *>(&other))
    {
        if (!whip->collisionedEntities.contains(this) && applyDamage(whip->whipDmg, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    else if (SubWeapon *subWeapon = dynamic_cast<SubWeapon *>(&other))
    {
        if (!subWeapon->collisionedEntities.contains(this) && applyDamage(subWeapon->subDamage, game.player))
        {
            game.createDropItem(DropType::MAGIC_CRYSTAL, sf::Vector2f(mapDims.position.x + mapDims.size.x / 2, mapDims.position.y + mapDims.size.y / 2));
            dead = true;
            gKilledBoss = true;
            game.particleSystem.spawnBigFireParticle(position, false);
        }
    }
    currentBossLife = life;
}

// Render Death and debug info (spawn zone)
void Death::draw(sf::RenderWindow &window)
{
    if (sprite && isActive && isInBossFight)
    {
        Boss::draw(window);
    }

    if(gDrawHitboxes){
        sf::RectangleShape goalMarker;
        goalMarker.setSize(sf::Vector2f(10.f, 10.f));
        goalMarker.setFillColor(sf::Color::Red);
        goalMarker.setPosition(goal);
        goalMarker.setOrigin(sf::Vector2f(5.f, 5.f));

        window.draw(goalMarker);
    }
}

void Death::resetPosition()
{
    Boss::resetPosition();

    speed = DEATH_SPEED;
    life = DEATH_LIFE;

    currentAnimation = sleepPhantomBat;
    dead = false;
}

void Death::hello() const
{
    std::cout << "Soy Death" << std::endl;
}
