#pragma once

#include <memory>
#include <stack>
#include "gameState.h"

typedef std::unique_ptr<GameState> GameStateRef;

class GameStateMachine
{
    public:
        GameStateMachine();
        virtual ~GameStateMachine() = default;

        void addState(GameStateRef newState);
        void replaceState(GameStateRef newState);
        void removeState();

        void processStateChanges();

        GameStateRef& getActiveState();

    private:
        std::stack<GameStateRef> states;
        GameStateRef newState;

        bool isRemoving;
        bool isAdding;
        bool isReplacing;
};