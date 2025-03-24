#include "createEnemies.h"

Zombie createZombie(const sf::Vector2f &position)
{
    const sf::IntRect ZOMBIE_SPRITE_REGION = {{1, 28}, {16, 32}};

    const float HITBOX_WIDTH = 12.f;
    const float HITBOX_HEIGHT = 30.f;

    // Crear la textura
    sf::Image zombieImage;
    if (!zombieImage.loadFromFile("./assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del zombie" << std::endl;
        throw std::runtime_error("Error cargando la imagen del zombie");
    }
    zombieImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *zombieTexture = new sf::Texture();
    if (!zombieTexture->loadFromImage(zombieImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete zombieTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto zombieSprite = std::make_shared<sf::Sprite>(*zombieTexture);
    zombieSprite->setTextureRect(ZOMBIE_SPRITE_REGION);
    zombieSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = zombieSprite->getLocalBounds();
    zombieSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    // Crear y retornar el zombie
    return Zombie(zombieSprite, hitboxes);
}

Leopard createLeopard(const sf::Vector2f &position)
{
    const sf::IntRect LEOPARD_SPRITE_REGION = {{56, 11}, {24, 16}};

    const float HITBOX_WIDTH = 22.f;
    const float HITBOX_HEIGHT = 16.f;

    // Crear la textura
    sf::Image leopardImage;
    if (!leopardImage.loadFromFile("./assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del leopard" << std::endl;
        throw std::runtime_error("Error cargando la imagen del leopard");
    }
    leopardImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *leopardTexture = new sf::Texture();
    if (!leopardTexture->loadFromImage(leopardImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete leopardTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto leopardSprite = std::make_shared<sf::Sprite>(*leopardTexture);
    leopardSprite->setTextureRect(LEOPARD_SPRITE_REGION);
    leopardSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = leopardSprite->getLocalBounds();
    leopardSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    // Crear y retornar el zombie
    return Leopard(leopardSprite, hitboxes);
}

Bat createBatSpawner(const sf::Vector2f &position, const sf::Vector2f &zoneSize)
{
    const sf::IntRect BAT_SPRITE_REGION = {{184, 11}, {16, 16}};

    const float HITBOX_WIDTH = 15.f;
    const float HITBOX_HEIGHT = 15.f;

    // Crear la textura
    sf::Image batImage;
    if (!batImage.loadFromFile("./assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del bat" << std::endl;
        throw std::runtime_error("Error cargando la imagen del bat");
    }
    batImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    sf::Texture *batTexture = new sf::Texture();
    if (!batTexture->loadFromImage(batImage))
    {
        std::cerr << "Error cargando la textura desde la imagen" << std::endl;
        delete batTexture;
        throw std::runtime_error("Error cargando la textura desde la imagen");
    }

    // Crear el sprite
    auto batSprite = std::make_shared<sf::Sprite>(*batTexture);
    batSprite->setTextureRect(BAT_SPRITE_REGION);
    batSprite->setPosition(position);

    // Ajustar el origen al centro inferior del sprite
    sf::FloatRect bounds = batSprite->getLocalBounds();
    batSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    // Crear las hitboxes
    std::vector<sf::FloatRect> hitboxes = {
        sf::FloatRect(
            {position.x - (HITBOX_WIDTH / 2.f), position.y - HITBOX_HEIGHT},
            {HITBOX_WIDTH, HITBOX_HEIGHT}),
    };

    // Crear y retornar el bat
    return Bat(batSprite, hitboxes, position, zoneSize);
}