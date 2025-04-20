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

        // Method that prepares the state machine so that it will stack
        // <newState> on top of the machine
        void addState(GameStateRef newState);

        // Method that prepares the state machine so that it will
        // remove the current state on top of the machine and stack <newState>
        void replaceState(GameStateRef newState);

        // Method that prepares the state machine so that it will remove all states
        // and replace them with <newState>
        void replaceAllStates(GameStateRef newState);

        // Method that prepares the state machine so that it will remove
        // the current state on top of the machine
        void removeState();

        // Method that processes the changes made by add/replace/removeState.
        // It is separated so that a sudden change doesn't occur in the middle of 
        // the code execution and cause problems
        void processStateChanges();

        // Function that return the reference to the current state on top of the machine
        GameStateRef& getActiveState();

    private:
        std::stack<GameStateRef> states;
        // Next state that will be used in processStateChanges()
        GameStateRef newState;

        // Indicates processStateChanges() to remove the current state
        bool isRemoving;

        // Indicates processStateChanges to add <newState>
        bool isAdding;

        // Indicates processStateChanges to replace the current state with <newState>
        bool isReplacing;

        bool isReplacingAll;
};