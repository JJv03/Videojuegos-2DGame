#include "demo.h"
#include "resources.h"
#include "map.h"
#include <iostream>
#include "zombie.h"

Map map;
Camera camera(320.0f);

const float movementSpeed = 75.0f;

// Lista de enemigos
std::vector<std::unique_ptr<Zombie>> enemies;
std::unordered_map<std::string, sf::Texture> gTextures;

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window)
{
    // En este caso primero se extrae en formato imagen para realizar las conversiones primero, después se almacena en la clase de recursos
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../assets/sprites/player/simonBelmont.png"))
        exit(-1);

    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["simon"].loadFromImage(simonImage))
        exit(-1);

    // Creación textura de ejemplo cuadrado
    sf::Image squareImage;
    if (!squareImage.loadFromFile("../assets/others/square.png"))
        exit(-1);

    squareImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    if (!Resources::textures["square"].loadFromImage(squareImage))
        exit(-1);

    if (!Resources::textures["background"].loadFromFile("../assets/maps/level1Entrance.png"))
        exit(-1);

    // Incializacion del grid del mapa
    sf::Image image;
    if (!image.loadFromFile("../assets/others/demoMap.png"))
        exit(-1);

    map.CreateFromImage(image);

    // Enemigo ----------------------------------------------------------------------------
    sf::Image enemyImage;
    if (!enemyImage.loadFromFile("../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del enemigo" << std::endl;
        exit(-1);
    }
    enemyImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    gTextures["enemy"] = sf::Texture(enemyImage, false);

    sf::Sprite enemySprite(gTextures["enemy"]);
    enemySprite.setTextureRect(sf::IntRect({1, 28}, {16, 32}));
    enemySprite.setPosition({150.f, 171.f});
    sf::FloatRect enemyHitbox = enemySprite.getGlobalBounds();
    // Ajusta el origen de las transformaciones al centro inferior
    enemySprite.setOrigin({enemyHitbox.size.x / 2.f, enemyHitbox.size.y});

    // Agregar un zombie al vector
    enemies.push_back(std::make_unique<Zombie>(enemyHitbox, enemySprite, 50.0f));

    // Posición inicial de la camara
    camera.position = sf::Vector2f(150.0f, 87.5f);
}

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        camera.position.x += movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        camera.position.x -= movementSpeed * deltaTime;
    }

    // Esto debera ser con el jugador pero solo tengo camara

    for (auto &enemy : enemies)
    {
        enemy->checkActivation(camera.position.x, 1200); // gWindowWidth hardcodeado
        enemy->update(deltaTime, camera.position.x);
    }
}

sf::RectangleShape FloatRectToRectShape(const sf::FloatRect &floatRect)
{
    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineColor(sf::Color::Red);
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

// Renderiza los elementos en la ventana
void Render(Renderer &renderer, sf::RenderWindow &window)
{
    map.DrawBackground(renderer);
    map.Draw(renderer);
    // renderer.DrawCut(Resources::textures["simon"], sf::Vector2f(), sf::Vector2f(7.5, 5.5), sf::IntRect({1, 21}, {16, 32}));

    for (const auto &enemy : enemies)
    {
        if (enemy->getActiveState() && enemy->sprite)
        {
            window.draw(*enemy->sprite);
            window.draw(FloatRectToRectShape(enemy->sprite->getGlobalBounds()));
        }
    }
}
