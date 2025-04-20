#include "gameStateMachine.h"


GameStateMachine::GameStateMachine() {
    this->isAdding = false;
    this->isReplacing = false;
    this->isRemoving = false;
    this->isReplacingAll = false;
}

void GameStateMachine::addState(GameStateRef newState)
{
    this->isAdding = true;
    this->newState = std::move(newState);
}

void GameStateMachine::replaceState(GameStateRef newState)
{
    this->isReplacing = true;
    this->newState = std::move(newState);
}

void GameStateMachine::replaceAllStates(GameStateRef newState)
{
    this->isReplacingAll = true;
    this->newState = std::move(newState);
}

void GameStateMachine::removeState()
{
    this->isRemoving = true;
}

void GameStateMachine::processStateChanges()
{
    if (this->isReplacingAll)
    {
        while (!this->states.empty())
        {
            this->states.top()->close();
            this->states.pop();
        }

        this->states.push(std::move(this->newState));
        this->states.top()->init();
        this->isReplacingAll = false;

        // Nos aseguramos de no ejecutar también isReplacing o isAdding accidentalmente
        this->isAdding = false;
        this->isReplacing = false;
        this->isRemoving = false;
        return; // Evitamos procesar más cambios
    }
    
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

GameStateRef& GameStateMachine::getActiveState()
{
    return this->states.top();
}