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

        void AddState(StateRef newState);
        void ReplaceState(StateRef newState);
        void RemoveState();

        void ProcessStateChanges();

        StateRef& GetActiveState();

    private:
        std::stack<StateRef> states;
        StateRef newState;

        bool isRemoving;
        bool isAdding;
        bool isReplacing;
};