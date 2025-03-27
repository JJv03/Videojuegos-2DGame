#pragma once
#include "gameStateMachine.h"

class Castlevania{
public:
    // Manages the game states like Menu, Game, Pause, Config... with a stack
    GameStateMachine states;

    // Method that runs the whole Castlevania game
    void run();
};