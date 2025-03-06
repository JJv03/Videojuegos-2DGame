#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "tilemap.h"


sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect)
{
    if (floatRect.size.x == 0.0f || floatRect.size.y == 0.0f)
    {
        return sf::RectangleShape();
    }

    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineColor(sf::Color::Red);
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

void drawHitboxes(const std::vector<std::vector<TileMap::SolidTileAttributes>>& solidTiles, sf::RenderWindow& window, float tileSize)
{
    for (size_t i = 0; i < solidTiles.size(); ++i) {
        for (size_t j = 0; j < solidTiles[i].size(); ++j) {
            sf::RectangleShape rect = FloatRectToRectShape(solidTiles[i][j].hitbox);
            rect.setPosition({static_cast<float>(j) * tileSize, static_cast<float>(i) * tileSize});
            window.draw(rect);
        }
    }
}



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

        /* for (size_t i = 0; i < tileMap.m_solidTiles.size(); ++i) {
            for (size_t j = 0; j < tileMap.m_solidTiles[i].size(); ++j) {
                sf::RectangleShape rect = FloatRectToRectShape(tileMap.m_solidTiles[i][j].hitbox);
                rect.setPosition({static_cast<float>(j) * tileMap.m_tileSize, static_cast<float>(i) * tileMap.m_tileSize});
                window.draw(rect);
            }
        } */

       drawHitboxes(tileMap.m_solidTiles, window, tileMap.m_tileSize);

        window.display();

        //std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    return 0;
}
