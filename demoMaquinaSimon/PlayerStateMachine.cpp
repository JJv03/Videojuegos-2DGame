#include "PlayerStateMachine.hpp"


PlayerStateMachine::PlayerStateMachine() {
    this->addState(std::make_unique<PlayerIdleState>());
}


void PlayerStateMachine::addState(StateRef newState)
{
    this->isAdding = true;
    this->newState = std::move(newState);
}

void PlayerStateMachine::replaceState(StateRef newState)
{
    this->isReplacing = true;
    this->newState = std::move(newState);
}

void PlayerStateMachine::removeState()
{
    this->isRemoving = true;
}

void PlayerStateMachine::processStateChanges(Player& player)
{
    if (this->isRemoving && !this->states.empty())
    {
        this->states.pop();

        if (!this->states.empty())
        {
            this->states.top()->start(player);
        }

        this->isRemoving = false;
    }

    if (this->isAdding)
    {
        if (!this->states.empty())
        {
            this->states.top()->pause(player);
        }

        this->states.push(std::move(this->newState));
        this->states.top()->init(player);
        this->isAdding = false;
    }

    if (this->isReplacing)
    {
        if (!this->states.empty())
        {
            this->states.pop();
        }
        this->states.push(std::move(this->newState));
        this->states.top()->init(player);
        this->isReplacing = false;
    }
}

StateRef& PlayerStateMachine::getActiveState()
{
    return this->states.top();
}