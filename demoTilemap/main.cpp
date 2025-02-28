#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "tilemap.h"
#include <iostream>
#include <fstream>

void leerNumeros(const std::string& archivo, std::vector<int>& listaNumeros) {
    std::ifstream file(archivo);
    std::string linea;
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return;
    }

    while (std::getline(file, linea)) {
        std::stringstream ss(linea);
        std::string numeroStr;
        
        // Procesar cada número separado por coma
        while (std::getline(ss, numeroStr, ',')) {
            try {
                int numero = std::stoi(numeroStr); // Convertir de string a int
                listaNumeros.push_back(numero); // Añadir el número a la lista
            } catch (const std::invalid_argument&) {
                // Si no se puede convertir el número, lo ignoramos
                std::cerr << "Número inválido en el archivo: " << numeroStr << std::endl;
            }
        }
    }

    file.close();
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 500}), "Tilemap", sf::Style::Default);
    TileMap tileMap;

    std::vector<int> tileExample;
    leerNumeros("../assets/tilesets/tilemap_1_1.txt", tileExample);

    // Cargar el mapa de tiles
    if (!tileMap.load("../assets/tilesets/tileset_1.png", tileExample, 24, 7)){
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
        window.draw(tileMap);
        window.display();
    }

    return 0;
}
