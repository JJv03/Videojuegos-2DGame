#include "demo.h"
#include "resources.h"
#include <iostream>

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    // En este caso primero se extrae en formato imagen para realizar las conversiones primero.
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/NES - Castlevania - Simon Belmont.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    // Finalemente se almacena en la clase de recursos
    if (!Resources::textures["simon"].loadFromImage(simonImage))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }
}

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime)
{
}

// Renderiza los elementos en la ventana
void Render(Renderer &renderer)
{
    renderer.Draw(Resources::textures["simon"], sf::Vector2f(), sf::Vector2f(7.5, 5.5), sf::IntRect({1, 21}, {16, 32}));
}
