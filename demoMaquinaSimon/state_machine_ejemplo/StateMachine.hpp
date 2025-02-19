#pragma once

#include <memory>
#include <stack>

class State;

typedef std::unique_ptr<State> StateRef;

class StateMachine
{
    public:
        StateMachine();
        virtual ~StateMachine() = default;

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