#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include "game.h"
#include "camera.h"
#include "resources.h"

// Variables globales de configuración
bool gEnMovimiento{false};
constexpr int escala{1};
constexpr int gWindowWidth{768 * escala};
constexpr int gWindowHeight{250 * escala};
const sf::Vector2f gViewOrigin{0.f, 27.f};
const sf::Vector2f gViewSize{256.f, 175.f};

std::string formatFPSandTime(float deltaTime)
{
    std::ostringstream oss;

    // Formatea FPS con 2 decimales
    oss << std::fixed << std::setprecision(2) << (1.f / deltaTime);
    std::string fps = oss.str();

    // Limpia el flujo
    oss.str("");
    oss.clear();

    // Formatea el tiempo en milisegundos con 2 decimales
    oss << std::fixed << std::setprecision(2) << (deltaTime * 1000);
    std::string ms = oss.str();

    return fps + " FPS\n" + ms + " ms";
}

int main()
{
    std::string nombreVentana{"Castlevania"};
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Clock deltaClock;

    if (!Init())
    {
        std::cerr << "Error en la inicialización" << std::endl;
        return -1;
    }

    // Variables para controlar el movimiento de la cámara
    Camera camera(sf::FloatRect(gViewOrigin, gViewSize));

    // ESTO NO DEBERIA IR AQUI =========
    float simonCurrentPositionX{0.0f};
    float simonNewPositionX{camera.startVertex.x + camera.viewSize.x * 0.5f};

    sf::Font font;
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cerr << "Error en la carga de la fuente de texto" << std::endl;
        return -1;
    }

    sf::Text text(font);
    text.setString("60 FPS\n16.67 ms");
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    //====================================

    // Bucle principal del juego
    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds(); // tiempo transcurrido entre fotograma

        // ESTO NO DEBERIA IR AQUI =========
        text.setString(formatFPSandTime(deltaTime));

        simonCurrentPositionX = sprites["simon"]->getPosition().x;
        // =================================

        Update(window, deltaTime);

        // ESTO NO DEBERIA IR AQUI =========

        simonNewPositionX = sprites["simon"]->getPosition().x;
        if (abs(simonNewPositionX - simonCurrentPositionX) > 0.01f)
        {
            camera.startVertex += sf::Vector2f{simonNewPositionX - simonCurrentPositionX, 0.f};
            std::cout << "UpdateView: " << simonNewPositionX - simonCurrentPositionX << std::endl;
        }
        window.setView(camera.GetView(window.getSize()));
        simonCurrentPositionX = simonNewPositionX;
        // =================================

        Render(window, text);
    }

    return 0;
}