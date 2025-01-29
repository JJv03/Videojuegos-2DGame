#include "demo.h"
#include "resources.h"
#include "map.h"
#include <iostream>

Map map;
Camera camera(320.0f);

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    // En este caso primero se extrae en formato imagen para realizar las conversiones primero, después se almacena en la clase de recursos
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/NES - Castlevania - Simon Belmont.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["simon"].loadFromImage(simonImage))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }

    // Creación textura de ejemplo cuadrado
    sf::Image squareImage;
    if (!squareImage.loadFromFile("../assets/others/square.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }
    squareImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["square"].loadFromImage(squareImage))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }

    // Incializacion del grid del mapa
    map.createCheckerboard(10, 10);

    // Posición inicial de la camara
    camera.position = sf::Vector2f(160.0f, 160.0f);
}

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime)
{
}

// Renderiza los elementos en la ventana
void Render(Renderer &renderer)
{
    map.Draw(renderer);
    // renderer.DrawCut(Resources::textures["simon"], sf::Vector2f(), sf::Vector2f(7.5, 5.5), sf::IntRect({1, 21}, {16, 32}));
}
