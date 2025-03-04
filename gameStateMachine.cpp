#include "gameStateMachine.hpp"


GameStateMachine::GameStateMachine() {
    this->addState(std::make_unique<State>());
}


void GameStateMachine::addState(StateRef newState)
{
    this->isAdding = true;
    this->newState = std::move(newState);
}

void GameStateMachine::replaceState(StateRef newState)
{
    this->isReplacing = true;
    this->newState = std::move(newState);
}

void GameStateMachine::removeState()
{
    this->isRemoving = true;
}

void GameStateMachine::processStateChanges()
{
    if (this->isRemoving)
    {
        if (!this->states.empty()){
            this->states.top()->close();
            this->states.pop();
    
            if (!this->states.empty())
            {
                this->states.top()->resume();
            }
        }
        
        this->isRemoving = false;
    }

    if (this->isAdding)
    {
        if (!this->states.empty())
        {
            this->states.top()->pause();
        }

        this->states.push(std::move(this->newState));
        this->states.top()->init();
        this->isAdding = false;
    }

    if (this->isReplacing)
    {
        if (!this->states.empty())
        {
            this->states.top()->close();
            this->states.pop();
        }
        this->states.push(std::move(this->newState));
        this->states.top()->init();
        this->isReplacing = false;
    }
}

StateRef& GameStateMachine::getActiveState()
{
    return this->states.top();
}