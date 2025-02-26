#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "tilemap.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({128, 64}), "Tilemap", sf::Style::Default);
    TileMap tileMap;

    int tileExample[] = {50, 1, 2, 3, 61, 72, 9, 9};

    // Cargar el mapa de tiles
    if (!tileMap.load("../assets/tilesets/tileset_1.png", tileExample, 4, 2)){
        return -1;
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        sf::RenderStates states;
        states.texture = &tileMap.m_tileset; // Asignar la textura
        window.draw(tileMap.m_vertices, states); // Dibujar el tilemap
        window.display();
    }

    return 0;
}
