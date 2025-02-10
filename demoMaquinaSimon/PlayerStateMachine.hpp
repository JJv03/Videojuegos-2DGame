#pragma once

#include <memory>
#include <stack>

#include "PlayerState.hpp"


typedef std::unique_ptr<PlayerState> StateRef;

class PlayerStateMachine
{
    public:
        PlayerStateMachine() {}
        ~PlayerStateMachine() {}

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