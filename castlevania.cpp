#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
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

constexpr int escala { 1 };
int minWindowWidth = 400;
int minWindowHeight = 400;

// Cámara
Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));
sf::View view(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

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
            } else if (const auto* resized = eventOpt->getIf<sf::Event::Resized>()) {                
                float newResizedWidth = std::max(static_cast<int>(resized->size.x), minWindowWidth);
                float newResizedHeight = std::max(static_cast<int>(resized->size.y), minWindowHeight);

                scaleX = static_cast<float>(newResizedWidth) / gWindowWidth;
                scaleY = static_cast<float>(newResizedHeight) / gWindowHeight;
                
                gWindowWidth = static_cast<float>(newResizedWidth);
                gWindowHeight = static_cast<float>(newResizedHeight);

                std::cout << "Resized to: " << gWindowWidth << "x" << gWindowHeight << std::endl;

                windowScaleFactor = std::min(
                    static_cast<float>(gWindowWidth) / originalWindowWidth,
                    static_cast<float>(gWindowHeight) / originalWindowHeight
                );
                
                view.setSize(sf::Vector2f(gWindowWidth, gWindowHeight));
                view.setCenter(sf::Vector2f(gWindowWidth / 2, gWindowHeight / 2));
                window.setView(view);
                window.setSize({gWindowWidth, gWindowHeight});
            }
            else {
                currentState->handleInput(game, *eventOpt);
            }
        }

        currentState->update(game, deltaTime, windowScaleFactor); // Pasar windowScaleFactor
        window.clear();
        window.setView(view);
        currentState->draw(game, window, windowScaleFactor); // Pasar windowScaleFactor
        window.display();

        states.processStateChanges();
    }
}