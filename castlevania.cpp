#include <SFML/Graphics>
#include <SFML/Window>
#include "castlevania.h"
#include "gameState.h"

constexpr int escala { 1 };
constexpr int gWindowWidth { 768 * escala };
constexpr int gWindowHeight { 250 * escala };

// Cámara
Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

void Castlevania::run(){
    Game game;

    states.addState(std::make_unique<MenuGS>());
    states.processStateChanges();

    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castlevania", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    // BUCLE PRINCIPAL DEL JUEGO

    while (window.isOpen())
    {
        StateRef& currentState = stateMachine.getActiveState();

        float deltaTime = deltaClock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            currentState->handleInput(game, event);
        }

        currentState->update(game, deltatime);
        currentState->draw(game, window);

        states.processStateChanges();
    }
}


