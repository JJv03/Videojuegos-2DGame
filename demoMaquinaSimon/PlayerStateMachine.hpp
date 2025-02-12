#pragma once

#include <memory>
#include <stack>

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerState;
class Player;

typedef std::unique_ptr<PlayerState> StateRef;

class PlayerStateMachine
{
    public:
        PlayerStateMachine();
        virtual ~PlayerStateMachine() = default;

        void addState(StateRef newState);
        void replaceState(StateRef newState);
        void removeState();

        void processStateChanges(Player& player);

        StateRef& getActiveState();

    private:
        std::stack<StateRef> states;
        StateRef newState;

        bool isRemoving;
        bool isAdding;
        bool isReplacing;
};