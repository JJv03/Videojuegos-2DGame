#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "camera.h"
#include "entity.h"
#include "animationManager.hpp"
#include "Player.hpp"
#include "PlayerState.hpp"

// Clase Player
Player player;
// Variables globales de configuración
bool gEnMovimiento { false };
// Global variable to keep track of attack time
bool facing { true }; // true = derecha, false = izquierda
constexpr int escala { 1 };
constexpr int gWindowWidth { 768 * escala };
constexpr int gWindowHeight { 250 * escala };
constexpr float gMovementSpeedSlow { 10.0f };

// Enemigos
constexpr float tiempoEnemigoRespawn { 2.0f }; //Demo colisiones
bool Simonatacado { false };
bool Simoninvulnerable { false };
float atacadoCooldown { 0.0f }; // Tracks the elapsed time since Simon was last attacked
constexpr float atacadoCooldownDuration { 5.0f }; // 5 seconds cooldown

// Cámara
Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

Entity gEnemy;
bool enemigoVivo { true };

// Contenedores para sprites y texturas
std::vector<sf::Sprite> gSprites;
std::unordered_map<std::string, sf::Texture> gTextures;

// Puntero global para acceder al sprite de Simon
sf::Sprite* gSimonSprite { nullptr };
sf::Sprite* gWhipSprite { nullptr };
bool isOnGround { true };       // indica si Simon está en el suelo
float verticalSpeed { 0.0f };   // velocidad vertical actual

// Definir el suelo y la pared para colisiones
sf::RectangleShape gFloor;
sf::RectangleShape gWall;

AnimationManager* gAnimationManager { nullptr };
AnimationManager* gWhipAnimationManager { nullptr };

// DEPURACION
animationID currentAnimation;

//ENEMIGOS DEMO COLISIONES

sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect)
{
    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineColor(sf::Color::Red);
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

sf::RectangleShape gVampireKiller;

void updateEnemyRespawn(float deltaTime, float& tiempoEnemigoRespawnActual)
{
    if (!enemigoVivo)
    {
        if (tiempoEnemigoRespawnActual >= tiempoEnemigoRespawn)
        {
            enemigoVivo = true;
            tiempoEnemigoRespawnActual = 0.0f;
            std::cout << "Enemigo respawneado" << std::endl;
        }
        else
        {
            tiempoEnemigoRespawnActual += deltaTime;
        }
    }
}

void CheckVampireKillerCollision(const bool ataque)
{
    std::cout << "Verificando colisión de Vampire Killer..." << std::endl;
    if (gEnemy.sprite && enemigoVivo && ataque)
    {
        sf::FloatRect enemyBounds = gEnemy.sprite->getGlobalBounds();
        sf::FloatRect vkBounds = gWhipSprite->getGlobalBounds();

        if (const std::optional<sf::FloatRect> intersection = vkBounds.findIntersection(enemyBounds))
        {
            if (gWhipAnimationManager->getCurrentFrameIndex() == 3)
            {
                std::cout << "Colisión con enemigo" << std::endl;
                enemigoVivo = false;
            }
        }
    }
}

void ChecKSimonAttackedCollision()
{
    std::cout << "Verificando colisión de Simon atacado..." << std::endl;
    if (gEnemy.sprite && enemigoVivo)
    {
        sf::FloatRect enemyBounds = gEnemy.sprite->getGlobalBounds();
        sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();

        if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(enemyBounds))
        {
            if (!Simonatacado)
            {
                std::cout << "HE SIDO ATACADO" << std::endl;
                Simonatacado = true; // Disable atacado
                Simoninvulnerable = true; // Enable invulnerability
                atacadoCooldown = 0.0f; // Reset the cooldown timer
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------
// Función de colisiones
void CheckCollisions(sf::FloatRect simonBounds, sf::FloatRect objectBounds)
{
    std::cout << "Verificando colisiones..." << std::endl;
    if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(objectBounds))
    {
        float overlapX { intersection->size.x };
        float overlapY { intersection->size.y };

        if (overlapX < overlapY)    // Colisión horizontal
        {
            if ((simonBounds.position.x + simonBounds.size.x * 0.5f) < (objectBounds.position.x + objectBounds.size.x * 0.5f))
            {
                gSimonSprite->move({-overlapX, 0.f});
            }
            else
            {
                gSimonSprite->move({overlapX, 0.f});
            }
        }
        else    // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                gSimonSprite->move({0.f, -overlapY});
                verticalSpeed = 0.0f;   // Simon deja de caere
                isOnGround = true;      // Indicamos que Simon está en el suelo
            }
            else
            {
                gSimonSprite->move({0.f, overlapY});
                verticalSpeed = 0.0f;   // Simon pasará a estar cayendo
            }
        }
    }
}

void CheckAllCollisions(const bool ataque)
{
    std::cout << "Global bounds" << std::endl;
    sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();
    std::cout << "Simon bounds" << std::endl;
    sf::FloatRect floorBounds = gFloor.getGlobalBounds();
    sf::FloatRect wallBounds = gWall.getGlobalBounds();

    std::cout << "Primeras colisiones" << std::endl;
    CheckCollisions(simonBounds, floorBounds);
    CheckCollisions(simonBounds, wallBounds);
    std::cout << "Mitad colisiones" << std::endl;
    ChecKSimonAttackedCollision();
    CheckVampireKillerCollision(ataque);
}

bool init()
{
    std::cout << "Inicializando..." << std::endl;

    // Mundo
    if (!gTextures["bgEntrada"].loadFromFile("../../assets/maps/level1Entrance.png", false))
    {
        std::cerr << "Error cargando la textura de fondo" << std::endl;
        return false;
    }
    sf::Sprite bgSprite(gTextures["bgEntrada"]);
    bgSprite.setTextureRect(sf::IntRect({1, 11}, {768, 192}));
    gSprites.push_back(bgSprite);

    // Simon
    sf::Image simonImage;
    if (!simonImage.loadFromFile("../../assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return false;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    simonImage.createMaskFromColor(sf::Color(0, 128, 0)); // color key
    gTextures["simon"] = sf::Texture(simonImage, false);

    player.sprite = new sf::Sprite(gTextures["simon"]);
    player.sprite->setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    player.sprite->setPosition({245.f, 171.f});
    
    sf::FloatRect bounds = player.sprite->getLocalBounds();
    player.sprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(*player.sprite);

    // Asignar gSimonSprite al sprite del jugador
    gSimonSprite = player.sprite;
    if (gSimonSprite)
    {
        std::cout << "gSimonSprite inicializado correctamente" << std::endl;
    }
    else
    {
        std::cerr << "Error: gSimonSprite no se pudo inicializar" << std::endl;
        return false;
    }
    
    player.initAnimations();
    // Inicializar AnimationManager
    gAnimationManager = new AnimationManager(*player.sprite);

    std::vector<AnimationManager::Frame> whipLvl1Frames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(17, 135), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(50, 135), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(67, 135), sf::Vector2(26, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(112, 134), sf::Vector2(28, 17)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(123, 25), sf::Vector2(16, 31)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(123, 25), sf::Vector2(16, 31)), 0.1f},
    };

    gAnimationManager->playAnimation(idleSimon);
    currentAnimation = idleSimon;

    // Whip
    sf::Image whipImage;
    if (!whipImage.loadFromFile("../../assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
        return false;
    }
    whipImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    whipImage.createMaskFromColor(sf::Color(0, 128, 0)); // color key
    gTextures["whip"] = sf::Texture(whipImage, false);

    sf::Sprite whipSprite(gTextures["whip"]);
    whipSprite.setTextureRect(sf::IntRect({17, 135}, {16, 32}));
    whipSprite.setPosition({245.f, 171.f});
    
    whipSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    
    gWhipSprite =  new sf::Sprite(whipSprite);
    
    // Inicializar AnimationManager
    gWhipAnimationManager = new AnimationManager(*gWhipSprite);

    if (!gWhipAnimationManager) {
        std::cerr << "Error: Failed to initialize Whip AnimationManager!" << std::endl;
        return false;
    }
    gWhipAnimationManager->addAnimation(whipLvl1StandingJumping, whipLvl1Frames, false);
    
    // Suelo
    gFloor.setSize(sf::Vector2f(static_cast<float>(gWindowWidth), 50.f)); // 50 píxeles de alto (puedes ajustar este valor)
    gFloor.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gFloor.setPosition({0.f, 171.f}); // Posicionado a lo largo del ancho, justo a los pies de Simon

    // Pared
    gWall.setSize(sf::Vector2f(50.f, 20.f));
    gWall.setFillColor(sf::Color(139, 69, 19)); // Color marrón
    gWall.setPosition({518.f, 75.f});

    // Enemigo ----------------------------------------------------------------------------
    sf::Image enemyImage;
    if (!enemyImage.loadFromFile("../../assets/sprites/enemies/enemies.png"))
    {
        std::cerr << "Error cargando la imagen del enemigo" << std::endl;
        return false;
    }
    enemyImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    gTextures["enemy"] = sf::Texture(enemyImage, false);

    gEnemy.sprite = new sf::Sprite(gTextures["enemy"]);
    gEnemy.sprite->setTextureRect(sf::IntRect({1, 28}, {16, 32}));
    gEnemy.sprite->setPosition({345.f, 171.f});
    sf::FloatRect bounds2 = gEnemy.sprite->getLocalBounds();
    // Ajusta el origen de las transformaciones al centro inferior
    gEnemy.sprite->setOrigin({bounds2.size.x / 2.f, bounds2.size.y});

    std::cout << "Inicialización completa" << std::endl;
    return true;
}

int main()
{
    std::cout << "Iniciando el juego..." << std::endl;
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castlevania", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    if (!init())
    {
        std::cerr << "Error en la inicialización" << std::endl;
        return -1;
    }
    
    float tiempoEnemigoRespawnActual { 0.0f };
    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();
        std::cout << "Delta time: " << deltaTime << std::endl;

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::cout << "Cerrando ventana..." << std::endl;
                window.close();
            }
            else
            {
                std::cout << "Manejando entrada del jugador..." << std::endl;
                player.handleInput(*event);
            }
        }

        // Verificar colisiones
        if (player.sprite->getPosition().x < 680)
        {
            std::cout << "Actualizando respawn de enemigo..." << std::endl;
            updateEnemyRespawn(deltaTime, tiempoEnemigoRespawnActual);
            std::cout << "Verificando todas las colisiones..." << std::endl;
            CheckAllCollisions(player.isAttacking);

            std::cout << "Actualizando jugador..." << std::endl;
            player.update(deltaTime);

            if (Simonatacado) {
                atacadoCooldown += deltaTime; // Increment the cooldown timer
                std::cout << "Cooldown: " << atacadoCooldown << std::endl;
                if (atacadoCooldown >= atacadoCooldownDuration) {
                    Simonatacado = false; // Re-enable atacado after the cooldown
                }
            }

            window.clear();
            std::cout << "Dibujando sprites..." << std::endl;
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
            
            if (enemigoVivo)
            {
                std::cout << "Dibujando enemigo..." << std::endl;
                window.draw(*gEnemy.sprite);
                window.draw(FloatRectToRectShape(gEnemy.sprite->getGlobalBounds()));
            }
        }
        else if(player.sprite->getPosition().x < 730)
        {
            std::cout << "Moviendo jugador lentamente..." << std::endl;
            player.sprite->move({1.5f * deltaTime * gMovementSpeedSlow, 0.f});
            player.sprite->setScale({-1.f, 1.f});
            if (!gAnimationManager->isPlaying(walkSlowSimon) || gAnimationManager->isAnimationFinished())
            {
                gAnimationManager->playAnimation(walkSlowSimon);
            }
            
            gAnimationManager->update(deltaTime * gMovementSpeedSlow);
            window.clear();
            std::cout << "Dibujando sprites..." << std::endl;
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
        }
        else
        {
            std::cout << "Cargando nueva textura de fondo..." << std::endl;
            if (!gTextures["bgEntrada"].loadFromFile("../../assets/maps/level1Entrance.png", false))
            {
                std::cerr << "Error cargando la textura de fondo" << std::endl;
                return false;
            }
            sf::Sprite bgSprite(gTextures["bgEntrada"]);
            bgSprite.setTextureRect(sf::IntRect({774, 11}, {1542, 192}));
            gSprites.push_back(bgSprite);
            player.sprite->setPosition({10.f, 171.f});
            window.clear();
            std::cout << "Dibujando sprites..." << std::endl;
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
        }
        window.draw(gFloor);
        window.draw(FloatRectToRectShape(gFloor.getGlobalBounds()));
        window.draw(gWall);
        window.draw(*player.sprite);
        if (player.isAttacking) {
            std::cout << "Dibujando látigo..." << std::endl;
            window.draw(*gWhipSprite);
            window.draw(FloatRectToRectShape(gWhipSprite->getGlobalBounds()));
        }
        window.display();
    }

    std::cout << "Liberando recursos..." << std::endl;
    delete gAnimationManager;
    delete gWhipAnimationManager;

    std::cout << "Juego terminado." << std::endl;
    return 0;
}