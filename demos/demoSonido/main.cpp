#include "soundManager.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    SoundManager soundManager;

    // Cargar sonidos
    soundManager.loadSound("explosion", "assets/sounds/04.wav");
    soundManager.loadSound("jump", "assets/sounds/23.wav");

    // Cargar música
    soundManager.loadMusic("background", "assets/music/05Wicked_Child(2).mp3");

    // Crear una ventana para capturar eventos
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SoundManager Test");

    std::cout << "Presiona:\n"
              << "  - Espacio para reproducir 'jump'\n"
              << "  - E para reproducir 'explosion'\n"
              << "  - M para iniciar la música\n"
              << "  - S para detener la música\n"
              << "  - Esc para salir\n";

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } 
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->scancode)
                {
                    case sf::Keyboard::Scancode::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Scancode::Space:
                        std::cout << "Space\n";
                        soundManager.playSound("jump");
                        break;
                    case sf::Keyboard::Scancode::E:
                        std::cout << "E\n";
                        soundManager.playSound("explosion");
                        break;
                    case sf::Keyboard::Scancode::M:
                        std::cout << "M\n";
                        soundManager.playMusic("background", true);
                        break;
                    case sf::Keyboard::Scancode::S:
                        std::cout << "S\n";
                        soundManager.stopMusic("background");
                        break;
                    default:
                        break;
                }
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
