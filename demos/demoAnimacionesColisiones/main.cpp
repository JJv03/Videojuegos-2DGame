#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "camera.h"
#include "entity.h"
#include "animationManager.hpp"


// Variables globales de configuración
bool gEnMovimiento { false };
// Global variable to keep track of attack time
bool facing { true }; // true = derecha, false = izquierda
constexpr int escala { 1 };
constexpr int gWindowWidth { 768 * escala };
constexpr int gWindowHeight { 250 * escala };
constexpr float gMovementSpeed { 50.0f };
constexpr float gMovementSpeedSlow { 10.0f };

// Enemigos
constexpr float tiempoEnemigoRespawn { 2.0f }; //Demo colisiones
bool Simonatacado { false };
bool Simoninvulnerable { false };
float atacadoCooldown { 0.0f }; // Tracks the elapsed time since Simon was last attacked
constexpr float atacadoCooldownDuration { 5.0f }; // 5 seconds cooldown


// Constantes para la física
constexpr float GRAVITY { 980.0f };       // aceleración en píxeles/segundo²
constexpr float JUMP_FORCE { 350.0f };    // velocidad inicial del salto (píxeles/segundo)

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
        }
        else
        {
            tiempoEnemigoRespawnActual += deltaTime;
        }
    }
}

void CheckVampireKillerCollision(const bool ataque)
{
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

void ChecKSimonAttackedCollision(){
    if (gEnemy.sprite && enemigoVivo)
    {
        sf::FloatRect enemyBounds = gEnemy.sprite->getGlobalBounds();
        sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();

        if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(enemyBounds))
        {
            if (!Simonatacado)
            {
                //gAnimationManager->playAnimation(hurtSimon);
                
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
    // Si esto da true, es porque la hitbox de Simon ha penetrado el objeto <objectBounds>
    if (const std::optional<sf::FloatRect> intersection = simonBounds.findIntersection(objectBounds))
    {
        float overlapX { intersection->size.x };
        float overlapY { intersection->size.y };

        if (overlapX < overlapY)    // Colisión horizontal
        {
            if ((simonBounds.position.x + simonBounds.size.x * 0.5f) < (objectBounds.position.x + objectBounds.size.x * 0.5f))
            {
                //std::cout << "Colision con borde lateral izquierdo de objeto." << std::endl;
                gSimonSprite->move({-overlapX, 0.f});
            }
            else
            {
                //std::cout << "Colision con borde lateral derecho de objeto." << std::endl;
                gSimonSprite->move({overlapX, 0.f});
            }
        }
        else    // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                //std::cout << "Colision con borde superior de objeto." << std::endl;
                gSimonSprite->move({0.f, -overlapY});
                verticalSpeed = 0.0f;   // Simon deja de caere
                isOnGround = true;      // Indicamos que Simon está en el suelo
            }
            else
            {
                //std::cout << "Colision con borde inferior de objeto." << std::endl;
                gSimonSprite->move({0.f, overlapY});
                verticalSpeed = 0.0f;   // Simon pasará a estar cayendo
            }
        }
    }
}


void CheckAllCollisions(const bool ataque)
{
    sf::FloatRect simonBounds = gSimonSprite->getGlobalBounds();
    sf::FloatRect floorBounds = gFloor.getGlobalBounds();
    sf::FloatRect wallBounds = gWall.getGlobalBounds();

    CheckCollisions(simonBounds, floorBounds);
    CheckCollisions(simonBounds, wallBounds);
    ChecKSimonAttackedCollision();
    CheckVampireKillerCollision(ataque);
}

// Inicializa Simon, las animaciones, el suelo y las paredes
bool init()
{
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

    sf::Sprite simonSprite(gTextures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition({245.f, 171.f});
    
    sf::FloatRect bounds = simonSprite.getLocalBounds();
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    gSimonSprite = &gSprites.back();

    // Inicializar AnimationManager
    gAnimationManager = new AnimationManager(*gSimonSprite);

    // Definir animaciones
    std::vector<AnimationManager::Frame> idleFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> hurtFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(275, 21), sf::Vector2(16, 32)), 0.2f}
    };

    std::vector<AnimationManager::Frame> jumpFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> duckFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(84, 21), sf::Vector2(16, 32)), 0.1f}
    };
    std::vector<AnimationManager::Frame> walkFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 0.1f},
    };

    std::vector<AnimationManager::Frame> walkSlowFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 32)), 1.0f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 32)), 1.0f},
    };

    std::vector<AnimationManager::Frame> attackFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(26, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(51, 78), sf::Vector2(16, 32)),  0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 21), sf::Vector2(16, 32)),  0.1f}
    };

    std::vector<AnimationManager::Frame> attackFloorFrames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(172, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(197, 78), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 78), sf::Vector2(16, 32)), 0.1f}
    };

    std::vector<AnimationManager::Frame> whipLvl1Frames {
        AnimationManager::Frame{sf::IntRect(sf::Vector2(17, 135), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(50, 135), sf::Vector2(16, 32)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(67, 135), sf::Vector2(26, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(112, 134), sf::Vector2(28, 17)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(123, 25), sf::Vector2(16, 31)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(123, 25), sf::Vector2(16, 31)), 0.1f},
    };

   


    gAnimationManager->addAnimation(idleSimon, idleFrames);
    gAnimationManager->addAnimation(jumpSimon, jumpFrames);
    gAnimationManager->addAnimation(walkSimon, walkFrames);
    gAnimationManager->addAnimation(walkSlowSimon, walkSlowFrames,false);
    gAnimationManager->addAnimation(duckSimon, duckFrames);
    gAnimationManager->addAnimation(attackSimon, attackFrames,false);
    gAnimationManager->addAnimation(attackFloorSimon, attackFloorFrames,false);
    

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



    


    return true;
}

// Movimiento y animación de Simon
bool updateMovement(const float deltaTime, bool haciaArriba, bool haciaIzquierda, bool haciaDerecha, bool ducking,bool atacando)
{
    if (!gSimonSprite)
    {
        std::cerr << "Error: sprite de Simon no inicializado" << std::endl;
        return false;
    }

    // Aplicar la gravedad solo si Simon no está en el suelo
    if (!isOnGround)
    {
        verticalSpeed += GRAVITY * deltaTime;
        gSimonSprite->move({0.f, verticalSpeed * deltaTime});
    }

    // Saltar solo si Simon está en el suelo
    if (haciaArriba && isOnGround)
    {
        verticalSpeed = -JUMP_FORCE;  // Aplicar la fuerza de salto
        isOnGround = false;          // Simon ya no está en el suelo
    }
    if (ducking)
    {
        gSimonSprite->move({0.f, 3.0f});
    }
    
    // Movimiento horizontal
    if(!atacando){
        if (haciaIzquierda)
        {
            gSimonSprite->move({-1.5f * deltaTime * gMovementSpeed, 0.f});
            gSimonSprite->setScale({1.f, 1.f});
        }
        else if (haciaDerecha)
        {
            gSimonSprite->move({1.5f * deltaTime * gMovementSpeed, 0.f});
            gSimonSprite->setScale({-1.f, 1.f});
        }

    }
    
    return true;
}

void updateAnimation(bool isOnGround, bool haciaDerecha, bool haciaIzquierda, bool atacando, bool ducking, bool atacandoDucking)
{    
   
    
    if (!isOnGround && !atacando){
        currentAnimation = jumpSimon;
    }
    else if ((haciaDerecha || haciaIzquierda)) {
        currentAnimation = walkSimon;
    }
    else if (atacando){
        
        currentAnimation = attackSimon;
    }
    else if (atacandoDucking)
    {
        currentAnimation = attackFloorSimon;
        gSimonSprite->move({0.f, 3.0f}); // Ajusta la posición vertical
    }
    
    else if (ducking){
        
        currentAnimation = duckSimon;
        gSimonSprite->move({0.f, 3.0f}); // Ajusta la posición vertical
    
    }
    
    else{
        currentAnimation = idleSimon;
    }
    
    if(facing){
        gSimonSprite->setScale({-1.f, 1.f});
    }
    else{
        gSimonSprite->setScale({1.f, 1.f});
    }
    
    
    if (!gAnimationManager->isPlaying(currentAnimation)){
        
        gAnimationManager->playAnimation(currentAnimation);
        
        return;
    }
        
}
    

// Bucle principal del juego
int main()
{
    sf::RenderWindow window(sf::VideoMode({gWindowWidth, gWindowHeight}), "Castlevania", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;

    if (!init())
    {
        return -1;
    }
    
    bool haciaIzquierda { false };
    bool haciaDerecha { false };
    bool estabaLlendoDerecha { false };
    bool estabaLlendoIzquierda { false };
    bool haciaArriba { false };
    bool atacando { false };
    bool finataque { false };
    bool ducking { false };
    bool duckttacking { false };
    bool pulsarOtraVez { true };
    bool limite { false };
    float tiempoEnemigoRespawnActual { 0.0f };
    while (window.isOpen())
    {
        float deltaTime = deltaClock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (!limite)
                {
                    /* code */
                
                
                switch (keyPressed->scancode)
                {
                    case sf::Keyboard::Scancode::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Scancode::Up:
                        
                        if ( !ducking &&  !duckttacking && !atacando && isOnGround )
                        {
                            haciaArriba = true;
                            verticalSpeed = -JUMP_FORCE;
                            isOnGround = false;

                        }
                        break;
                    case sf::Keyboard::Scancode::Down:
                        if (!atacando)
                        {
                            ducking = true;
                        }
                    
                        
                        
                        
                        
                        break;
                    case sf::Keyboard::Scancode::Left:
                        
                        if (!ducking &&  !duckttacking && !atacando && (isOnGround ||(!isOnGround && !facing)))
                        {
                            
                            facing = false;
                            haciaIzquierda = true;
                            haciaDerecha = false;
                            gSimonSprite->setScale({-1.f, 1.f});
                        }
                        
                        
                        break;
                    case sf::Keyboard::Scancode::Right:
                        
                        if (!ducking && !duckttacking && !atacando && (isOnGround ||(!isOnGround && facing)))
                        {
                            facing = true;
                            haciaIzquierda = false;
                            haciaDerecha = true;
                            gSimonSprite->setScale({1.f, 1.f});
                        }
                        break;
                    case sf::Keyboard::Scancode::Z:
                        

                        if (!atacando && pulsarOtraVez){
                            gWhipAnimationManager->playAnimation(whipLvl1StandingJumping);
                            pulsarOtraVez = false;

                            if (ducking){
                                duckttacking = true;
                                atacando = false;                                
                            }
                            else
                            {
                                atacando = true;
                                estabaLlendoDerecha = haciaDerecha;
                                estabaLlendoIzquierda = haciaIzquierda;
                                                               
                                haciaArriba = false;
                                haciaDerecha = false;
                                haciaIzquierda = false;
                                gSimonSprite->setScale({-1.f, 1.f});
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
            {
                switch (keyReleased->scancode)
                {
                    case sf::Keyboard::Scancode::Up:
                        haciaArriba = false;
                        break;
                    case sf::Keyboard::Scancode::Left:
                        haciaIzquierda = false;
                        break;
                    case sf::Keyboard::Scancode::Right:
                        haciaDerecha = false;

                        break;
                    case sf::Keyboard::Scancode::Down:
                        ducking = false;
                        break;
                    case sf::Keyboard::Scancode::Z:
                        pulsarOtraVez = true;
                        if (finataque)
                        {
                            haciaIzquierda = estabaLlendoIzquierda;
                            haciaDerecha = estabaLlendoDerecha;
                            estabaLlendoDerecha = false;
                            estabaLlendoIzquierda = false;
                        }
                        finataque = false;
                        
                        

                        break;
                    default:
                        break;
                }
            }
        }
        //std::cout << currentAnimation << std::endl;
        // Verificar colisiones
        if (gSimonSprite->getPosition().x < 680)
        {
            updateEnemyRespawn(deltaTime, tiempoEnemigoRespawnActual);
            CheckAllCollisions(atacando || duckttacking);

            if (!updateMovement(deltaTime, haciaArriba, haciaIzquierda, haciaDerecha, ducking,atacando))
            {
                return -1;
            }
            updateAnimation(isOnGround, haciaDerecha, haciaIzquierda,atacando,ducking,duckttacking);
            //gWhipAnimationManager->update( deltaTime);
            gAnimationManager->update(deltaTime);
            
            if (Simonatacado) {
                atacadoCooldown += deltaTime; // Increment the cooldown timer
                std::cout << "Cooldown: " << atacadoCooldown << std::endl;
                if (atacadoCooldown >= atacadoCooldownDuration) {
                    Simonatacado = false; // Re-enable atacado after the cooldown
                }
            }
            
            if (atacando || duckttacking) {
                //gAnimationManager->update(deltaTime);
                gWhipAnimationManager->update(deltaTime);
                int currentFrame = gWhipAnimationManager->getCurrentFrameIndex();
                if(currentFrame==2 || currentFrame==3){
                    gWhipSprite->setPosition(sf::Vector2f(gSimonSprite->getPosition().x + (facing ? 24 : -16), gSimonSprite->getPosition().y-1));
                    gWhipSprite->setScale({facing ? -1.f : 1.f, 1.f});
                }
                else{
                    gWhipSprite->setPosition(sf::Vector2f(gSimonSprite->getPosition().x + (facing ? -16 : 16), gSimonSprite->getPosition().y-1));
                    gWhipSprite->setScale({facing ? 1.f : -1.f, 1.f});
                }
            }
            
            
            // Resetear 'atacando' cuando la animación termine
            if (atacando && gAnimationManager->isAnimationFinished()) {
                atacando = false;
                finataque = true;
                
            }
            if (duckttacking && gAnimationManager->isAnimationFinished()) {
                duckttacking = false;
            }
            if (facing)
            {
                gWhipSprite->setScale({-1.f, 1.f});
            }
            else
            {
                gWhipSprite->setScale({1.f, 1.f});
            }
            
            //std::cout << "Posicion simon:" << gSimonSprite->getPosition().x << " " << gSimonSprite->getPosition().y << std::endl;
            window.clear();
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
            
            if (enemigoVivo)
            {
                window.draw(*gEnemy.sprite);
                window.draw(FloatRectToRectShape(gEnemy.sprite->getGlobalBounds()));
            }
            

        }
        else if(gSimonSprite->getPosition().x < 730){

            haciaDerecha = true;
            haciaIzquierda = false;
            haciaArriba = false;
            ducking = false;
            atacando = false;
            isOnGround = true;
            duckttacking = false;
            gSimonSprite->move({1.5f * deltaTime * gMovementSpeedSlow, 0.f});
            gSimonSprite->setScale({-1.f, 1.f});
            if (!gAnimationManager->isPlaying(walkSlowSimon) || gAnimationManager->isAnimationFinished())
            {
                gAnimationManager->playAnimation(walkSlowSimon);
            }
            
            gAnimationManager->update(deltaTime*gMovementSpeedSlow);
            /*if (gAnimationManager->isAnimationFinished())
            {
                limite = false;
            }*/
            window.clear();
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
            
        }
        else{
            if (!gTextures["bgEntrada"].loadFromFile("../../assets/maps/level1Entrance.png", false))
            {
                std::cerr << "Error cargando la textura de fondo" << std::endl;
                return false;
            }
            sf::Sprite bgSprite(gTextures["bgEntrada"]);
            bgSprite.setTextureRect(sf::IntRect({774, 11}, {1542, 192}));
            gSprites.push_back(bgSprite);
            gSimonSprite->setPosition({10.f, 171.f});
            //simonSprite.setPosition({245.f, 171.f});
            window.clear();
            for (const auto& sprite : gSprites)
            {
                window.draw(sprite);
            }
        
        }
        window.draw(gFloor);
        window.draw(FloatRectToRectShape(gFloor.getGlobalBounds()));
        
        
        window.draw(gWall);
       // window.draw(FloatRectToRectShape(gWall.getGlobalBounds()));
        window.draw(*gSimonSprite);
       // window.draw(FloatRectToRectShape(gSimonSprite->getGlobalBounds()));
        if (atacando || duckttacking) {
          window.draw(*gWhipSprite);
          window.draw(FloatRectToRectShape(gWhipSprite->getGlobalBounds()));
        }
        window.display();
    }
    

    delete gAnimationManager;
    //delete gWhipAnimationManager;

    return 0;
}
