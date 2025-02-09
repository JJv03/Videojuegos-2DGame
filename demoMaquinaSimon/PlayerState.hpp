#pragma once

class PlayerState
{
public:
    PlayerState() {}
    ~PlayerState() {}

    virtual void init() = 0;
    virtual void handleInput() = 0;
    virtual void draw(float deltaTime) = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;
};