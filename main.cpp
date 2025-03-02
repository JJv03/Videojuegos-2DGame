#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include "game.h"
#include "camera.h"
#include "resources.h"


// Global configuration variables
constexpr int scale { 1 };
constexpr int gWindowWidth { 768 * scale };
constexpr int gWindowHeight { 250 * scale };


std::string formatFPSandTime(float deltaTime)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << (1.f / deltaTime);
    std::string fps = oss.str();

    // Clean the stream
    oss.str("");
    oss.clear();

    oss << std::fixed << std::setprecision(2) << (deltaTime * 1000);
    std::string ms = oss.str();
    return fps + " FPS\n" + ms + " ms";
}

int main()
{
    std::string windowName { "Castlevania NES" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), windowName, sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    if (!Init())
    {
        std::cerr << "Error en la inicialización" << std::endl;
        return -1;
    }

    sf::Font font;
    if (!font.openFromFile("../assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cerr << "Error en la carga de la fuente de texto" << std::endl;
        return -1;
    }
    
    sf::Text text(font);
    text.setString("60 FPS\n16.67 ms");
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    
    // Game loop
    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();
        text.setString(formatFPSandTime(deltaTime));

        Update(window, deltaTime);

        Render(window, text);
    }

    return 0;
}
