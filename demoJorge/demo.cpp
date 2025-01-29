#include "demo.h"
#include <iostream>

// Variables globales
sf::Texture simonTexture;

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/Simon.png"))
    {
        std::cerr << "Error cargando la imagen" << std::endl;
        exit(-1);
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!simonTexture.loadFromImage(simonImage))
    {
        std::cerr << "Error cargando la textura" << std::endl;
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
    renderer.Draw(simonTexture, sf::Vector2f(), sf::Vector2f(7.5, 5.5), sf::IntRect({1, 21}, {16, 32}));
}
