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

// constexpr int escala { 1 };
// int minWindowWidth = 400;
// int minWindowHeight = 400;

// Cámara
Camera camera(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(gWindowWidth, gWindowHeight)));

void Castlevania::run(){
    states.addState(std::make_unique<MenuGS>(&states));
    states.processStateChanges();

    //sf::RenderWindow window(sf::VideoMode(sf::Vector2u(gWindowWidth, gWindowHeight)), "Castleveina", sf::Style::Default);
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castleveina", sf::Style::Default, sf::State::Fullscreen);
    window.setVerticalSyncEnabled(true);

    sf::Image icon;
    if (!icon.loadFromFile("./assets/sprites/icon.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return;
    }
    window.setIcon(icon);

    sf::Clock deltaClock;

    // BUCLE PRINCIPAL DEL JUEGO

    while (window.isOpen())
    {
        GameStateRef& currentState = states.getActiveState();

        float deltaTime = deltaClock.restart().asSeconds();

        while (auto eventOpt = window.pollEvent()) 
        {
            if (eventOpt->is<sf::Event::Closed>()){
                window.close();
            }
            else {
                currentState->handleInput(*eventOpt);
            }
        }

        currentState->update(deltaTime);
        window.clear();
        sf::View view = camera.getView(window.getSize());
        window.setView(view);
        currentState->draw(window, camera);
        window.display();

        states.processStateChanges();
    }
}


