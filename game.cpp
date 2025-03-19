#include "game.h"
#include <iostream>
#include <cmath>
#include "globals.h"

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
        
    // --------------------------------------------------
    // GUI
    // --------------------------------------------------
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

    float margin = gWindowWidth * 0.025f;

    // Score
    sf::Text scoreText(font, "SCORE-000000", 12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(margin, margin));

    // Time
    sf::Text timeText(font, "TIME 0147", 12);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(sf::Vector2f(gWindowWidth * 0.46f, margin));

    // Stage
    sf::Text stageText(font, "STAGE 01", 12);
    stageText.setFillColor(sf::Color::White);
    stageText.setPosition(sf::Vector2f(gWindowWidth * 0.76f, margin));

    // Player
    sf::Text playerText(font, "PLAYER", 12);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(sf::Vector2f(margin, margin + 15));

    // Enemy
    sf::Text enemyText(font, "ENEMY", 12);
    enemyText.setFillColor(sf::Color::White);
    enemyText.setPosition(sf::Vector2f(margin, margin + 30));

    texts.push_back(scoreText);
    texts.push_back(timeText);
    texts.push_back(stageText);
    texts.push_back(playerText);
    texts.push_back(enemyText);
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

    // GUI
    for (const auto& text : texts) {
        std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    player.draw(window);
}

sf::View Game::getView(sf::RenderWindow& window, Camera& camera){
    camera.startVertex.x = player.sprite->getPosition().x - (camera.getView(window.getSize()).getSize().x / 2.f);

    sf::View view = camera.getView(window.getSize()); // Obtener la vista actual de la cámara

    sf::Vector2f playerPosition = player.sprite->getPosition();

     // Centrar la vista horizontalmente en la posición del jugador
    view.setCenter(sf::Vector2f(static_cast<int>(playerPosition.x), static_cast<int>(view.getCenter().y))); 
    
    return view;
}
