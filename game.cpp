#include <iostream>
#include <cmath>
#include <optional>
#include <sstream>
#include "game.h"
#include "globals.h"
#include "utils.h"
#include "item.h"

std::unordered_map<std::string, sf::Texture> gTextures;


// Constructor, destructor
Game::Game() : configManager(configManager::getInstance())
{
    Player player;
    tilemaps = TilemapManager();
    collisionGrid = CollisionGrid(this);
}

// Initializes a new game from the beggining
void Game::init()
{
    currentLevel = gStartingLevel;
    currentStage = gStartingStage;

    gameSoundManager.loadMusic("vampireKiller", "./assets/music/03Vampire_Killer.mp3");
    gameSoundManager.loadMusic("deadMusic", "./assets/music/13Player_Miss.mp3");

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
    // Load subweapon weapon image
    sf::Image subweaponImage;
    if (!subweaponImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error loading subweapon image" << std::endl;
    }
    subweaponImage.createMaskFromColor(gColorKeyGrey);
    subweaponImage.createMaskFromColor(gColorKeyGreen);
    gTextures["subweapon"] = sf::Texture(subweaponImage, false);

    // Create subweapon sprite
    auto subweaponSprite = std::make_shared<sf::Sprite>(gTextures["subweapon"]);
    subweaponSprite->setTextureRect(sf::IntRect({354, 477}, {16, 16}));
    //subweaponSprite->setTextureRect(sf::IntRect({515, 477}, {16, 16}));
    // Set up the subweapon (subweapon)
    player.subWeapon.sprite = subweaponSprite;
    player.subWeapon.hitboxes.push_back(player.subWeapon.sprite->getLocalBounds());

    // Initialize subweapon AnimationManager
    AnimationManager *subweaponAnimationManager = new AnimationManager(*player.subWeapon.sprite);
    if (!subweaponAnimationManager)
    {
        std::cerr << "Error: Failed to initialize subweapon AnimationManager!" << std::endl;
    }

    // Add animations (similar to whip)
    subweaponAnimationManager->addAnimation(axeThrowing, player.subWeapon.axeFrames);
    subweaponAnimationManager->addAnimation(daggerThrowing, player.subWeapon.daggerFrames, false);
    
    subweaponAnimationManager->addAnimation(fireBombThrowing, player.subWeapon.firebombFrames, false);
    //subweaponAnimationManager->addAnimation(stunThrowing, player.subWeapon.stopwatchFrames, false);
    subweaponAnimationManager->addAnimation(boomerangThrowing, player.subWeapon.boomerangFrames, false);

    // Assign animation managers
    player.subWeapon.animationManager = subweaponAnimationManager;
    //player.subWeapon.animationManager->playAnimation(daggerThrowing);

    // Load item textures
    if (!loadItemTextures())
    {
        throw std::runtime_error("Failed to load item textures.");
    }

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
    guiHeartSprite = std::make_shared<sf::Sprite>(heartSprite);


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
    player.setState(std::make_unique<PlayerIdleState>());

    // GUI subweapon
    guiSubWeaponSprite = std::make_shared<sf::Sprite>(*itemTextures[ItemType::DAGGER]);
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event)
{
    player.handleInput(event);
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime, const sf::Vector2f &viewPosition)
{   
    // std::cout << player.getBounds().position.x << ", " << player.getBounds().position.y << std::endl;
    player.update(deltaTime, viewPosition);

    enemyManager->update(deltaTime, currentLevel, currentStage);

    tilemaps[currentStage].updateItems(deltaTime);

    static float timeAccumulator = 0.0f;
    timeAccumulator += deltaTime;

    // Reduce time every second
    if (timeAccumulator >= 1.0f)
    {
        if (time > 0)
            time -= static_cast<int>(timeAccumulator);
        if (time == 0 && !player.isDead){
            player.setState(std::make_unique<PlayerDeadState>());
        }

      
        timeAccumulator = 0.0f;
        updateGUITime();
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
    checkCollisions(viewPosition);
    //checkCollisions();

    // Update the subweapon sprite item
    guiSubWeaponSprite->setTexture(*itemTextures[player.subWeaponType]);

    if (isLoading)
    {
        player.setState(std::make_unique<PlayerIdleState>());

        // player.isOnGround = false;
    }

    if (player.deathRestart)
    {
        player.deathRestart = false;
        revivingClock.restart();
        gameSoundManager.stopAllMusic();
        auto audio = configManager.getAudio();
        gameSoundManager.playMusic("deadMusic", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
    }
    
    if (player.isDead && revivingClock.getElapsedTime().asSeconds() > gRevivingTime)
    {
        player.lives -= 1;
        std::cout << "Player lives: " << player.lives << std::endl;
        player.health = player.maxHealth;
        player.setState(std::make_unique<PlayerIdleState>());
        isLoading = true;
        if(player.lives >= 0){
            restartStage();
        } else {
            restartLevel();
        }
    }
}

void Game::updateGUITime()
{
    std::stringstream timeStream;
    timeStream << "TIME   " << std::setw(4) << std::setfill('0') << std::to_string(time);
    texts[1].setString(timeStream.str());
}


// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow &window, Camera &camera)
{

    if (isLoading)
    {
        isLoading = false;
        loadingClock.restart();
    }

    if (loadingClock.getElapsedTime().asSeconds() < gLoadingTime)
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

        // Subweapon box (the red rectangle)
        sf::RectangleShape redBorder(sf::Vector2f(gGUI_subweaponBox_size_x, gGUI_subweaponBox_size_y));
        sf::Vector2f redBorderPosition(gGUI_subweaponBox_offset_position_x + virtualWorldOffset.x,
                                       gGUI_subweaponBox_offset_position_y + virtualWorldOffset.y);
        redBorder.setPosition(redBorderPosition);
        redBorder.setFillColor(sf::Color::Transparent);
        redBorder.setOutlineColor(sf::Color::Red);
        redBorder.setOutlineThickness(1.f);
        window.draw(redBorder);

        // Draw the subweapon in the box
        if (player.subWeaponType != ItemType::NONE) {
            redBorderPosition.x += 5.f;
            //redBorderPosition.y -= 2.f;
            guiSubWeaponSprite->setPosition(redBorderPosition);
            window.draw(*guiSubWeaponSprite);
        }

        // Draw the heart counter icon
        guiHeartSprite->setPosition(sf::Vector2f(gGUI_heartCounter_position_x, gGUI_heartCounter_position_y) + virtualWorldOffset);
        window.draw(*guiHeartSprite);


        // PLAYER and ENTITIES =====================================

        player.draw(window);

        enemyManager->draw(window, currentLevel, currentStage);

        // window.draw(FloatRectToRectShape(player.gPlayerActivationZone));
        // window.draw(FloatRectToRectShape(player.gPlayerDeactivationZone));

        // collisionGrid.drawCells(window, virtualCoordOfUpperLeftCornerOfGame);
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
    checkItemsCollisions();

    checkPlayerMapBoundCollisions();

    checkPlayerCollisions();

    enemyManager->checkCollisions(currentLevel, currentStage, tilemaps);
}

void Game::checkCollisions(const sf::Vector2f &viewPosition)
{   
    // 1. Add tiles (static entities)
    staticEntities.clear();

    for (auto &doorTile : tilemaps[currentStage].m_doorTiles)
        staticEntities.push_back(&doorTile);
    for (auto &breakableTile : tilemaps[currentStage].m_breakableTiles)
        staticEntities.push_back(&breakableTile);

    // 2. Add everything else (dynamic entities)
    dynamicEntities.clear();

    //      Add player and weapons
    dynamicEntities.push_back(&player);
    dynamicEntities.push_back(&player.whip);
    dynamicEntities.push_back(&player.subWeapon);

    //      Add enemies
    for(auto& enemy : enemyManager->getEnemies(currentLevel, currentStage)) dynamicEntities.push_back(enemy);
    
    for(auto& item : tilemaps[currentStage].m_items) dynamicEntities.push_back(item.get());

    // ... ADD THE REST OF ENTITIES


    // Cálculo aparte porque no entra en CollisionGrid
    checkPlayerMapBoundCollisions();
    checkSolidTileCollisions(dynamicEntities);
    
    collisionGrid.checkCollisions(staticEntities, dynamicEntities, viewPosition);
}

void Game::checkSolidTileCollisions(std::vector<Entity*> &dynamicEntities){

    for(auto& e : dynamicEntities){
        for (auto& solidTileRow : tilemaps[currentStage].m_solidTiles){
            for (auto &t : solidTileRow){
                if (checkIntersections(*e, t)) {
                    e->onCollision(t, *this);
                    t.onCollision(*e, *this);
                }
            }
        }
    }
}


void Game::checkItemsCollisions()
{
    for (auto &item : tilemaps[currentStage].m_items)
    {
        int maxCollisions = 2;

        if (item->m_isOnGround)
            continue; // Skip if item is already on ground

        for (int col = 0; col < tilemaps[currentStage].m_tilesPerRow && maxCollisions > 0; ++col)
        {
            for (int row = 0; row < tilemaps[currentStage].m_tilesPerColumn && maxCollisions > 0; ++row)
            {
                sf::FloatRect tileBounds = tilemaps[currentStage].m_solidTiles[row][col].hitboxes[0];

                // ignore hitboxless tiles
                if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f)
                    continue;

                sf::FloatRect itemBounds = item->sprite->getGlobalBounds();
                if (const std::optional<sf::FloatRect> intersection = itemBounds.findIntersection(tileBounds))
                {
                    maxCollisions--;
                    const float overlapX = intersection->size.x;
                    const float overlapY = intersection->size.y;

                    if (overlapX >= overlapY) // Vertical collision
                    {
                        if ((itemBounds.position.y + itemBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
                        { // Bottom collision
                            item->sprite->move({0.f, -overlapY});
                            itemBounds.position.y -= overlapY;
                            item->m_isOnGround = true; // Set item to be on ground
                        }
                    }
                }
            }
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

void Game::computePlayerTileIntersection(bool &hasCollided, const sf::FloatRect &tileBounds)
{
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();
    if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(tileBounds))
    {
        hasCollided = true;

        const float overlapX = intersection->size.x;
        const float overlapY = intersection->size.y;

        // Mejora para no mover a Simon horizontalmente si tiene el 99% de su cuerpo sobre un tile
        bool simonIsTouchingGround = false;
        if (overlapY < 7.66f) {
            simonIsTouchingGround = ((playerBounds.position.y + playerBounds.size.y) < (tileBounds.position.y + tileBounds.size.y));
                
        }
        if (overlapX < overlapY && (! simonIsTouchingGround)) // Horizontal collision
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
        else // Vertical collision
        {
            if ((playerBounds.position.y + playerBounds.size.y * 0.5f) < (tileBounds.position.y + tileBounds.size.y * 0.5f))
            { // Simon's feet are collisioning with the tile
                if (player.verticalSpeed >= 0.0f)
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
                if (overlapY >= 1.f){
                    player.sprite->move({0.f, overlapY});
                    playerBounds.position.y += overlapY;
                    player.verticalSpeed = 0.0f; // (For security) Simon starts falling
                }
            }
        }
    }
}

void Game::checkPlayerCollisions()
{
    bool hasCollided = false;

    // std::cout << "Player: " << playerBounds.position.x << ", " << playerBounds.position.y << ", " << playerBounds.size.x << ", " << playerBounds.size.y << std::endl;
    // std::cout << "Tile: " << tileBounds.position.x << ", " << tileBounds.position.y << ", " << tileBounds.size.x << ", " << tileBounds.size.y << std::endl;
    // std::cout << "Ground: " << player.isOnGround << std::endl;

    // Solid tiles
    for (int col = 0; col < tilemaps[currentStage].m_tilesPerRow; ++col)
    {
        for (int row = 0; row < tilemaps[currentStage].m_tilesPerColumn; ++row)
        {
            for (sf::FloatRect tileBounds : tilemaps[currentStage].m_solidTiles[row][col].hitboxes)
            {
                // ignore hitboxless tiles
                if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f)
                    continue;

                computePlayerTileIntersection(hasCollided, tileBounds);
            }
        }
    }

    if (!hasCollided)
    { // If Simon is not colliding with any solid tile
        player.isOnGround = false;
        // std::cout << "NO COLISIONS" << std::endl;
    }

    // Simon collects items
    for (auto it = tilemaps[currentStage].m_items.begin(); it != tilemaps[currentStage].m_items.end();)
    {
        sf::FloatRect itemBounds = (*it)->sprite->getGlobalBounds();
        sf::FloatRect playerBounds = player.sprite->getGlobalBounds();

        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(itemBounds))
        {   
            handleSimonInteractionWithItem((*it)->m_type);
            it = tilemaps[currentStage].m_items.erase(it); // erase item from vector and move iterator
        }
        else
        {
            ++it; // move iterator
        }
    }

    // Breakable tiles: first we check Simon collisions
    // for (auto &breakableTile : tilemaps[currentStage].m_breakableTiles)
    // {
    //     // Breakable tiles only have 1 hitbox
    //     sf::FloatRect tileBounds = breakableTile.hitboxes[0];

    //     // ignore hitboxless tiles
    //     if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) continue;

    //     if (breakableTile.isDestroyed) continue;

    //     computePlayerTileIntersection(hasCollided, tileBounds);
    // }

    // Breakable tiles: second we check whip collisions
    if (player.isAttacking)
    {
        for (auto &breakableTile : tilemaps[currentStage].m_breakableTiles)
        {
            if (breakableTile.isDestroyed)
                continue;

            sf::FloatRect tileBounds = breakableTile.hitboxes[0]; // Breakable tiles only have 1 hitbox

            // ignore hitboxless tiles
            if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f)
                continue;

            sf::FloatRect whipBounds = player.whip.sprite->getGlobalBounds();
            if (const std::optional<sf::FloatRect> intersection = whipBounds.findIntersection(tileBounds))
            {
                if (breakableTile.isBreakable)
                {
                    breakableTile.isDestroyed = true;
                    createDropItem(breakableTile.sprite->getPosition(), breakableTile.dropType);
                }
            }
        }
    }

    // Door tiles
    // i+1 = stage number
    sf::FloatRect playerBounds = player.sprite->getGlobalBounds();
    for (auto &doorEntry : tilemaps[currentStage].m_doorTiles)
    {
        sf::FloatRect doorBounds = doorEntry.hitboxes[0];

        if (const std::optional<sf::FloatRect> intersection = playerBounds.findIntersection(doorBounds))
        {

            int doorId = doorEntry.doorId;

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

void Game::activateDoorTile(int doorId)
{
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
// -------------------------------------------------------------------------------------
//                                    AUXILIARY FUNCTIONS
// -------------------------------------------------------------------------------------

void Game::createDropItem(sf::Vector2f itemPosition, DropType dropType)
{
    std::shared_ptr<Item> item = getDropItem(dropType, itemPosition);
    if (item)
    {
        tilemaps[currentStage].m_items.push_back(std::move(item));
    }
}

// BORRAR
void Game::handleSimonInteractionWithItem(ItemType itemType)
{
    if (isSubweaponItem(itemType))
    {
        player.subWeaponType = itemType;
    }
    // else if (itemType == ItemType::MORNING_STAR)
    // {

    // }
    else if (itemType == ItemType::SMALL_HEART)
    {
        player.hearts += 1;
    }
    else if (itemType == ItemType::LARGE_HEART)
    {
        player.hearts += 5;
    }
    else if (isScoringItem(itemType))
    {
        player.score += getItemScore(itemType);
    }
    // else if (itemType == ItemType::INVISIBILITY_POTION) {
    //
    // }
    else if (itemType == ItemType::PORK_CHOP) {
        player.health += 6;
        if (player.health > player.maxHealth)
            player.health = player.maxHealth;
    }
    // else if (itemType == ItemType::DOUBLE_SHOT)
    // {
    //     player.subWeaponType = ItemType::DOUBLE_SHOT;
    // }
    // else if (itemType == ItemType::TRIPLE_SHOT)
    // {
    //     player.subWeaponType = ItemType::TRIPLE_SHOT;
    // }
    else if (itemType == ItemType::MAGIC_CRYSTAL) {
        return;
    }
    else if (itemType == ItemType::ONEUP)
    {
        player.lives += 1;
    }
}

// -------------------------------------------------------------------------------------
//                                    DEBUG TILEMAP
// -------------------------------------------------------------------------------------

int Game::startStage(int stage, int fromStairs)
{

    if (unsigned(stage) > tilemaps.tilemaps.size())
    {
        std::cerr << "ERROR: Level " << currentLevel << ", stage " << stage << " doesn't exist";
        return -1;
    }

    currentStage = stage;

    setLevelMusic(currentLevel);

    std::cout << "Current stage: " << currentStage << std::endl;
    if (fromStairs == 0)
    {
        player.sprite->setPosition(tilemaps[currentStage].initialPosition);
    }
    else
    {
        for (auto &door : tilemaps[currentStage].m_doorTiles)
        {
            if (door.doorId == fromStairs)
            {
                player.sprite->setPosition(door.playerAparition);
                break;
            }
        }
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

void Game::restartStage()
{
    std::cout << "Current stage: " << currentStage << std::endl;

    setLevelMusic(currentLevel);

    tilemaps.restartBreakableTiles();

    enemyManager->restartEnemies(currentLevel, currentStage);
    
    time = 300;
    updateGUITime();
    
    player.dir = PlayerDirection::RIGHT;
    player.health = player.maxHealth;
    player.subWeaponType = ItemType::NONE;
    player.hearts = 5;

    player.sprite->setPosition(tilemaps[currentStage].initialPosition);
}

void Game::restartLevel()
{
    std::cout << "Current stage: " << currentStage << std::endl;

    for(auto& tilemap : tilemaps.tilemaps)
    {
        tilemap.m_items.clear();

        for(auto& breakableTile: tilemap.m_breakableTiles)
        {
            breakableTile.isBreakable = true;
            breakableTile.isDestroyed = false;
        }
    }
    
    time = 300;
    updateGUITime();

    player.dir = PlayerDirection::RIGHT;
    player.health = player.maxHealth;
    player.subWeaponType = ItemType::NONE;
    player.hearts = 5;
    player.score = 0;
    player.lives = 3;
    
    startStage(1);
}

void Game::setLevelMusic(int level){
    // Music for the game
    auto audio = configManager.getAudio();
    switch (level){
    case 1:
        gameSoundManager.playMusic("vampireKiller", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        break;
    }
}