#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "demo.h"
#include "camera.h"

using namespace std;

// Definición de constantes para la escala y dimensiones de la ventana
const int escala{1};
const int gWindowWidth{1200 * escala};
const int gWindowHeight{900 * escala};
const string windowName{"demo"};

int main()
{
    // Creación de la ventana SFML
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), windowName, sf::Style::Default);
    sf::Clock deltaClock; // Reloj para calcular deltaTime (tiempo real, sin dependencia de la velocidad del procesador)
    Camera camera;        // Instancia de la cámara

    Init(window); // Inicialización del entorno

    while (window.isOpen())
    {
        // Calcula el tiempo transcurrido entre fotograma
        float deltaTime = deltaClock.restart().asSeconds();

        // Manejo de eventos de la ventana
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) // Cerrar la ventana si se detecta el evento de cierre
            {
                window.close();
            }
        }

        window.setView(camera.GetView(window.getSize())); // Actualiza la vista de la cámara
        Update(deltaTime);                                // Llama a la función de actualización lógica

        window.clear(); // Limpia la ventana antes de renderizar

        Render(window); // Dibuja los elementos en pantalla

        window.display(); // Muestra el contenido renderizado en la ventana
    }
}
