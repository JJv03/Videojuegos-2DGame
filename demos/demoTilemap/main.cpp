#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
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


int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 500}), "Tilemap", sf::Style::Default);
    TileMap tileMap;

    

    // Cargar el mapa de tiles
    if (!tileMap.load("../../assets/tilesets/tileset_1.png", "../../assets/tilesets/tilemap_1_1.txt", 24, 7)){
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

        drawHitboxes(tileMap.m_solidTiles, window, tileMap.m_tileSize);

        window.display();
    }

    return 0;
}
