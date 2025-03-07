#include "Player.hpp"
#include "camera.h"
#include <iostream>

using namespace std;

bool gEnMovimiento { false };
constexpr int escala { 1 };
constexpr int gWindowWidth{ 800 * escala};
constexpr int gWindowHeight{ 250 * escala};

// NUEVO
AnimationManager* gAnimationManager { nullptr };
animationID currentAnimation;

sf::RectangleShape gFloor;


Camera camera(sf::FloatRect({0.f, 0.f}, {gWindowWidth, gWindowHeight}));

// Contenedores para sprites y texturas
std::vector<sf::Sprite> gSprites;
std::unordered_map<std::string, sf::Texture> gTextures;

Player player = Player();
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
                player.sprite->move({-overlapX, 0.f});
            }
            else
            {
                //std::cout << "Colision con borde lateral derecho de objeto." << std::endl;
                player.sprite->move({overlapX, 0.f});
            }
        }
        else    // Colisión vertical
        {
            if ((simonBounds.position.y + simonBounds.size.y * 0.5f) < (objectBounds.position.y + objectBounds.size.y * 0.5f))
            {
                //std::cout << "Colision con borde superior de objeto." << std::endl;
                player.sprite->move({0.f, -overlapY});
                player.verticalSpeed = 0.0f;   // Simon deja de caere
                player.isOnGround = true;      // Indicamos que Simon está en el suelo
            }
            else
            {
                //std::cout << "Colision con borde inferior de objeto." << std::endl;
                player.sprite->move({0.f, overlapY});
                player.verticalSpeed = 0.0f;   // Simon pasará a estar cayendo
            }
        }
    }
}


void CheckAllCollisions(const bool ataque)
{
    sf::FloatRect simonBounds = player.sprite->getGlobalBounds();
    sf::FloatRect floorBounds = gFloor.getGlobalBounds();

    CheckCollisions(simonBounds, floorBounds);
    
}

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
    simonImage.createMaskFromColor(sf::Color(0, 128, 0)); // color key

    gTextures["simon"] = sf::Texture(simonImage, false);

    sf::Sprite simonSprite(gTextures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition({245.f, 171.f});
    sf::FloatRect bounds = simonSprite.getLocalBounds();
    
    // Ajusta el origen de las transformaciones al centro inferior
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    player.sprite = &gSprites.back();

    // AÑADIDO NUEVO POR SERGIO, INTENTANDO JUNTAR LA MÁQUINA DE ESTADOS PARA Q
    // TENGA TODOS, USANDO LA CLASE PLAYER Y LA DE ANIMATION MANAGER
    // Inicializar AnimationManager
    gAnimationManager = new AnimationManager(*player.sprite);
    
    gAnimationManager->addAnimation(idleSimon, player.idleFrames);
    gAnimationManager->addAnimation(jumpSimon, player.jumpFrames);
    gAnimationManager->addAnimation(walkSimon, player.walkFrames);
    gAnimationManager->addAnimation(walkSlowSimon, player.walkSlowFrames,false);
    gAnimationManager->addAnimation(duckSimon, player.duckFrames);
    gAnimationManager->addAnimation(attackSimon, player.attackFrames,false);
    gAnimationManager->addAnimation(attackFloorSimon, player.attackFloorFrames,false);
    

    gAnimationManager->playAnimation(idleSimon);
    currentAnimation = idleSimon;
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
        CheckAllCollisions(false);
        player.update(deltaTime);
        if (!gAnimationManager->isPlaying(player.currentAnimation)){
        
            gAnimationManager->playAnimation(player.currentAnimation);

        }
        gAnimationManager->update(deltaTime);
        if (player.isAttacking && gAnimationManager->isAnimationFinished())
        {
            player.isAttacking = false;
            player.attackedFinished = true;
        }
        
        window.clear();
        
        for (const auto& sprite : gSprites)
        {   
            window.draw(sprite);
        }
        window.draw(gFloor);
        //window.draw(*player.sprite);
        player.draw(window);
        window.display();
    }
}