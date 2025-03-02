#include "Player.hpp"
#include "camera.h"
#include <iostream>

using namespace std;

bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth{ 800 * escala};
constexpr int gWindowHeight{ 250 * escala};

sf::RectangleShape gFloor;


Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

// Contenedores para sprites y texturas
std::vector<sf::Sprite> gSprites;
std::unordered_map<std::string, sf::Texture> gTextures;

Player player;

bool init()
{
    // Fondo ----------------------------------------------------------------------------
    if (!gTextures["bgEntrada"].loadFromFile("../../assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura de fondo" << std::endl;
        return false;
    }
    sf::Sprite bgSprite(gTextures["bgEntrada"]);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));
    gSprites.push_back(bgSprite);

    // Suelo ----------------------------------------------------------------------------

    gFloor.setSize(sf::Vector2f(static_cast<float>(gWindowWidth), 50.f)); // 50 píxeles de alto (puedes ajustar este valor)
    gFloor.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gFloor.setPosition({0.f, 171.f}); // Posicionado a lo largo del ancho, justo a los pies de Simon

    // Simon ----------------------------------------------------------------------------

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
    simonSprite.setPosition({245.f, 171.f});
    sf::FloatRect bounds = simonSprite.getLocalBounds();
    
    // Ajusta el origen de las transformaciones al centro inferior
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    player.sprite = &gSprites.back();

    return true;
}

int main(){
    std::string nombreVentana { "Castlevania: En busca de la Eduardomena Pose" };
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), nombreVentana, sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    init();

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

        window.setView(camera.GetView(window.getSize()));

        player.update(deltaTime);
        window.clear(sf::Color::Black);
        for (const auto& sprite : gSprites)
        {   
            window.draw(sprite);
        }
        window.draw(gFloor);
        player.draw(window);
        window.draw(*player.sprite);
        window.display();
    }
}