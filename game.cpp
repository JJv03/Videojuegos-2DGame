#include <iostream>
#include "game.h"
#include "resources.h"

bool Init()
{
    // Fondo ----------------------------------------------------------------------------
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("../assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura de fondo" << std::endl;
        return false;
    }
    Resources::textures["bgEntrada"] = bgTexture;

    sf::Sprite bgSprite(Resources::textures["bgEntrada"]);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));

    Resources::sprites["bgEntrada"] = bgSprite;

    // Suelo ----------------------------------------------------------------------------
    sf::RectangleShape floor;
    floor.setSize(sf::Vector2f(static_cast<float>(768), 50.f)); // Demomento gwindowwidht hardcodeado luego no se usara aqui seguramente
    floor.setFillColor(sf::Color(139, 69, 19));
    floor.setPosition({0.f, 171.f});

    Resources::rectangles["floor"] = floor;

    // Pared Arriba ----------------------------------------------------------------------------
    sf::RectangleShape wallUp;
    wallUp.setSize(sf::Vector2f(50.f, 20.f));
    wallUp.setFillColor(sf::Color(139, 69, 19));
    wallUp.setPosition({518.f, 75.f});

    Resources::rectangles["wallUp"] = wallUp;

    // Pared Abajo ----------------------------------------------------------------------------
    sf::RectangleShape wallDown;
    wallDown.setSize(sf::Vector2f(50.f, 20.f));
    wallDown.setFillColor(sf::Color(139, 69, 19));
    wallDown.setPosition({465.f, 125.f});

    Resources::rectangles["wallDown"] = wallDown;

    // Simon ----------------------------------------------------------------------------
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return false;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));
    sf::Texture simonTexture;
    simonTexture.loadFromImage(simonImage);

    Resources::textures["simon"] = simonTexture;

    sf::Sprite simonSprite(Resources::textures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition({100.f, 171.f});
    sf::FloatRect bounds = simonSprite.getLocalBounds();
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});

    Resources::sprites["simon"] = simonSprite;

    // Vampire Killer ----------------------------------------------------------------------------
    sf::RectangleShape vampireKiller;
    vampireKiller.setSize(sf::Vector2f(30.f, 5.f));
    vampireKiller.setFillColor(sf::Color::Red);

    Resources::rectangles["vapireKiller"] = vampireKiller;

    // Enemigo ----------------------------------------------------------------------------
    sf::Image enemyImage;
    if (!enemyImage.loadFromFile("../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del enemigo" << std::endl;
        return false;
    }
    enemyImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));
    sf::Texture enemyTexture;
    enemyTexture.loadFromImage(enemyImage);

    Resources::textures["enemy"] = enemyTexture;

    sf::Sprite enemySprite(Resources::textures["enemy"]);
    enemySprite.setTextureRect(sf::IntRect({1, 28}, {16, 32}));
    enemySprite.setPosition({345.f, 171.f});
    sf::FloatRect enemyBounds = enemySprite.getLocalBounds();
    enemySprite.setOrigin({enemyBounds.size.x / 2.f, enemyBounds.size.y});

    Resources::sprites["enemy"] = enemySprite;

    return true;
}
