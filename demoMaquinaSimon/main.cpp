#include "Player.hpp"

bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth{ 800 * escala};
constexpr int gWindowHeight{ 250 * escala};

int main(){
    Player player;

    std::string nombreVentana { "Castlevania: En busca de la Eduardomena Pose" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    while (window.isOpen()){

        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            } else {
                player.handleInput(*event);
            }
        }

        player.update(deltaTime);

        window.clear(sf::Color::Black);
        player.draw(window);
        window.display();
    }
    
    
}