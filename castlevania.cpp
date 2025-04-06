#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "castlevania.h"
#include "gameState.h"
#include "camera.h"
#include "game.h"
#include "gameStateMachine.h"
#include "globals.h" 
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>
#include "utils.h"

// constexpr int escala { 1 };
// int minWindowWidth = 400;
// int minWindowHeight = 400;

// Cámara
Camera camera(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight))));

void Castlevania::run(){
    states.addState(std::make_unique<MenuGS>(&states));
    states.processStateChanges();

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(gWindowWidth, gWindowHeight)), "Castleveina", sf::Style::Default);
    //sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castleveina", sf::Style::Default, sf::State::Fullscreen);
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::Image icon;
    if (!icon.loadFromFile("./assets/sprites/icon.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return;
    }
    window.setIcon(icon);

    sf::Clock deltaClock;

    // MAIN GAME LOOP

    while (window.isOpen())
    {
        GameStateRef& currentState = states.getActiveState();

        float deltaTime = deltaClock.restart().asSeconds();
        deltaTime = std::min(deltaTime, 1/60.f);

        while (auto eventOpt = window.pollEvent()) 
        {
            if (eventOpt->is<sf::Event::Closed>()){
                window.close();
            }
            else {
                currentState->handleInput(*eventOpt);
            }
        }

        
        sf::View view = currentState->getView(window, camera);
        window.setView(view);

        currentState->update(deltaTime, getVirtualUpperLeftCornerCoordOfGameView(window));
        
        window.clear();
        currentState->draw(window, camera);
        window.display();

        states.processStateChanges();
        
        // Esto va muy bien para ver las cosas a cámara lenta xD
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}


