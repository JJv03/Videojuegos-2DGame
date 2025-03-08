#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "castlevania.h"
#include "gameState.h"
#include "camera.h"
#include "game.h"
#include "gameStateMachine.h"
#include <iostream>

constexpr int escala { 1 };
constexpr int gWindowWidth { 768 * escala };
constexpr int gWindowHeight { 250 * escala };

// Cámara
Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

void Castlevania::run(){
    Game game;

    states.addState(std::make_unique<MenuGS>(&states));
    states.processStateChanges();

    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castleveina", sf::Style::Default);
    StateRef& currentState = states.getActiveState();
    std::cout<<"Antes Init"<<std::endl;
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
        StateRef& currentState = states.getActiveState();

        float deltaTime = deltaClock.restart().asSeconds();

        while (auto eventOpt = window.pollEvent()) 
        {
            if (eventOpt->is<sf::Event::Closed>())
            {
                window.close();
            } else {
                currentState->handleInput(game, *eventOpt);
            }
        }

        currentState->update(game, deltaTime);
        window.clear();
        currentState->draw(game, window);
        window.display();

        states.processStateChanges();
    }
}


