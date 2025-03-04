#include "game.h"
#include "gameState.h"

void Game::run(){
    states.addState(std::make_unique<MenuGS>());
    states.processStateChanges();

    // abrir ventana, deltatime, ...


    // BUCLE PRINCIPAL DEL JUEGO

    // while(window.isOpen){
    //    StateRef& currentState = stateMachine.getActiveState();
    // 
    //    if (currentState) {
    //        while (const std::optional event = window.pollEvent()){
    //            currentState->handleInput(event);
    //        }
    //        currentState->update(deltatime);
    //        currentState->draw(window);
    //    }
    //
    // states.processStateChanges();
    //}
}


