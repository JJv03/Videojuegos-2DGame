#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include "tilemap.h"
#include "camera.h"


// Cámara
const sf::Vector2f gViewOrigin {0.f, 27.f};
const sf::Vector2f gViewSize { 256.f, 175.f };
Camera camera(sf::FloatRect(gViewOrigin, gViewSize));

sf::Sprite* gSimonSprite { nullptr };


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

    // Variables para controlar el movimiento de la cámara
    float simonCurrentPositionX {0.0f};
    float simonNewPositionX {camera.startVertex.x + camera.viewSize.x * 0.5f};
    

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

        if (abs(simonNewPositionX - simonCurrentPositionX) > 0.01f) {
            camera.startVertex += sf::Vector2f{simonNewPositionX - simonCurrentPositionX, 0.f};
            //std::cout << "UpdateView: " << simonNewPositionX - simonCurrentPositionX << std::endl;
        }
        window.setView(camera.GetView(window.getSize()));

        window.clear(); 
        window.draw(tileMap);

        drawHitboxes(tileMap.m_solidTiles, window, tileMap.m_tileSize);

        window.display();
    }

    return 0;
}
