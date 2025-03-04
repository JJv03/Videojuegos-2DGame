#pragma once
#include "gameStateMachine.h"

class Game{
public:
    GameStateMachine states;

    void run();
};