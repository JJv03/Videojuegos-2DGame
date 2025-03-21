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
    // tilemapManager = TilemapManager();
}

// Initializes a new game from the beggining
void Game::init(){
    currentLevel = 1;
    currentStage = 1;

    //tilemapManager.loadLevel(1);

    // Temporary
    if (!tileMap.load("./assets/tilesets/tileset_1.png", "./assets/tilemaps/level1/tilemap_1_1.txt")){
        std::cerr << "Error al cargar el tilemap." << std::endl;
        return;
    }

    // Simon ----------------------------------------------------------------
    
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
    simonSprite.setPosition({245.f, 161.f});
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

    // Whip ----------------------------------------------------------------
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
    
    // Inicializar AnimationManager ----------------------------------------------------------------
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

    float margin = gWindowWidth * 0.015f;

    // Score
    sf::Text scoreText(font, "SCORE-000000", 11);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(margin, margin));

    // Time
    sf::Text timeText(font, "TIME 0147", 11);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(sf::Vector2f(gWindowWidth * 0.46f, margin));

    // Stage
    sf::Text stageText(font, "STAGE 01", 11);
    stageText.setFillColor(sf::Color::White);
    stageText.setPosition(sf::Vector2f(gWindowWidth * 0.76f, margin));

    // Player
    sf::Text playerText(font, "PLAYER", 11);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(sf::Vector2f(margin, margin + 15));

    // Enemy
    sf::Text enemyText(font, "ENEMY", 11);
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
    //checkCollisions();
}

// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow& window, Camera& camera){
    //camera.updateView(*player.sprite, tileMap.getMapBounds(), 100.f);
    tileMap.drawScene(window, camera);

    // GUI
    sf::View gameView = window.getView();

    // Configurar una nueva vista fija para la GUI
    sf::View guiView(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(gWindowWidth, gWindowHeight)));
    window.setView(guiView); // Aplicar la vista fija para la GUI

    // Dibujar el rectángulo negro en la parte superior
    sf::RectangleShape guiBackground(sf::Vector2f(gWindowWidth, 50)); // Altura de 50px
    guiBackground.setFillColor(sf::Color::Black);
    guiBackground.setPosition(sf::Vector2f(0, 0));
    window.draw(guiBackground);

    // Dibujar los textos de la GUI
    for (const auto& text : texts) {
        window.draw(text);
    }

    // Restaurar la vista original del juego
    window.setView(gameView);

    player.draw(window);
}

/*void Game::draw(sf::RenderWindow& window, Camera& camera) {
    // Modificar la vista del juego para bajarlo
    sf::View gameView = camera.getView(window.getSize()); // Obtener la vista actual de la cámara
    gameView.move(sf::Vector2f(0, 20)); // Mover la vista del juego 20 píxeles hacia abajo (ajusta según sea necesario)
    window.setView(gameView);

    // Dibujar el mapa y los sprites
    tileMap.drawScene(window, camera);
    player.draw(window);

    // Restaurar la vista de la GUI
    sf::View guiView(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(gWindowWidth, gWindowHeight)));
    window.setView(guiView); // Aplicar la vista fija para la GUI

    // Dibujar el rectángulo negro en la parte superior
    sf::RectangleShape guiBackground(sf::Vector2f(gWindowWidth, 50)); // Altura de 50px
    guiBackground.setFillColor(sf::Color::Black);
    guiBackground.setPosition(sf::Vector2f(0, 0));
    window.draw(guiBackground);

    // Dibujar los textos de la GUI
    for (const auto& text : texts) {
        window.draw(text);
    }

    // Restaurar la vista original del juego
    window.setView(gameView);
}*/

sf::View Game::getView(sf::RenderWindow& window, Camera& camera) {
    sf::View view = camera.getView(window.getSize());
    sf::Vector2f playerPosition = this->player.sprite->getPosition();

    // Obtener los límites del mapa
    sf::FloatRect mapBounds = this->tileMap.getMapBounds();
    sf::Vector2f viewSize = view.getSize();

    // Calcular los límites permitidos para la cámara en X
    float minX = mapBounds.position.x + viewSize.x / 2;
    float maxX = mapBounds.position.x + mapBounds.size.x - viewSize.x / 2;

    // Calcular los límites permitidos para la cámara en Y
    float minY = mapBounds.position.y + viewSize.y / 2;
    float maxY = mapBounds.position.y + mapBounds.size.y - viewSize.y / 2;

    // Restringir la posición de la cámara dentro de los límites del mapa
    float centerX = std::max(minX, std::min(view.getCenter().x, maxX));
    float centerY = std::max(minY, std::min(view.getCenter().y, maxY));
    view.setCenter({centerX, centerY});

    // Esquina de la cámara en la esquina superior izquierda, limitada por los bordes del mapa
    if(playerPosition.x < minX) {
        camera.startVertex.x = minX - (camera.getView(window.getSize()).getSize().x / 2.f);
    }else if(playerPosition.x > maxX) {
        camera.startVertex.x = maxX - (camera.getView(window.getSize()).getSize().x / 2.f);
    } else {
        camera.startVertex.x = player.sprite->getPosition().x -
            (camera.getView(window.getSize()).getSize().x / 2.f);
    }

    return view;
}


// -------------------------------------------------------------------------------------
//                                    COLLISIONS
// -------------------------------------------------------------------------------------


void Game::checkCollisions() {
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

    // Check collisions with the tilemap
    for (int row = 0; row < tileMap.m_tilesPerRow; ++row) {
        for (int col = 0; col < tileMap.m_tilesPerColumn; ++col) {
            if (tileMap.m_solidTiles[row][col].isVisible) {
                sf::FloatRect tileBounds = tileMap.m_solidTiles[row][col].hitbox;

                if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBounds)) {
                    const float overlapX = intersection->size.x;
                    const float overlapY = intersection->size.y;

                    if (overlapX < overlapY) {      // Horizontal collision
                        if ((playerBounds.position.x + playerBounds.size.x * 0.5f) < (tileBounds.position.x + tileBounds.size.x * 0.5f))
                        {
                            player.sprite->move({-overlapX, 0.f});
                        }
                        else
                        {
                            player.sprite->move({overlapX, 0.f});
                        }
                    } else {    // Vertical collision
                        // Simon's feet are collisioning with the tile
                        if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
                        {
                            if (player.verticalSpeed < 0.0f) {      // If player is NOT going up
                                player.sprite->move({0.f, -overlapY});
                                player.isOnGround = true;      // Indicamos que Simon está en el suelo
                            }
                        }
                        else    // Simon's head is collisioning with the tile
                        {
                            player.sprite->move({0.f, overlapY});
                            player.verticalSpeed = 0.0f;        // Simon starts falling
                        }
                    }
                }
            }
        }
    }
}
