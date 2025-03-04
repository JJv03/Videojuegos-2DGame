#pragma once

#include <memory>
#include <stack>
#include "gameState.h"

typedef std::unique_ptr<GameState> StateRef;

class GameStateMachine
{
    public:
        GameStateMachine();
        virtual ~GameStateMachine() = default;

        void addState(StateRef newState);
        void replaceState(StateRef newState);
        void removeState();

        void processStateChanges();

        StateRef& getActiveState();

    private:
        std::stack<StateRef> states;
        StateRef newState;

        bool isRemoving;
        bool isAdding;
        bool isReplacing;
};