#include "game.h"
#include <iostream>
#include <cmath>
#include <optional>
#include <sstream>
#include "globals.h"
#include "utils.h"
#include "configManager.h"

std::unordered_map<std::string, sf::Texture> gTextures;

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

    configManager& configManager = configManager::getInstance();
    auto audio = configManager.getAudio();
    float musicVol = (audio.master_volume * audio.music_volume)/100;

    gameSoundManager.loadMusic("gameMusic", "./assets/music/03Vampire_Killer.mp3");
    gameSoundManager.playMusic("gameMusic", musicVol);

    tilemaps.loadLevel(1);

    // Temporary
    // if (!tileMap.load(1, 1)){
    //    std::cerr << "Error al cargar el tilemap." << std::endl;
    //    return;
    //}

    // Simon ----------------------------------------------------------------

    // Loads simon texture
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    simonImage.createMaskFromColor(gColorKeyGrey);
    simonImage.createMaskFromColor(gColorKeyGreen);

    gTextures["simon"] = sf::Texture(simonImage, false);

    auto simonSprite = std::make_shared<sf::Sprite>(gTextures["simon"]);
    simonSprite->setTextureRect(sf::IntRect({1, 21}, {16, 32}));
    simonSprite->setPosition(tilemaps[currentStage].initialPosition);

    sf::FloatRect bounds = simonSprite->getLocalBounds();

    // Adjusts the transformation origin to the bottom center
    simonSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});
    player.sprite = simonSprite;
    player.hitboxes.push_back(player.sprite.get()->getLocalBounds());

    AnimationManager *animationManager = new AnimationManager(*player.sprite);

    animationManager->addAnimation(idleSimon, player.idleFrames);
    animationManager->addAnimation(jumpSimon, player.jumpFrames);
    animationManager->addAnimation(walkSimon, player.walkFrames);
    animationManager->addAnimation(walkSlowSimon, player.walkSlowFrames, false);
    animationManager->addAnimation(duckSimon, player.duckFrames);
    animationManager->addAnimation(attackSimon, player.attackFrames, false);
    animationManager->addAnimation(attackFloorSimon, player.attackFloorFrames, false);

    animationManager->playAnimation(idleSimon);
    player.currentAnimation = idleSimon;

    // Enemies -------------------------------------------------------------

    // LEVEL 1 - STAGE 2
    zombiesSpawner.push_back(ZombieSpawner({200.f, 176.f}, {50.f, 50.f}, 1, 2));
    leopard.push_back(createLeopard({696.f, 112.f}, 1, 2));
    leopard.push_back(createLeopard({888.f, 80.f}, 1, 2));
    leopard.push_back(createLeopard({950.f, 112.f}, 1, 2));
    zombiesSpawner.push_back(ZombieSpawner({1300.f, 176.f}, {50.f, 50.f}, 1, 2));

    // LEVEL 1 - STAGE 3
    bat.push_back(createBatSpawner({136.f, 112.f}, {50.f, 50.f}, 1, 3));
    bat.push_back(createBatSpawner({395.f, 112.f}, {50.f, 50.f}, 1, 3));

    // LEVEL 1 - STAGE 5
    zombiesSpawner.push_back(ZombieSpawner({203.f, 176.f}, {50.f, 50.f}, 1, 5));

    // Whip ----------------------------------------------------------------
    sf::Image whipImage;
    if (!whipImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    whipImage.createMaskFromColor(gColorKeyGrey);
    whipImage.createMaskFromColor(gColorKeyGreen);
    gTextures["whip"] = sf::Texture(whipImage, false);

    auto whipSprite = std::make_shared<sf::Sprite>(gTextures["whip"]);
    whipSprite->setTextureRect(sf::IntRect({1, 477}, {8, 32}));
    whipSprite->setPosition({245.f, 171.f});

    whipSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y});

    player.whip.sprite = whipSprite;
    player.whip.hitboxes.push_back(player.whip.sprite.get()->getLocalBounds());

    // Inicializar AnimationManager ----------------------------------------------------------------
    AnimationManager *whipAnimationManager = new AnimationManager(*player.whip.sprite);

    if (!whipAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Whip AnimationManager!" << std::endl;
    }
    whipAnimationManager->addAnimation(whipLvl1StandingJumping, player.whip.lvl1Frames, false);

    // Player and whip manage its animations so they don't have to be managed outside
    player.animationManager = animationManager;
    player.whip.animationManager = whipAnimationManager;

    // --------------------------------------------------
    // GUI
    // --------------------------------------------------
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cout << "No se ha encontrado la fuente" << std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

    float margin = gGUI_size_x * gGUI_MarginFactor;

    // Score
    sf::Text scoreText(font, "SCORE-000000", gGUI_text_size);
    scoreText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(margin, margin + gGUI_position_y));
    scoreText.setPosition(textPositions.back());

    // Time
    sf::Text timeText(font, "TIME 300", gGUI_text_size);
    timeText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(gGUI_size_x * gGUI_TimePositionXFactor, margin + gGUI_position_y));
    timeText.setPosition(textPositions.back());

    // Stage
    sf::Text stageText(font, "STAGE 01", gGUI_text_size);
    stageText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(gGUI_size_x * gGUI_StagePositionXFactor, margin + gGUI_position_y));
    stageText.setPosition(textPositions.back());

    // Player
    sf::Text playerText(font, "PLAYER", gGUI_text_size);
    playerText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(margin, (margin + gGUI_PlayerPositionYFactor) + gGUI_position_y));
    playerText.setPosition(textPositions.back());

    // Enemy
    sf::Text enemyText(font, "ENEMY", gGUI_text_size);
    enemyText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(margin, (margin + gGUI_EnemyPositionYFactor) + gGUI_position_y));
    enemyText.setPosition(textPositions.back());

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
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.update(deltaTime, player.gPlayerActivationZone, player.gPlayerDeactivationZone);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard.level == currentLevel && leopard.stage == currentStage)
        {
            leopard.update(deltaTime, player.gPlayerActivationZone, player.gPlayerDeactivationZone, player.sprite->getPosition());
        }
    }
    for (auto &bat : bat)
    {
        if (bat.level == currentLevel && bat.stage == currentStage)
        {
            bat.update(deltaTime, player.gPlayerActivationZone, player.gPlayerDeactivationZone);
        }
    }

    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    // Reduce time every second
    if (timeAccumulator >= 1.0f)
    {
        if (time > 0)
            time -= static_cast<int>(timeAccumulator);
        if (time < 0)
            time = 0;
        timeAccumulator = 0.0f;
        texts[1].setString("TIME " + std::to_string(time));
    }

    // Update score
    std::stringstream scoreStream;
    scoreStream << "SCORE-" << std::setw(6) << std::setfill('0') << player.score; // Format with zeroes
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
            window.draw(FloatRectToRectShape(player.whip.sprite->getGlobalBounds()));
        }

        // =========================================
        // ================== GUI ==================
        // =========================================

        sf::Vector2f virtualCoordOfUpperLeftCornerOfGame = getVirtualUpperLeftCornerCoordOfGameView(window);
        sf::Vector2f guiPosition(virtualCoordOfUpperLeftCornerOfGame);
        
        // Draw the black rectangle
        sf::RectangleShape guiBackground(sf::Vector2f(gGUI_size_x, gGUI_size_y));
        guiBackground.setFillColor(gGUI_color);
        guiBackground.setPosition(guiPosition);
        window.draw(guiBackground);

        // Draw the GUI texts
        sf::Vector2f virtualWorldOffset(virtualCoordOfUpperLeftCornerOfGame.x - gGUI_position_x,
                                        virtualCoordOfUpperLeftCornerOfGame.y - gGUI_position_y);
        
        for (int i = 0; i < static_cast<int>(texts.size()); ++i) {
            sf::Text& text = texts[i];
            sf::Vector2f& pos = textPositions[i];
            text.setPosition(pos + virtualWorldOffset);
            window.draw(text);
        }

        // Draw the health bars
        drawHealthBars(window, player.health, 16, virtualWorldOffset); // CHANGE FOR BOSS HEALTH!!!!!



        // PLAYER and ENTITIES =====================================

        player.draw(window);

        for (auto &zombieSpawner : zombiesSpawner)
        {
            if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
            {
                zombieSpawner.draw(window, true);
            }
        }
        for (auto &leopard : leopard)
        {
            if (leopard.level == currentLevel && leopard.stage == currentStage)
            {
                leopard.draw(window, true);
            }
        }
        for (auto &bat : bat)
        {
            if (bat.level == currentLevel && bat.stage == currentStage)
            {
                bat.draw(window, true);
            }
        }

        window.draw(FloatRectToRectShape(player.gPlayerActivationZone));
        window.draw(FloatRectToRectShape(player.gPlayerDeactivationZone));
    }
}

void Game::drawHealthBars(sf::RenderWindow &window, int playerHealth, int bossHealth, sf::Vector2f virtualWorldset)
{
    const int MAX_HEALTH = 16;
    const int SEGMENT_WIDTH = 5;
    const int SEGMENT_HEIGHT = 6;
    const int SPACING = 2;
    const float BORDER_THICKNESS = 1.0f;
    const sf::Vector2f PLAYER_POS(gGUI_PlayerHpBar_position_x + virtualWorldset.x,
                                  gGUI_PlayerHpBar_position_y + virtualWorldset.y);
    const sf::Vector2f BOSS_POS(gGUI_BossHpBar_position_x + virtualWorldset.x,
                                gGUI_BossHpBar_position_y + virtualWorldset.y);

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
        segment.setPosition(sf::Vector2f(PLAYER_POS.x + i * (SEGMENT_WIDTH + SPACING) + xOffset, PLAYER_POS.y + yOffset));

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
        segment.setPosition(sf::Vector2f(BOSS_POS.x + i * (SEGMENT_WIDTH + SPACING) + xOffset, BOSS_POS.y + yOffset));

        segment.setOutlineThickness(BORDER_THICKNESS);
        segment.setOutlineColor(isFull ? sf::Color::Black : sf::Color::White);
        segment.setFillColor(isFull ? sf::Color(255, 150, 120) : sf::Color::Black);

        window.draw(segment);
    }
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

    // TO-DO: ESTAS BOUNDS DEBERÍAN SER CON LAS HITBOXES NO CON LOS SPRITES
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

    sf::FloatRect whipBounds = player.whip.sprite->getGlobalBounds();

    for (auto &zombieSpawner : zombiesSpawner)
    {
        if (zombieSpawner.level == currentLevel && zombieSpawner.stage == currentStage)
        {
            zombieSpawner.checkCollisions(whipBounds, tilemaps[currentStage], player.isAttacking, player.damage);
        }
    }
    for (auto &leopard : leopard)
    {
        if (leopard.level == currentLevel && leopard.stage == currentStage)
        {
            leopard.checkCollisions(playerBounds, whipBounds, tilemaps[currentStage], player.isAttacking, player.damage);
        }
    }
    for (auto &bat : bat)
    {
        if (bat.level == currentLevel && bat.stage == currentStage)
        {
            bat.checkCollisions(playerBounds, whipBounds, player.isAttacking, player.damage);
        }
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
    //std::cout << "Player: " << playerBounds.position.x << ", " << playerBounds.position.y << ", " << playerBounds.size.x << ", " << playerBounds.size.y << std::endl;
    bool hasCollided = false;

    // Solid tiles
    for (int col = 0; col < tilemaps[currentStage].m_tilesPerRow; ++col)
    {
        for (int row = 0; row < tilemaps[currentStage].m_tilesPerColumn; ++row)
        {
            // if (tilemaps[currentStage].m_solidTiles[row][col].isVisible)
            // {
            for(auto tileBounds : tilemaps[currentStage].m_solidTiles[row][col].hitboxes){
                sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

                // if (row == 5 && col == 3) {
                //     std::cout << "Tile: " << tileBounds.position.x << ", " << tileBounds.position.y << ", " << tileBounds.size.x << ", " << tileBounds.size.y << std::endl;
                //     std::cout << "Player: " << playerBounds.position.x << ", " << playerBounds.position.y << ", " << playerBounds.size.x << ", " << playerBounds.size.y << std::endl;
                // }
                if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBounds))
                {
                    const float overlapX = intersection->size.x;
                    const float overlapY = intersection->size.y;
                    //std::cout << "Overlap: " << overlapX << ", " << overlapY << std::endl;

                    if (overlapX < overlapY)
                    { // Horizontal collision
                        //std::cout << "COLISION HORIZONTAL" << std::endl;
                        if ((playerBounds.position.x + playerBounds.size.x * 0.5f) < (tileBounds.position.x + tileBounds.size.x * 0.5f))
                        {
                            player.sprite->move({-overlapX, 0.f});
                            playerBounds.position.x -= overlapX;
                        }
                        else
                        {
                            player.sprite->move({overlapX, 0.f});
                            playerBounds.position.x += overlapX;
                        }
                    }
                    else
                    { // Vertical collision
                        //std::cout << "COLISION VERTICAL" << std::endl;
                        if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
                        { // Simon's feet are collisioning with the tile

                            if (!player.isOnGround && player.verticalSpeed >= 0.0f)
                            { // If player is NOT going up
                                player.sprite->move({0.f, -overlapY});
                                player.verticalSpeed = 0.0f; // (For security) Simon stops falling
                                player.isOnGround = true; // Set Simon to be on ground
                                playerBounds.position.y -= overlapY;
                            }
                        }
                        else // Simon's head is collisioning with the tile
                        {
                            player.sprite->move({0.f, overlapY});
                            playerBounds.position.y += overlapY;
                            player.verticalSpeed = 0.0f; // (For security) Simon starts falling
                        }
                    }
                    
                    hasCollided = true;
                }
            }
        }
    }

    if (!hasCollided)
    { // If Simon is not colliding with any solid tile
        player.isOnGround = false;
        //std::cout << "NO COLISIONS" << std::endl;
    }

    // Door tiles
    // i+1 = stage number
    for (auto &doorEntry : tilemaps[currentStage].m_doorTiles)
    {
        sf::FloatRect doorBounds = doorEntry.second.hitboxes[0];

        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(doorBounds))
        {

            int doorId = doorEntry.first;

            if (int(currentStage) == tilemaps.doors[doorId].prev_stage)
            {
                std::cout << "NEXT STAGE" << std::endl;
                isLoading = true;

                if (tilemaps.doors[doorId].type == DoorTile::Type::STAIRS)
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

                if (tilemaps.doors[doorId].type == DoorTile::Type::STAIRS)
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

// -------------------------------------------------------------------------------------
//                                    DEBUG TILEMAP
// -------------------------------------------------------------------------------------

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
        player.sprite->setPosition(tilemaps[currentStage].initialPosition);
    }
    else
    {
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
