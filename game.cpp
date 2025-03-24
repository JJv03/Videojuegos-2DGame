#include "game.h"
#include <iostream>
#include <cmath>
#include <optional>
#include <sstream>
#include "globals.h"

AnimationManager *gAnimationManager{nullptr};
AnimationManager *gWhipAnimationManager{nullptr};
std::unordered_map<std::string, sf::Texture> gTextures;
std::vector<sf::Sprite> gSprites;

// Constructor, destructor
Game::Game()
{
    Player player;
    // tileMap = TileMap();
    tilemaps = TilemapManager();
}

// Initializes a new game from the beggining
void Game::init()
{
    currentLevel = 1;
    currentStage = 1;

    gameSoundManager.loadMusic("gameMusic", "./assets/music/03Vampire_Killer.mp3");
    gameSoundManager.playMusic("gameMusic", gMusicVolume);

    tilemaps.loadLevel(1);

    // Temporary
    // if (!tileMap.load(1, 1)){
    //    std::cerr << "Error al cargar el tilemap." << std::endl;
    //    return;
    //}

    // Simon ----------------------------------------------------------------

    // Cargar imagen y configurar textura de Simon (aplicando color key)
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    simonImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    simonImage.createMaskFromColor(sf::Color(0, 128, 0));        // color key

    gTextures["simon"] = sf::Texture(simonImage, false);

    sf::Sprite simonSprite(gTextures["simon"]);
    simonSprite.setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite.setPosition(tilemaps[currentStage].initialPosition);

    sf::FloatRect bounds = simonSprite.getLocalBounds();

    // Ajusta el origen de las transformaciones al centro inferior
    simonSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    gSprites.push_back(simonSprite);
    player.sprite = &gSprites.back();

    gAnimationManager = new AnimationManager(*player.sprite);

    gAnimationManager->addAnimation(idleSimon, player.idleFrames);
    gAnimationManager->addAnimation(jumpSimon, player.jumpFrames);
    gAnimationManager->addAnimation(walkSimon, player.walkFrames);
    gAnimationManager->addAnimation(walkSlowSimon, player.walkSlowFrames, false);
    gAnimationManager->addAnimation(duckSimon, player.duckFrames);
    gAnimationManager->addAnimation(attackSimon, player.attackFrames, false);
    gAnimationManager->addAnimation(attackFloorSimon, player.attackFloorFrames, false);

    gAnimationManager->playAnimation(idleSimon);
    player.currentAnimation = idleSimon;

    // Enemies -------------------------------------------------------------

    const sf::Vector2f ZOMBIE_POSITION = {350.f, 171.f};
    const sf::Vector2f LEOPARD_POSITION = {400.f, 171.f};

    zombiesSpawner.push_back(ZombieSpawner(ZOMBIE_POSITION, {50.f, 50.f}));

    leopard.push_back(Leopard::createLeopard(LEOPARD_POSITION));

    // Whip ----------------------------------------------------------------
    sf::Image whipImage;
    if (!whipImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    whipImage.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    whipImage.createMaskFromColor(sf::Color(0, 128, 0));        // color key
    gTextures["whip"] = sf::Texture(whipImage, false);

    sf::Sprite whipSprite(gTextures["whip"]);
    whipSprite.setTextureRect(sf::IntRect({1, 477}, {8, 32}));
    whipSprite.setPosition({245.f, 171.f});

    whipSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});

    player.whipSprite = new sf::Sprite(whipSprite);

    // Inicializar AnimationManager ----------------------------------------------------------------
    gWhipAnimationManager = new AnimationManager(*player.whipSprite);

    if (!gWhipAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Whip AnimationManager!" << std::endl;
    }
    gWhipAnimationManager->addAnimation(whipLvl1StandingJumping, player.whipLvl1Frames, false);

    // Se pasa a player y así toda la animación no se tiene que gestionar en el main
    player.gAnimationManager = gAnimationManager;
    player.gWhipAnimationManager = gWhipAnimationManager;

    // --------------------------------------------------
    // GUI
    // --------------------------------------------------
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cout << "No se ha encontrado la fuente" << std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

    float margin = gWindowWidth * 0.015f;

    // Score
    sf::Text scoreText(font, "SCORE-000000", 11);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(margin, margin));

    // Time
    sf::Text timeText(font, "TIME 300", 11);
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

    startStage(1);
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event)
{
    player.handleInput(event);
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime)
{
    player.update(deltaTime);

    for (auto &zombieSpawner : zombiesSpawner)
    {
        zombieSpawner.update(deltaTime, player.gPlayerActivationZone, player.gPlayerDeactivationZone);
    }
    for (auto &leopard : leopard)
    {
        leopard.update(deltaTime, player.gPlayerActivationZone, player.gPlayerDeactivationZone, player.sprite->getPosition());
    }

    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    // Reducir el tiempo cada segundo completo
    if (timeAccumulator >= 1.0f)
    {
        if (time > 0)
            time -= static_cast<int>(timeAccumulator);
        if (time < 0)
            time = 0;
        timeAccumulator = 0.0f;
        texts[1].setString("TIME " + std::to_string(time));
    }

    // Actualizar el score
    std::stringstream scoreStream;
    scoreStream << "SCORE-" << std::setw(6) << std::setfill('0') << player.score; // Formato con ceros
    texts[0].setString(scoreStream.str());

    checkCollisions();
}

// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow &window, Camera &camera)
{

    if (isLoading)
    {
        isLoading = false;
        loadingClock.restart();
    }

    if (loadingClock.getElapsedTime().asSeconds() < 0.5f)
    {
        sf::RectangleShape blackScreen(camera.getView(window.getSize()).getSize());
        blackScreen.setFillColor(sf::Color::Black);
        window.draw(blackScreen);
    }
    else
    {
        // camera.updateView(*player.sprite, tileMap.getMapBounds(), 100.f);
        tilemaps[currentStage].drawScene(window, camera);

        // DEBUG: Draw player and whip hitbox
        window.draw(FloatRectToRectShape(player.sprite->getGlobalBounds()));
        if (player.isAttacking)
        {
            window.draw(FloatRectToRectShape(player.whipSprite->getGlobalBounds()));
        }

        // GUI
        sf::View gameView = window.getView();

        // Configure a new view for the GUI
        sf::View guiView(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(gWindowWidth, gWindowHeight)));
        window.setView(guiView);

        // Draw the black rectangle
        sf::RectangleShape guiBackground(sf::Vector2f(gWindowWidth, 50));
        guiBackground.setFillColor(sf::Color::Black);
        guiBackground.setPosition(sf::Vector2f(0, 0));
        window.draw(guiBackground);

        // Draw the GUI texts
        for (const auto &text : texts)
        {
            window.draw(text);
        }

        drawHealthBars(window, player.health, 16); // CHANGE FOR BOSS HEALTH!!!!!

        // Restore the original game view
        window.setView(gameView);

        player.draw(window);

        for (auto &zombieSpawner : zombiesSpawner)
        {
            zombieSpawner.draw(window, true);
        }
        for (auto &leopard : leopard)
        {
            leopard.draw(window, true);
        }

        window.draw(FloatRectToRectShape(player.gPlayerActivationZone));
        window.draw(FloatRectToRectShape(player.gPlayerDeactivationZone));
    }
}

void Game::drawHealthBars(sf::RenderWindow &window, int playerHealth, int bossHealth)
{
    const int MAX_HEALTH = 16;
    const int SEGMENT_WIDTH = 5;
    const int SEGMENT_HEIGHT = 10;
    const int SPACING = 2;
    const float BORDER_THICKNESS = 1.0f;
    const sf::Vector2f START_POS(78, 22);

    // Player
    for (int i = 0; i < MAX_HEALTH; ++i)
    {
        bool isFull = (i < playerHealth);

        float width = SEGMENT_WIDTH - (isFull ? 0 : BORDER_THICKNESS * 2);
        float height = SEGMENT_HEIGHT - (isFull ? 0 : BORDER_THICKNESS * 2);
        sf::RectangleShape segment(sf::Vector2f(width, height));

        // Adjust the position to center the empty segments within the border
        float xOffset = isFull ? 0 : BORDER_THICKNESS;
        float yOffset = isFull ? 0 : BORDER_THICKNESS;
        segment.setPosition(sf::Vector2f(78 + i * (SEGMENT_WIDTH + SPACING) + xOffset, 22 + yOffset));

        segment.setOutlineThickness(BORDER_THICKNESS);
        segment.setOutlineColor(isFull ? sf::Color::Black : sf::Color::White);
        segment.setFillColor(isFull ? sf::Color::Red : sf::Color::Black);

        window.draw(segment);
    }

    // Boss
    for (int i = 0; i < MAX_HEALTH; ++i)
    {
        bool isFull = (i < bossHealth);

        float width = SEGMENT_WIDTH - (isFull ? 0 : BORDER_THICKNESS * 2);
        float height = SEGMENT_HEIGHT - (isFull ? 0 : BORDER_THICKNESS * 2);
        sf::RectangleShape segment(sf::Vector2f(width, height));

        float xOffset = isFull ? 0 : BORDER_THICKNESS;
        float yOffset = isFull ? 0 : BORDER_THICKNESS;
        segment.setPosition(sf::Vector2f(78 + i * (SEGMENT_WIDTH + SPACING) + xOffset, 35 + yOffset));

        segment.setOutlineThickness(BORDER_THICKNESS);
        segment.setOutlineColor(isFull ? sf::Color::Black : sf::Color::White);
        segment.setFillColor(isFull ? sf::Color(255, 150, 120) : sf::Color::Black);

        window.draw(segment);
    }
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

sf::View Game::getView(sf::RenderWindow &window, Camera &camera)
{
    sf::View view = camera.getView(window.getSize());
    sf::Vector2f playerPosition = this->player.sprite->getPosition();

    // Obtener los límites del mapa
    sf::FloatRect mapBounds = this->tilemaps[currentStage].getMapBounds();
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
    if (playerPosition.x < minX)
    {
        camera.startVertex.x = minX - (camera.getView(window.getSize()).getSize().x / 2.f);
    }
    else if (playerPosition.x > maxX)
    {
        camera.startVertex.x = maxX - (camera.getView(window.getSize()).getSize().x / 2.f);
    }
    else
    {
        camera.startVertex.x = player.sprite->getPosition().x -
                               (camera.getView(window.getSize()).getSize().x / 2.f);
    }

    return view;
}

// -------------------------------------------------------------------------------------
//                                    COLLISIONS
// -------------------------------------------------------------------------------------

void Game::checkCollisions()
{

    checkPlayerMapBoundCollisions();

    checkPlayerTileCollisions();

    checkEnemiesCollisions();
}

void Game::checkEnemiesCollisions()
{

    // ESTAS BOUNDS DEBERÍAN SER CON LAS HITBOXES NO CON LOS SPRITES
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

    sf::FloatRect whipBounds = player.whipSprite->getGlobalBounds();

    for (auto &zombieSpawner : zombiesSpawner)
    {
        zombieSpawner.checkCollisions(playerBounds, whipBounds, tilemaps[currentStage], player.isAttacking, player.DAMAGE);
    }
    for (auto &leopard : leopard)
    {
        leopard.checkCollisions(playerBounds, whipBounds, tilemaps[currentStage], player.isAttacking, player.DAMAGE);
    }
}

void Game::checkPlayerMapBoundCollisions()
{
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

    sf::FloatRect mapBounds = tilemaps[currentStage].getMapBounds();

    float halfPlayerWidth = playerBounds.size.x / 2;
    // float halfPlayerHeight = playerBounds.size.y / 2; // Comentado por warning

    if (playerBounds.position.x < mapBounds.position.x)
    {
        player.sprite->setPosition({mapBounds.position.x + halfPlayerWidth, player.sprite->getPosition().y});
    }
    if (playerBounds.position.x + playerBounds.size.x > mapBounds.position.x + mapBounds.size.x)
    {
        player.sprite->setPosition({mapBounds.position.x + mapBounds.size.x - halfPlayerWidth, player.sprite->getPosition().y});
    }
    /*
    if (playerBounds.position.y < mapBounds.position.y) {
        player.sprite->setPosition({player.sprite->getPosition().x, mapBounds.position.y + halfPlayerHeight});
    }
    if (playerBounds.position.y + playerBounds.size.y > mapBounds.position.y + mapBounds.size.y) {
        player.sprite->setPosition({player.sprite->getPosition().x, mapBounds.position.y + mapBounds.size.y - halfPlayerHeight});
    }
    */
}

void Game::checkPlayerTileCollisions()
{
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();
    bool hasCollided = false;

    // Solid tiles
    for (int col = 0; col < tilemaps[currentStage].m_tilesPerRow; ++col)
    {
        for (int row = 0; row < tilemaps[currentStage].m_tilesPerColumn; ++row)
        {
            if (tilemaps[currentStage].m_solidTiles[row][col].isVisible)
            {
                sf::FloatRect tileBounds = tilemaps[currentStage].m_solidTiles[row][col].hitbox;

                if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBounds))
                {
                    hasCollided = true;
                    const float overlapX = intersection->size.x;
                    const float overlapY = intersection->size.y;

                    if (overlapX < overlapY)
                    { // Horizontal collision
                        if ((playerBounds.position.x + playerBounds.size.x * 0.5f) < (tileBounds.position.x + tileBounds.size.x * 0.5f))
                        {
                            player.sprite->move({-overlapX, 0.f});
                        }
                        else
                        {
                            player.sprite->move({overlapX, 0.f});
                        }
                    }
                    else
                    { // Vertical collision
                        if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
                        { // Simon's feet are collisioning with the tile
                            if (player.verticalSpeed > 0.0f)
                            { // If player is NOT going up
                                player.sprite->move({0.f, -overlapY});
                                player.isOnGround = true; // Set Simon to be on ground
                            }
                        }
                        else // Simon's head is collisioning with the tile
                        {
                            player.sprite->move({0.f, overlapY});
                            player.verticalSpeed = 0.0f; // Simon starts falling
                        }
                    }
                }
            }
        }
    }

    if (!hasCollided)
    { // If Simon is not colliding with any solid tile
        player.isOnGround = false;
    }

    // Door tiles
    // i+1 = stage number
    for (auto &doorEntry : tilemaps[currentStage].m_doorTiles)
    {
        sf::FloatRect doorBounds = doorEntry.second.hitbox;

        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(doorBounds))
        {

            if (doorEntry.second.type == TileMap::DoorTile::Type::DOOR)
            {
                // Quitar puerta (ya no está disponible)
            }

            int doorId = doorEntry.first;

            if (int(currentStage) == tilemaps.doors[doorId].prev_stage)
            {
                std::cout << "NEXT STAGE" << std::endl;
                isLoading = true;

                if (tilemaps.doors[doorId].type == TileMap::DoorTile::Type::STAIRS)
                {
                    startStage(tilemaps.doors[doorId].next_stage, doorId);
                }
                else
                {
                    startStage(tilemaps.doors[doorId].next_stage);
                }
            }
            else if (int(currentStage) == tilemaps.doors[doorId].next_stage)
            {
                std::cout << "PREVIOUS STAGE" << std::endl;
                isLoading = true;

                if (tilemaps.doors[doorId].type == TileMap::DoorTile::Type::STAIRS)
                {
                    startStage(tilemaps.doors[doorId].prev_stage, doorId);
                }
                else
                {
                    startStage(tilemaps.doors[doorId].prev_stage);
                }
            }
            else if (100 == tilemaps.doors[doorId].next_stage)
            {
                std::cout << "NEXT LEVEL" << std::endl;
                isLoading = true;
                currentLevel += 1;
                tilemaps.loadLevel(currentLevel);
            }
            else
            {
                std::cout << "ERROR: Stage doesn't correspond to any door stages" << std::endl;
            }
        }
    }
}

int Game::startStage(int stage, int fromDoor)
{
    if (unsigned(stage) > tilemaps.tilemaps.size())
    {
        std::cerr << "ERROR: Level " << currentLevel << ", stage " << stage << " doesn't exist";
        return -1;
    }

    currentStage = stage;

    std::cout << "Current stage: " << currentStage << std::endl;
    if (fromDoor == 0)
    {
        std::cout << "NO stairs: " << tilemaps[currentStage].initialPosition.x << ", " << tilemaps[currentStage].initialPosition.y << std::endl;
        player.sprite->setPosition(tilemaps[currentStage].initialPosition);
    }
    else
    {
        std::cout << "STAIRS: fromDoor: " << fromDoor << ", " << tilemaps[currentStage].m_doorTiles[fromDoor].playerAparition.x << ", " << tilemaps[currentStage].m_doorTiles[fromDoor].playerAparition.y << std::endl;
        player.sprite->setPosition(tilemaps[currentStage].m_doorTiles[fromDoor].playerAparition);
    }

    return stage;
}

int Game::goToStage(int fromDoor)
{
    if (tilemaps.doors.find(fromDoor) == tilemaps.doors.end())
    {
        std::cerr << "ERROR: Level " << currentLevel << ", door " << fromDoor << " doesn't exist";
        return -1;
    }

    return fromDoor;
}
