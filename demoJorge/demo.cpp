#include "demo.h"

sf::RectangleShape rectangle;

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    float width = 1.0f;
    float height = 2.0f;

    rectangle.setSize(sf::Vector2f(width, height));                 // Establece el tamaño
    rectangle.setOrigin(sf::Vector2f(width / 2.0f, height / 2.0f)); // Centra el origen
    rectangle.setFillColor(sf::Color::Transparent);                 // Hace el relleno transparente
    rectangle.setOutlineThickness(0.02f);                           // Define el grosor del contorno
    rectangle.setOutlineColor(sf::Color(0, 255, 0));                // Color verde para el contorno
}

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime)
{
}

// Renderiza los elementos en la ventana
void Render(sf::RenderWindow &window)
{
    window.draw(rectangle);
}