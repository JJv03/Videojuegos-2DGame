#include <iostream>
#include "game.h"
#include "resources.h"

// Constantes para la física
constexpr float GRAVITY{980.0f};       // aceleración en píxeles/segundo²
constexpr float JUMP_FORCE{350.0f};    // velocidad inicial del salto (píxeles/segundo)
constexpr float MOVEMENT_SPEED{50.0f}; // velocidad de movimiento base
constexpr float ATACK_TIME{0.5f};      // tiempo entre ataques

// HABRA QUE REORGANIZARLOS
float verticalSpeed{0.0f};
bool isOnGround{true};
bool enemigoVivo{true};
bool isAtacking = false;

// LOS SPRITES DEBERAN IR ASOCIADOS A SU PROPIA CLASE NO EN UNA DICCIONARIO DE RECURSOS
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

// HAY QUE TENER EN CUENTA QUE TODO LO RELACIONADO CON SIMON VA EN LA CLASE PLAYER, POR EJ LA EXCRACCIÓN DEL SPRITE DEBERIA HACERSE DESDE AHI EN VEZ DEL DICCIONARIO
// TAMBIEN DE OTRAS ENTIDADES
// LAS MODIFICACIONES SE REALIZAN DE LAS CLASES NO DEL DICCIONARIO

void Update(sf::RenderWindow &window, float deltaTime)
{

    // ESTOS SERÁN ATRIBUTOS PROPIOS DE LA CLASE SIMON NO HARA FALTA PONERLOS AQUI Y ESTAR PASANDOLOS TODO EL RATO
    bool goingUp = false;
    bool goingLeft = false;
    bool goingRight = false;

    float timeActualAtack = 0.0f;
    float timeActualEnemyRespawn = 0.0f;

    // DETECCIÓN DE INPUTS
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }
        else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            switch (keyPressed->scancode)
            {
            case sf::Keyboard::Scancode::Escape:
                window.close();
                break;
            case sf::Keyboard::Scancode::Z:
                isAtacking = true;
                std::cout << "Ataque activado" << std::endl;
                break;
            case sf::Keyboard::Scancode::Up:
                goingUp = true;
                if (isOnGround)
                {
                    verticalSpeed = -JUMP_FORCE;
                    isOnGround = false;
                }
                break;
            case sf::Keyboard::Scancode::Down:
                verticalSpeed = JUMP_FORCE * 0.5f;
                break;
            case sf::Keyboard::Scancode::Left:
                goingLeft = true;
                goingRight = false;
                Resources::sprites["simon"].setScale({1.f, 1.f});
                break;
            case sf::Keyboard::Scancode::Right:
                goingRight = true;
                goingLeft = false;
                Resources::sprites["simon"].setScale({-1.f, 1.f});
                break;
            default:
                break;
            }
        }
        else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            switch (keyReleased->scancode)
            {
            case sf::Keyboard::Scancode::Up:
                goingUp = false;
                break;
            case sf::Keyboard::Scancode::Left:
                goingLeft = false;
                break;
            case sf::Keyboard::Scancode::Right:
                goingRight = false;
                break;
            default:
                break;
            }
        }
    }

    // REALIZACIÓN DE LAS ACTUALIZACIONES
    updateMovement(deltaTime, goingUp, goingLeft, goingRight);
    updateEnemyRespawn(deltaTime, timeActualEnemyRespawn);
    updateSimonAtaque(isAtacking, deltaTime, timeActualAtack);
    CheckAllCollisions(isAtacking);
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

// EL RENDERIZADO SERA DE LOS SPRITES DE LAS CLASES, NO DE LOS DEL DICCIONARIO
void Render(sf::RenderWindow &window, const sf::Text &text)
{
    window.clear(sf::Color::Black);
    window.draw(Resources::sprites["bgEntrada"]);
    window.draw(Resources::rectangles["floor"]);
    window.draw(FloatRectToRectShape(Resources::rectangles["floor"].getGlobalBounds()));
    window.draw(Resources::rectangles["wallUp"]);
    window.draw(FloatRectToRectShape(Resources::rectangles["wallUp"].getGlobalBounds()));
    window.draw(Resources::rectangles["wallDown"]);
    window.draw(FloatRectToRectShape(Resources::rectangles["wallDown"].getGlobalBounds()));
    window.draw(Resources::sprites["simon"]);
    window.draw(FloatRectToRectShape(Resources::sprites["simon"].getGlobalBounds()));
    if (isAtacking)
    {
        window.draw(Resources::rectangles["vapireKiller"]);
    }
    if (enemigoVivo)
    {
        window.draw(Resources::sprites["enemy"]);
        window.draw(FloatRectToRectShape(Resources::sprites["enemy"].getGlobalBounds()));
    }
    window.draw(text);
    window.display();
}

// ========================================
// ========================================
// ESTAS FUNCIONES HAY QUE REDISTRIBUIRLAS
// ========================================
// ========================================

void updateMovement(const float deltaTime, const bool goingUp, const bool goingLeft, const bool goingRight)
{

    // Movimiento vertical con gravedad
    verticalSpeed += GRAVITY * deltaTime;
    Resources::sprites["simon"].move({0.f, verticalSpeed * deltaTime});

    if (goingUp)
    {
        if (goingRight)
        {
            // gSimonSprite->move({1.f * deltaTime * gMovementSpeed, -1.5f * deltaTime * gMovementSpeed});
            Resources::sprites["simon"].move({1.f * deltaTime * MOVEMENT_SPEED, 0.f});
        }
        else if (goingLeft)
        {
            // gSimonSprite->move({-1.f * deltaTime * gMovementSpeed, -1.5f * deltaTime * gMovementSpeed});
            Resources::sprites["simon"].move({-1.f * deltaTime * MOVEMENT_SPEED, 0.f});
        }
    }
    else if (goingLeft)
    {
        Resources::sprites["simon"].move({-1.5f * deltaTime * MOVEMENT_SPEED, 0.f});
    }
    else if (goingRight)
    {
        Resources::sprites["simon"].move({1.5f * deltaTime * MOVEMENT_SPEED, 0.f});
    }
}

void updateEnemyRespawn(float deltaTime, float &timeActualEnemyRespawn)
{
    if (!enemigoVivo)
    {
        if (timeActualEnemyRespawn >= 2.0)
        {
            enemigoVivo = true;
            timeActualEnemyRespawn = 0.0f;
        }
        else
        {
            timeActualEnemyRespawn += deltaTime;
        }
    }
}

void updateSimonAtaque(bool &ataque, float deltaTime, float &timeActualAtack)
{
    if (ataque)
    {
        if (timeActualAtack >= ATACK_TIME)
        {
            // gSimonSprite->setTextureRect(sf::IntRect({1, 53}, {16, 32}));
            timeActualAtack = 0.0f;
            ataque = false;
        }
        else
        {
            timeActualAtack += deltaTime;
        }
    }
    sf::Vector2f position = Resources::sprites["simon"].getPosition();
    position.x += Resources::sprites["simon"].getLocalBounds().size.x * 0.5f;
    position.y -= Resources::sprites["simon"].getLocalBounds().size.y * 0.5f;
    Resources::rectangles["vapireKiller"].setPosition(position);
}

void CheckCollisions(sf::FloatRect simonBounds, sf::FloatRect objectBounds, const bool debug = false)
{
    // Si esto da true, es porque la hitbox de Simon ha penetrado el objeto <objectBounds>
    if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(objectBounds))
    {
        float overlapX{intersection->size.x};
        float overlapY{intersection->size.y};

        if (overlapX < overlapY) // Colisión horizontal
        {
            if ((simonBounds.position.x + simonBounds.size.x * 0.5f) < (objectBounds.position.x + objectBounds.size.x * 0.5f))
            {
                if (debug)
                    std::cout << "Colision con borde lateral izquierdo de objeto." << std::endl;
                Resources::sprites["simon"].move({-overlapX, 0.f});
            }
            else
            {
                if (debug)
                    std::cout << "Colision con borde lateral derecho de objeto." << std::endl;
                Resources::sprites["simon"].move({overlapX, 0.f});
            }
        }
        else // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                if (debug)
                    std::cout << "Colision con borde superior de objeto." << std::endl;
                Resources::sprites["simon"].move({0.f, -overlapY});
                verticalSpeed = 0.0f; // Simon deja de caere
                isOnGround = true;    // Indicamos que Simon está en el suelo
            }
            else
            {
                if (debug)
                    std::cout << "Colision con borde inferior de objeto." << std::endl;
                Resources::sprites["simon"].move({0.f, overlapY});
                verticalSpeed = 0.0f; // Simon pasará a estar cayendo
            }
        }
    }
}

void CheckVampireKillerCollision(const bool ataque)
{
    if (enemigoVivo && ataque)
    {
        sf::FloatRect enemyBounds = Resources::sprites["simon"].getGlobalBounds();
        sf::FloatRect vkBounds = Resources::rectangles["vapireKiller"].getGlobalBounds();

        if (const std::optional<sf::FloatRect> intersection = vkBounds.findIntersection(enemyBounds))
        {
            enemigoVivo = false;
        }
    }
}

void CheckAllCollisions(const bool ataque, const bool debug = false)
{
    sf::FloatRect simonBounds = Resources::sprites["simon"].getGlobalBounds();
    sf::FloatRect floorBounds = Resources::rectangles["floor"].getGlobalBounds();
    sf::FloatRect wallUpBounds = Resources::rectangles["wallUp"].getGlobalBounds();
    sf::FloatRect wallDownBounds = Resources::rectangles["wallDown"].getGlobalBounds();

    CheckCollisions(simonBounds, floorBounds, debug);
    CheckCollisions(simonBounds, wallUpBounds, debug);
    CheckCollisions(simonBounds, wallDownBounds, debug);
    CheckVampireKillerCollision(ataque);
}
