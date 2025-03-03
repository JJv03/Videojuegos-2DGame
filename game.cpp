#include <iostream>
#include "game.h"
#include "resources.h"

/* Creo que hay que quitarlo
void gestionarEventos() {
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            switch (keyPressed->scancode)
            {
                case sf::Keyboard::Scancode::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Scancode::Z:
                    ataque = true;
                    std::cout << "Ataque activado" << std::endl;
                    break;
                case sf::Keyboard::Scancode::Up:
                    haciaArriba = true;
                    if (isOnGround)
                    {
                        verticalSpeed = -JUMP_FORCE;
                        isOnGround = false;
                    }
                    break;
                case sf::Keyboard::Scancode::Down:
                    verticalSpeed = JUMP_FORCE * 0.5f;
                    break;
                case sf::Keyboard::Scancode::Left:
                    haciaIzquierda = true;
                    haciaDerecha = false;
                    gSimonSprite->setScale({1.f, 1.f});
                    break;
                case sf::Keyboard::Scancode::Right:
                    haciaDerecha = true;
                    haciaIzquierda = false;
                    gSimonSprite->setScale({-1.f, 1.f});
                    break;
                default:
                    break;
            }
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            switch (keyReleased->scancode)
            {
                case sf::Keyboard::Scancode::Up:
                    haciaArriba = false;
                    break;
                case sf::Keyboard::Scancode::Left:
                    haciaIzquierda = false;
                    break;
                case sf::Keyboard::Scancode::Right:
                    haciaDerecha = false;
                    break;
                default:
                    break;
            }
        }
    }
}

*/

void Update(sf::RenderWindow &window, float deltaTime)
{
    //gestionarEventos(window);
}
