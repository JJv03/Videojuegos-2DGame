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
#include "configManager.h"
#include <fstream>

// constexpr int escala { 1 };
// int minWindowWidth = 400;
// int minWindowHeight = 400;

// Cámara
Camera camera(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight))));

void Castlevania::run(){
    std::ifstream archivo("./assets/DONOTDELETE/PLEASE/IBEGYOU/coconut.png");

    if (!archivo) {  // Si no se puede abrir el archivo (no existe, por ejemplo)
        std::cerr << "THE COCONUT HAS DISAPPEARED..." << std::endl;
        return;
    }

    if (gSkipAnims) {
        states.addState(std::make_unique<GameGS>(&states));
    }
    else{
        states.addState(std::make_unique<InitMenuGS>(&states));
    }

    //states.addState(std::make_unique<InitMenuGS>(&states));
    states.processStateChanges();

    configManager &configManager = configManager.getInstance();

    sf::VideoMode screenMode = configManager.getVideo().window_mode ? sf::VideoMode::getDesktopMode() : sf::VideoMode(sf::Vector2u(gWindowWidth, gWindowHeight));
    sf::State state = configManager.getVideo().window_mode ? sf::State::Fullscreen : sf::State::Windowed;

    sf::RenderWindow window(screenMode, "Castlevania", state);
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

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

        //sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        //std::cout << "Mouse View X: " << worldPos.x << ", Y: " << worldPos.y << std::endl;
        
        sf::View view = currentState->getView(window, camera);
        window.setView(view);

        currentState->update(deltaTime, getVirtualUpperLeftCornerCoordOfGameView(window), window.hasFocus());
        
        window.clear();
        currentState->draw(window, camera);
        
        auto newState = configManager.getVideo().window_mode ? sf::State::Fullscreen : sf::State::Windowed;
        if (state != newState){
            state = newState;
            sf::VideoMode screenMode = configManager.getVideo().window_mode ? sf::VideoMode::getDesktopMode() : sf::VideoMode(sf::Vector2u(gWindowWidth, gWindowHeight));
            window.create(screenMode, "Castlevania", state);
            window.setIcon(icon);
        }

        window.display();

        states.processStateChanges();
        
        // Esto va muy bien para ver las cosas a cámara lenta xD
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}


