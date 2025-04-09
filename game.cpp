#include "game.h"
#include <iostream>
#include <cmath>
#include <optional>
#include <sstream>
#include "globals.h"
#include "utils.h"
#include "configManager.h"

std::unordered_map<std::string, sf::Texture> gTextures;
std::vector<sf::Sprite> gameSprites;

// Constructor, destructor
Game::Game()
{
    Player player;
    tilemaps = TilemapManager();
    collisionGrid = CollisionGrid();
}

// Initializes a new game from the beggining
void Game::init()
{
    currentLevel = gStartingLevel;
    currentStage = gStartingStage;

    configManager &configManager = configManager::getInstance();
    auto audio = configManager.getAudio();

    gameSoundManager.loadMusic("gameMusic", "./assets/music/03Vampire_Killer.mp3");
    gameSoundManager.playMusic("gameMusic", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));

    tilemaps.loadLevel(currentLevel);

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
    animationManager->addAnimation(hurtSimon, player.hurtFrames);
    animationManager->addAnimation(deathSimon, player.deadFrames, false);
    // animationManager->addAnimation(invulnerableSimon,player.invulnerableFrames,false);
    animationManager->playAnimation(idleSimon);
    player.currentAnimation = idleSimon;

    // Enemies -------------------------------------------------------------

    enemyManager = new EnemyManager(&player);
    enemyManager->loadEnemiesFromLevel(1, tilemaps);

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

    // Inicializar whip AnimationManager ----------------------------------------------------------------
    AnimationManager *whipAnimationManager = new AnimationManager(*player.whip.sprite);

    if (!whipAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Whip AnimationManager!" << std::endl;
    }
    whipAnimationManager->addAnimation(whipLvl1StandingJumping, player.whip.lvl1Frames, false);
    whipAnimationManager->addAnimation(whipNoAttack, player.whip.noAttackFrames, false);

    // Player and whip manage its animations so they don't have to be managed outside
    player.animationManager = animationManager;
    player.whip.animationManager = whipAnimationManager;

    player.whip.animationManager->playAnimation(whipNoAttack);

    // Secondary weapons ----------------------------------------------------------------
    // Load knife image
    sf::Image knifeImage;
    if (!knifeImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error loading knife image" << std::endl;
    }
    knifeImage.createMaskFromColor(gColorKeyGrey);
    knifeImage.createMaskFromColor(gColorKeyGreen);
    gTextures["knife"] = sf::Texture(knifeImage, false);

    // Create knife sprite
    auto knifeSprite = std::make_shared<sf::Sprite>(gTextures["knife"]);
    knifeSprite->setTextureRect(sf::IntRect({354, 477}, {16, 16}));

    // Set up the subweapon (knife)
    player.subWeapon.sprite = knifeSprite;
    player.subWeapon.hitboxes.push_back(player.subWeapon.sprite->getLocalBounds());

    // Initialize knife AnimationManager
    AnimationManager *knifeAnimationManager = new AnimationManager(*player.subWeapon.sprite);
    if (!knifeAnimationManager)
    {
        std::cerr << "Error: Failed to initialize Knife AnimationManager!" << std::endl;
    }

    // Add animations (similar to whip)
    knifeAnimationManager->addAnimation(knifeThrowing, player.subWeapon.knifeFrames, false);
    knifeAnimationManager->addAnimation(knifeIdle, player.subWeapon.knifeFrames, false);

    // Assign animation managers
    player.subWeapon.animationManager = knifeAnimationManager;
    player.subWeapon.animationManager->playAnimation(knifeIdle);

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
    sf::Text timeText(font, "TIME   0300", gGUI_text_size);
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

    sf::Image heartImage;
    if (!heartImage.loadFromFile("./assets/sprites/items/itemsObjects.png"))
    {
        std::cerr << "Error loading heart image" << std::endl;
    }
    heartImage.createMaskFromColor(gColorKeyGrey);

    gTextures["heart"] = sf::Texture(heartImage, false);
    
    sf::Sprite heartSprite(gTextures["heart"], sf::IntRect({18, 1}, {8, 8}));
    heartSprite.setPosition(sf::Vector2f(180, -4));
    gameSprites.push_back(heartSprite);

    // Hearts
    sf::Text hearts(font, "-00", gGUI_text_size);
    hearts.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(189, -4));
    hearts.setPosition(textPositions.back());

    // Lives
    sf::Text lives(font, "P-03", gGUI_text_size);
    lives.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(181, 6));
    lives.setPosition(textPositions.back());

    texts.push_back(scoreText);
    texts.push_back(timeText);
    texts.push_back(stageText);
    texts.push_back(playerText);
    texts.push_back(enemyText);
    texts.push_back(hearts);
    texts.push_back(lives);

    startStage(currentStage);
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event)
{
    player.handleInput(event);
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime, const sf::Vector2f& viewPosition)
{
    // std::cout << player.getBounds().position.x << ", " << player.getBounds().position.y << std::endl;
    player.update(deltaTime);

    enemyManager->update(deltaTime, currentLevel, currentStage);

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
        std::stringstream timeStream;
        timeStream << "TIME   " << std::setw(4) << std::setfill('0') << std::to_string(time);
        texts[1].setString(timeStream.str());
    }

    // Update score
    std::stringstream scoreStream;
    scoreStream << "SCORE-" << std::setw(6) << std::setfill('0') << player.score; // Format with zeroes
    texts[0].setString(scoreStream.str());

    // Update hearts
    std::stringstream heartsStream;
    heartsStream << "-" << std::setw(2) << std::setfill('0') << player.hearts; // Format with zeroes
    texts[5].setString(heartsStream.str());

    // Update lives
    std::stringstream livesStream;
    livesStream << "P-" << std::setw(2) << std::setfill('0') << player.lives; // Format with zeroes
    texts[6].setString(livesStream.str());

    // Cuando esté implementado collisionGrid, cambiar la función existente por la nueva:
    //checkCollisions(viewPosition);
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
        sf::RectangleShape guiBackground(sf::Vector2f(gGUI_size_x + 10, gGUI_size_y));
        guiBackground.setFillColor(gGUI_color);
        guiBackground.setPosition(guiPosition);
        window.draw(guiBackground);

        // Draw the GUI texts
        sf::Vector2f virtualWorldOffset(virtualCoordOfUpperLeftCornerOfGame.x - gGUI_position_x,
                                        virtualCoordOfUpperLeftCornerOfGame.y - gGUI_position_y);

        for (int i = 0; i < static_cast<int>(texts.size()); ++i)
        {
            sf::Text &text = texts[i];
            sf::Vector2f &pos = textPositions[i];
            text.setPosition(pos + virtualWorldOffset);
            window.draw(text);
        }

        // Draw the health bars
        drawHealthBars(window, player.health, 16, virtualWorldOffset); // CHANGE FOR BOSS HEALTH!!!!!

        // PLAYER and ENTITIES =====================================

        player.draw(window);

        enemyManager->draw(window, currentLevel, currentStage);

        window.draw(FloatRectToRectShape(player.gPlayerActivationZone));
        window.draw(FloatRectToRectShape(player.gPlayerDeactivationZone));

        // Red rectangle
        sf::RectangleShape redBorder(sf::Vector2f(27, 17));
        redBorder.setPosition(sf::Vector2f(140 + virtualWorldOffset.x, -4 + virtualWorldOffset.y));
        redBorder.setFillColor(sf::Color::Transparent);
        redBorder.setOutlineColor(sf::Color::Red);
        redBorder.setOutlineThickness(2.f); // Puedes ajustar el grosor del borde
        window.draw(redBorder);

        // For now the heart
        sf::Sprite sprite(gameSprites[0]);
        sf::Vector2f pos(180, -4);
        sprite.setPosition(pos + virtualWorldOffset);
        window.draw(sprite);

        //collisionGrid.drawCells(window, virtualCoordOfUpperLeftCornerOfGame);
    }
}

void Game::drawHealthBars(sf::RenderWindow &window, int playerHealth, int bossHealth, sf::Vector2f virtualWorldset)
{
    const int MAX_HEALTH = 16;
    const float SEGMENT_WIDTH = 3.5;
    const float SEGMENT_HEIGHT = 6;
    const float SPACING = 1.2;
    const float BORDER_THICKNESS = 1.f;
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

    enemyManager->checkCollisions(currentLevel, currentStage, tilemaps);
}

// vvvvvvv NO BORRAR vvvvvvv

void Game::checkCollisions(const sf::Vector2f &viewPosition)
{
    // Descomentar cuando esté implementado y borrar el checkCollisions antiguo
    allEntities.clear();

    // Add player and weapons
    allEntities.push_back(&player);
    allEntities.push_back(&player.whip);
    allEntities.push_back(&player.subWeapon);

    // Add tiles
    /*
    for (auto& solidTileRow : tilemaps[currentStage].m_solidTiles){
        for (auto& solidTile : solidTileRow){
            allEntities.push_back(&solidTile);
        }
    }
    for (auto& doorTile : tilemaps[currentStage].m_doorTiles) allEntities.push_back(&doorTile.second);
    for (auto& breakableTile : tilemaps[currentStage].m_breakableTiles) allEntities.push_back(&breakableTile);


    // Add enemies
    for (auto& spawner : zombiesSpawner){
        for (auto& zombie : spawner.zombies){
            allEntities.push_back(&zombie);
        }
    }
    for (auto& leo : leopard) allEntities.push_back(&leo);
    for (auto& b : bat) allEntities.push_back(&b);
    */
    // for (auto& item : items) allEntities.push_back(&item);

    // ... ADD THE REST OF ENTITIES

    collisionGrid.checkCollisions(allEntities, viewPosition);
    checkPlayerTileCollisions(); // quitar
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

void Game::computePlayerTileIntersection(bool& hasCollided, const sf::FloatRect &tileBounds)
{
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();
    if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBounds))
    {
        hasCollided = true;

        const float overlapX = intersection->size.x;
        const float overlapY = intersection->size.y;
        //std::cout << "Overlap: " << overlapX << ", " << overlapY << std::endl;

        if (overlapX < overlapY)    // Horizontal collision
        { 
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
        else    // Vertical collision
        { 
            if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
            { // Simon's feet are collisioning with the tile

                if (!player.isOnGround && player.verticalSpeed >= 0.0f)
                // if (player.verticalSpeed >= 0.0f)        // CUANDO ESTÉN TODO CON HITBOXES BUENAS
                { // If player is NOT going up

                    // Option 1: adjust overlapedY and make it be .15f
                    // float theoreticallyCorrectPositionY = player.sprite->getPosition().y - overlapY;
                    // float targetPositionY = static_cast<int>(theoreticallyCorrectPositionY) + gSimonFeetCollisionNewHeight;
                    // float moveY = theoreticallyCorrectPositionY - targetPositionY;

                    // Option 2: read the tile and place the player on top of it, regardless of anything else
                    float targetPositionY = tileBounds.position.y - playerBounds.size.y + gSimonFeetCollisionNewHeight;
                    float moveY = targetPositionY - playerBounds.position.y;

                    player.sprite->move({0.f, moveY});
                    playerBounds.position.y += moveY;
                    player.verticalSpeed = 0.0f; // (For security) Simon stops falling
                    player.isOnGround = true;    // Set Simon to be on ground
                }
            }
            else // Simon's head is collisioning with the tile
            {
                player.sprite->move({0.f, overlapY});
                playerBounds.position.y += overlapY;
                player.verticalSpeed = 0.0f; // (For security) Simon starts falling
            }
        }
    }
}

void Game::checkPlayerTileCollisions()
{
    bool hasCollided = false;

    // std::cout << "Player: " << playerBounds.position.x << ", " << playerBounds.position.y << ", " << playerBounds.size.x << ", " << playerBounds.size.y << std::endl;
    // std::cout << "Tile: " << tileBounds.position.x << ", " << tileBounds.position.y << ", " << tileBounds.size.x << ", " << tileBounds.size.y << std::endl;
    // std::cout << "Ground: " << player.isOnGround << std::endl;

    // Solid tiles
    if (!tilemaps[currentStage].m_solidTileHitboxes.empty())    // Optimized hitboxes
    {
        for (sf::FloatRect tileBounds : tilemaps[currentStage].m_solidTileHitboxes)
        {
            computePlayerTileIntersection(hasCollided, tileBounds);
        }
    }
    else
    {
        for (int col = 0; col < tilemaps[currentStage].m_tilesPerRow; ++col)
        {
            for (int row = 0; row < tilemaps[currentStage].m_tilesPerColumn; ++row)
            {
                for (sf::FloatRect tileBounds : tilemaps[currentStage].m_solidTiles[row][col].hitboxes)
                {
                    // ignore hitboxless tiles
                    if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) continue;

                    computePlayerTileIntersection(hasCollided, tileBounds);
                }
            }
        }
    }

    if (!hasCollided)
    { // If Simon is not colliding with any solid tile
        player.isOnGround = false;
        // std::cout << "NO COLISIONS" << std::endl;
    }

    // Door tiles
    // i+1 = stage number
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();
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
                enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);
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
