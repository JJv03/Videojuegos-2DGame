#pragma once

#include "PlayerState.hpp"

class PlayerWalkState : public PlayerState
{
    PlayerWalkState() {}
    ~PlayerWalkState() {}

    void init() override;
    void handleInput(Player& player, sf::Event event) override;
    void update(Player& player, float deltaTime) override;
    void draw(Player& player, sf::RenderWindow &window) override;

    void pause() override;
    void resume() override;
};