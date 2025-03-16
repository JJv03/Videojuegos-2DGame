#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include "tilemap.h"
#include "camera.h"
#include <cmath>

// Cámara
const sf::Vector2f gViewOrigin {0.f, 0.f};
const sf::Vector2f gViewSize { 256.f, 175.f };
Camera camera(sf::FloatRect(gViewOrigin, gViewSize));
std::unordered_map<std::string, sf::Texture> gTextures;

constexpr float gMovementSpeed { 50.0f };


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



bool updateMovement(const float deltaTime, const bool haciaIzquierda, const bool haciaDerecha)
{
    if (!gSimonSprite)
    {
        std::cerr << "Error: sprite de Simon no inicializado" << std::endl;
        return false;
    }
    
    if (haciaIzquierda)
    {
        gSimonSprite->move({-1.5f * deltaTime * gMovementSpeed, 0.f});
    }
    else if (haciaDerecha)
    {
        gSimonSprite->move({1.5f * deltaTime * gMovementSpeed, 0.f});
    }

    return true;
}


int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 500}), "Tilemap", sf::Style::Default);
    TileMap tileMap;

    // Variables para controlar el movimiento de la cámara
    float simonCurrentPositionX {camera.startVertex.x + (camera.GetView(window.getSize()).getSize().x / 2.f)};
    float simonNewPositionX {camera.startVertex.x + (camera.GetView(window.getSize()).getSize().x / 2.f)};
    
    // Variables para controlar el movimiento
    bool haciaIzquierda { false };
    bool haciaDerecha { false };

    sf::Clock deltaClock;

    // Cargar el mapa de tiles
    if (!tileMap.load("../../assets/tilesets/tileset_1.png", "../../assets/tilesets/tilemap_1_1.txt", 24, 7)){
        return -1;
    }

    // Cargar imagen y configurar textura de Simon (aplicando color key)
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../../assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return false;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    gTextures["simon"] = sf::Texture(simonImage, false);
    sf::Sprite simonSprite(gTextures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    float simonStartX = camera.startVertex.x + (camera.GetView(window.getSize()).getSize().x / 2.f);
    float simonStartY = 160.f;  // Mantén la altura como antes
    simonSprite.setPosition({simonStartX, simonStartY});

    
    // Ajustar la cámara a la posición inicial de Simón
    camera.startVertex.x = simonStartX - (camera.GetView(window.getSize()).getSize().x / 2.f);

    sf::FloatRect bounds = simonSprite.getLocalBounds();
    
    // Ajusta el origen de las transformaciones al centro inferior
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSimonSprite = &simonSprite;

    while (window.isOpen()) {
        float deltaTime = deltaClock.restart().asSeconds(); // tiempo transcurrido entre fotograma
        simonCurrentPositionX = gSimonSprite->getPosition().x;

    
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
                        std::cout << "Ataque activado" << std::endl;
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

        if (!updateMovement(deltaTime, haciaIzquierda, haciaDerecha))
        {
            std::cerr << "Error en el update" << std::endl;
            return -1;
        }
        
        simonNewPositionX = gSimonSprite->getPosition().x;

        camera.startVertex.x = simonNewPositionX - (camera.GetView(window.getSize()).getSize().x / 2.f);
        
        sf::View view = camera.GetView(window.getSize());

        sf::Vector2f roundedCenter(
            static_cast<float>(floor(view.getCenter().x * 100) / 100), 
            static_cast<float>(floor(view.getCenter().y * 100) / 100)
        );

        view.setCenter(roundedCenter);

        window.setView(view);

        simonCurrentPositionX = simonNewPositionX;
 
        window.clear(); 

        //window.draw(tileMap);
        tileMap.drawScene(window, camera);

        if (gSimonSprite)
        {
            window.draw(*gSimonSprite);
        }
        drawHitboxes(tileMap.m_solidTiles, window, tileMap.m_tileSize);

        window.display();
    }

    return 0;
}
