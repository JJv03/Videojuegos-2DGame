#include "game.h"
#include <iostream>
#include <cmath>

AnimationManager* gAnimationManager { nullptr };
AnimationManager* gWhipAnimationManager { nullptr };
std::unordered_map<std::string, sf::Texture> gTextures;
std::vector<sf::Sprite> gSprites;

// Constructor, destructor
Game::Game(){
    player = Player();
    tileMap = TileMap();
}

// Initializes a new game from the beggining
void Game::init(){
    if (!tileMap.load("./assets/tilesets/tileset_1.png", "./assets/tilesets/tilemap_1_1.txt", 24, 7)){
        std::cerr << "Error al cargar el tilemap." << std::endl;
        return;
    }

// Simon ----------------------------------------------------------------------------
    
    // Cargar imagen y configurar textura de Simon (aplicando color key)
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
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

    gAnimationManager = new AnimationManager(*player.sprite);
    
    gAnimationManager->addAnimation(idleSimon, player.idleFrames);
    gAnimationManager->addAnimation(jumpSimon, player.jumpFrames);
    gAnimationManager->addAnimation(walkSimon, player.walkFrames);
    gAnimationManager->addAnimation(walkSlowSimon, player.walkSlowFrames,false);
    gAnimationManager->addAnimation(duckSimon, player.duckFrames);
    gAnimationManager->addAnimation(attackSimon, player.attackFrames,false);
    gAnimationManager->addAnimation(attackFloorSimon, player.attackFloorFrames,false);
    

    gAnimationManager->playAnimation(idleSimon);
    player.currentAnimation = idleSimon;

    // Whip
    sf::Image whipImage;
    if (!whipImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    whipImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    whipImage.createMaskFromColor(sf::Color(0, 128, 0)); // color key
    gTextures["whip"] = sf::Texture(whipImage, false);

    sf::Sprite whipSprite(gTextures["whip"]);
    whipSprite.setTextureRect(sf::IntRect({1, 477}, {8, 32}));
    whipSprite.setPosition({245.f, 171.f});
    
    whipSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    
    
    player.whipSprite =  new sf::Sprite(whipSprite);
    
    // Inicializar AnimationManager
    gWhipAnimationManager = new AnimationManager(*player.whipSprite);

    if (!gWhipAnimationManager) {
        std::cerr << "Error: Failed to initialize Whip AnimationManager!" << std::endl;
    }
    gWhipAnimationManager->addAnimation(whipLvl1StandingJumping, player.whipLvl1Frames, false);

    // Se pasa a player y así toda la animación no se tiene que gestionar en el main
    player.gAnimationManager = gAnimationManager;
    player.gWhipAnimationManager = gWhipAnimationManager;
        
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event){
    player.handleInput(event);
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime){
    player.update(deltaTime);
}

// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow& window, Camera& camera){
    //camera.updateView(*player.sprite, tileMap.getMapBounds(), 100.f);
    tileMap.drawScene(window, camera);
    player.draw(window);
}

sf::View Game::getView(sf::RenderWindow& window, Camera& camera){
    float verticalMargin = 100.f;
    sf::View view = camera.getView(window.getSize());

    sf::Vector2f playerPos = player.sprite->getPosition();
    sf::Vector2f viewCenter = view.getCenter();
    sf::FloatRect mapBounds = tileMap.getMapBounds();

    // Nueva posición de la vista, centrándose en el jugador (horizontalmente)
    float newX = playerPos.x;

    // Limitar la posición vertical si el jugador se mueve dentro del margen permitido
    float minY = mapBounds.position.y + verticalMargin;
    float maxY = mapBounds.position.y + mapBounds.size.y - verticalMargin;
    
    float newY = viewCenter.y; // Mantener la altura por defecto
    if (playerPos.y < minY) {
        newY = minY;
    } else if (playerPos.y > maxY) {
        newY = maxY;
    }

    // Evitar que la cámara se salga de los límites del mapa
    float halfWidth = view.getSize().x / 2.0f;
    float halfHeight = view.getSize().y / 2.0f;

    newX = std::max(mapBounds.position.x + halfWidth, std::min(newX, mapBounds.position.x + mapBounds.size.x - halfWidth));
    newY = std::max(mapBounds.position.x + halfHeight, std::min(newY, mapBounds.position.x + mapBounds.size.y - halfHeight));

    // Aplicar nueva posición de la vista
    view.setCenter(sf::Vector2f(newX, newY));

    return view;
}
