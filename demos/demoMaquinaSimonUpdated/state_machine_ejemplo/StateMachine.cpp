#include "StateMachine.hpp"


StateMachine::StateMachine() {
    this->addState(std::make_unique<State>());
}


void StateMachine::addState(StateRef newState)
{
    this->isAdding = true;
    this->newState = std::move(newState);
}

void StateMachine::replaceState(StateRef newState)
{
    this->isReplacing = true;
    this->newState = std::move(newState);
}

void StateMachine::removeState()
{
    this->isRemoving = true;
}

void StateMachine::processStateChanges()
{
    if (this->isRemoving && !this->states.empty())
    {
        this->states.pop();

        if (!this->states.empty())
        {
            //this->states.top()->start();
        }

        this->isRemoving = false;
    }

    if (this->isAdding)
    {
        if (!this->states.empty())
        {
            //this->states.top()->pause();
        }

        this->states.push(std::move(this->newState));
        //this->states.top()->init();
        this->isAdding = false;
    }

    if (this->isReplacing)
    {
        if (!this->states.empty())
        {
            this->states.pop();
        }
        this->states.push(std::move(this->newState));
        //this->states.top()->init();
        this->isReplacing = false;
    }
}

StateRef& StateMachine::getActiveState()
{
    return this->states.top();
}