#include "Player.hpp"

bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth{ 800 * escala};
constexpr int gWindowHeight{ 250 * escala};

int main(){
    printf("Hola\n");

    Player player;

    std::string nombreVentana { "Castlevania: En busca de la Eduardomena Pose" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    // run the program as long as the window is open
    while (window.isOpen()){
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            } else {
                player.handleInput(*event);
            }
        }
        window.clear(sf::Color::Black);
        window.display();
    }
    
}