#include <iostream>
#include <cmath>
#include <optional>
#include <sstream>
#include <thread>
#include <chrono>
#include "game.h"
#include "globals.h"
#include "utils.h"
#include "item.h"

using BossPos = TileMap::BossPosition;

// Constructor, destructor
Game::Game() : configManager(configManager::getInstance())
{
    Player player;

    tilemaps = TilemapManager();
    collisionGrid = CollisionGrid(this);

    particleSystem = ParticleSystem();

    // Loads simon texture
    sf::Image simonImage;
    if (!simonImage.loadFromFile("./assets/sprites/player/simonBelmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    simonImage.createMaskFromColor(gColorKeyGrey);
    simonImage.createMaskFromColor(gColorKeyGreen);

    gTextures["simon"] = sf::Texture(simonImage, false);

    sf::Image mapBat;
    if (!mapBat.loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    mapBat.createMaskFromColor(gColorKeyGrey);
    mapBat.createMaskFromColor(gColorKeyGreen);

    gTextures["mapBat"] = sf::Texture(mapBat, false);
}

// Initializes a new game from the beggining
void Game::init()
{
    currentLevel = gStartingLevel;
    currentStage = gStartingStage;
    withOutLives = false;
    isInBossFight = false;
    goBack = false;
    goToEndAnimation = false;
    showInter = false;

    // Music and sounds
    loadMusic();
    loadSounds();

    tilemaps.loadLevel(currentLevel);
    prepareVariablesForLevel();

    // Simon ----------------------------------------------------------------
    // Load item textures
    if (!player.loadSpritesAndAnimations())
    {
        throw std::runtime_error("Failed to load player textures.");
    }

    player.sprite->setPosition(tilemaps[currentStage].initialPosition);

    // Enemies -------------------------------------------------------------
    enemyManager = new EnemyManager(&player);
    enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);

    // Bosses -------------------------------------------------------------
    bossManager = new BossManager(&player);
    bossManager->loadBossesFromLevel(currentLevel, tilemaps);

    // Load item textures
    if (!loadItemTextures())
    {
        throw std::runtime_error("Failed to load item textures.");
    }

    // ----- TRANSITION BETWEEN LEVELS -----

    // Animated sprites

    // LITTLE BAT
    auto lBat = std::make_shared<sf::Sprite>(gTextures["mapBat"]);

    lBat->setTextureRect(sf::IntRect(sf::Vector2i(271, 953), sf::Vector2i(16, 16)));

    lBat->setScale(sf::Vector2f(1, 1));

    lBat->setPosition(sf::Vector2f(0, 0));

    littleBat = lBat;

    std::vector<AnimationManager::Frame> littleFlyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(271, 953), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(288, 953), sf::Vector2(16, 16)), 0.1f}};

    littleBatManager = new AnimationManager(*littleBat);

    littleBatManager->addAnimation(liBat, littleFlyFrames, true);

    littleBatManager->playAnimation(liBat);

    // BIG BAT
    auto bBat = std::make_shared<sf::Sprite>(gTextures["mapBat"]);

    bBat->setTextureRect(sf::IntRect(sf::Vector2i(305, 953), sf::Vector2i(16, 16)));

    bBat->setScale(sf::Vector2f(1, 1));

    bBat->setPosition(sf::Vector2f(0, 0));

    bigBat = bBat;

    std::vector<AnimationManager::Frame> bigFlyFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(305, 953), sf::Vector2(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(322, 953), sf::Vector2(16, 16)), 0.1f}};

    bigBatManager = new AnimationManager(*bigBat);

    bigBatManager->addAnimation(biBat, bigFlyFrames, true);

    bigBatManager->playAnimation(biBat);

    // SIMON
    auto simonS = std::make_shared<sf::Sprite>(gTextures["simon"]);

    simonS->setTextureRect(sf::IntRect(sf::Vector2i(29, 21), sf::Vector2i(16, 32)));

    simonS->setScale(sf::Vector2f(-1, 1));

    simonS->setPosition(sf::Vector2f(500, 150));

    simon = simonS;

    simonManager = new AnimationManager(*simon);

    std::vector<AnimationManager::Frame> walkFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 21), sf::Vector2(16, 31)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 31)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 21), sf::Vector2(16, 31)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 31)), 0.1f},
    };

    simonManager->addAnimation(walkSimon, walkFrames, true);

    simonManager->playAnimation(walkSimon);

    // CUBE
    auto box = std::make_shared<sf::Sprite>(gTextures["mapBat"]);

    box->setTextureRect(sf::IntRect(sf::Vector2i(263, 962), sf::Vector2i(6, 7)));

    box->setScale(sf::Vector2f(1, 1));

    box->setPosition(sf::Vector2f(0, 0));

    cube = box;

    // MAP
    auto mapa = std::make_shared<sf::Sprite>(gTextures["mapBat"]);

    mapa->setTextureRect(sf::IntRect(sf::Vector2i(262, 808), sf::Vector2i(384, 144)));

    mapa->setScale(sf::Vector2f(1, 1));

    mapa->setPosition(sf::Vector2f(750, 30));

    map = mapa;

    switch (currentLevel)
    {
    case 1:
        time = 300;
        break;
    
    case 3:
        time = 500;
        break;
    
    case 5:
        time = 500;
        break;
    
    case 7:
        time = 700;
        break;
    }

    // Load GUI
    loadGUI();

    startStage(currentStage);
    player.setState(std::make_unique<PlayerIdleState>());
}

void Game::loadGUI()
{
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf"))
    {
        std::cout << "No se ha encontrado la fuente" << std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    float margin = gGUI_size_x * gGUI_MarginFactor;

    // Score
    sf::Text scoreText(font, "SCORE-000000", gGUI_text_size);
    scoreText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(margin, margin + gGUI_position_y));
    scoreText.setPosition(textPositions.back());

    // Time
    std::stringstream timeStream;
    timeStream << "TIME   " << std::setw(4) << std::setfill('0') << std::to_string(time);
    sf::Text timeText(font, "TIME   0300", gGUI_text_size);
    timeText.setString(timeStream.str());
    timeText.setFillColor(gGUI_text_color);
    textPositions.push_back(sf::Vector2f(gGUI_size_x * gGUI_TimePositionXFactor, margin + gGUI_position_y));
    timeText.setPosition(textPositions.back());

    // Stage
    std::stringstream stageStream;
    int stage = 0;
    switch (currentLevel)
    {
    case 1:
        stage = 1;
        break;
    
    case 3:
        stage = 6;
        break;
    
    case 5:
        stage = 11;
        break;

    case 7:
        stage = 17;
        break;
    }
    stageStream << "STAGE " << std::setw(2) << std::setfill('0') << std::to_string(stage);
    sf::Text stageText(font, "STAGE 01", gGUI_text_size);
    stageText.setString(stageStream.str());
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
    heartImage.createMaskFromColor(gColorKeyGreen);

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

    // GUI subweapon
    guiSubWeaponSprite = getItemSprite(ItemType::NONE);

    sf::Text gOver(font, "GAME OVER", 8);
    gOver.setFillColor(sf::Color::White);
    deadScreenTexts.push_back(gOver);

    sf::Text cont(font, "CONTINUE", 8);
    cont.setFillColor(sf::Color::White);
    deadScreenTexts.push_back(cont);

    sf::Text end(font, "END", 8);
    end.setFillColor(sf::Color::White);
    deadScreenTexts.push_back(end);

    position = 0;
    sf::Sprite hSprite(gTextures["heart"], sf::IntRect({18, 1}, {8, 8}));
    hSprite.setScale(sf::Vector2f(1.5, 1.5));
    gSprites.push_back(hSprite);
}

void Game::loadMusic()
{
    // Level 1
    gameSoundManager.loadMusic("vampireKiller", "./assets/music/03Vampire_Killer.mp3");

    // Level 2
    gameSoundManager.loadMusic("stalker1", "./assets/music/04Stalker(1).mp3");
    gameSoundManager.loadMusic("stalker2", "./assets/music/04Stalker(2).mp3");

    // Level 3
    gameSoundManager.loadMusic("wicked1", "./assets/music/05Wicked_Child(1).mp3");
    gameSoundManager.loadMusic("wicked2", "./assets/music/05Wicked_Child(2).mp3");

    // Level 4
    gameSoundManager.loadMusic("edge1", "./assets/music/06Walking_On_the_Edge(1).mp3");
    gameSoundManager.loadMusic("edge2", "./assets/music/06Walking_On_the_Edge(2).mp3");

    // Level 5
    gameSoundManager.loadMusic("heart1", "./assets/music/07Heart_of_Fire(1).mp3");
    gameSoundManager.loadMusic("heart2", "./assets/music/07Heart_of_Fire(2).mp3");

    // Level 6
    gameSoundManager.loadMusic("outTime", "./assets/music/08Out_of_Time.mp3");

    // Boss Music
    gameSoundManager.loadMusic("boss", "./assets/music/10Poison_Mind.mp3");
    gameSoundManager.loadMusic("victoryBoss", "./assets/music/15Stage_Clear.mp3");
    gameSoundManager.loadMusic("dracula1", "./assets/music/09Nothing_to_Lose.mp3");
    gameSoundManager.loadMusic("dracula2.1", "./assets/music/11Black_Night(1).mp3");
    gameSoundManager.loadMusic("dracula2.2", "./assets/music/11Black_Night(2).mp3");
    gameSoundManager.loadMusic("victoryDracula", "./assets/music/16All_Clear.mp3");

    // Dead/Game over
    gameSoundManager.loadMusic("deadMusic", "./assets/music/13Player_Miss.mp3");
    gameSoundManager.loadMusic("gameOver", "./assets/music/14Game_Over.mp3");
}

void Game::loadSounds()
{
    gameSoundManager.loadSound("pause", "./assets/sounds/01.wav");
    gameSoundManager.loadSound("02", "./assets/sounds/02.wav");
    gameSoundManager.loadSound("deflecting", "./assets/sounds/03.wav");
    gameSoundManager.loadSound("time_tally", "./assets/sounds/04.wav");
    gameSoundManager.loadSound("heart_tally", "./assets/sounds/05.wav");
    gameSoundManager.loadSound("end_lvl_points", "./assets/sounds/06.wav");
    gameSoundManager.loadSound("between_lvl_steps", "./assets/sounds/07.wav");
    gameSoundManager.loadSound("whip", "./assets/sounds/08.wav");
    gameSoundManager.loadSound("throw_dagger", "./assets/sounds/09.wav");
    gameSoundManager.loadSound("whip_use", "./assets/sounds/10.wav");
    gameSoundManager.loadSound("falling_stage", "./assets/sounds/11.wav");
    gameSoundManager.loadSound("falling_stage2", "./assets/sounds/12.wav");
    gameSoundManager.loadSound("fallen", "./assets/sounds/13.wav");
    gameSoundManager.loadSound("splash", "./assets/sounds/14.wav");
    gameSoundManager.loadSound("water_surface_hit", "./assets/sounds/15.wav");
    gameSoundManager.loadSound("strong_enemy_hit", "./assets/sounds/16.wav");
    gameSoundManager.loadSound("17", "./assets/sounds/17.wav");
    gameSoundManager.loadSound("18", "./assets/sounds/18.wav");
    gameSoundManager.loadSound("19", "./assets/sounds/19.wav");
    gameSoundManager.loadSound("whip_hit", "./assets/sounds/20.wav");
    gameSoundManager.loadSound("block_break", "./assets/sounds/21.wav");
    gameSoundManager.loadSound("heart_pick", "./assets/sounds/22.wav");
    gameSoundManager.loadSound("score_item_pick", "./assets/sounds/23.wav");
    gameSoundManager.loadSound("other_item_pick", "./assets/sounds/24.wav");
    gameSoundManager.loadSound("castle_entrance", "./assets/sounds/25.wav");
    gameSoundManager.loadSound("stopwatch", "./assets/sounds/26.wav");
    gameSoundManager.loadSound("invisibility_on", "./assets/sounds/27.wav");
    gameSoundManager.loadSound("invisibility_off", "./assets/sounds/28.wav");
    gameSoundManager.loadSound("door", "./assets/sounds/29.wav");
    gameSoundManager.loadSound("firebomb", "./assets/sounds/30.wav");
    gameSoundManager.loadSound("castle_collapsing", "./assets/sounds/31.wav");
    gameSoundManager.loadSound("castle_collapsing2", "./assets/sounds/32.wav");
    gameSoundManager.loadSound("low_on_time", "./assets/sounds/33.wav");
    gameSoundManager.loadSound("secret_treasure", "./assets/sounds/34.wav");
    gameSoundManager.loadSound("rosary", "./assets/sounds/35.wav");
    gameSoundManager.loadSound("one_up", "./assets/sounds/36.wav");
    gameSoundManager.loadSound("hurt", "./assets/sounds/37.wav");
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event)
{
    if (!isLoading)
    {
        if (!withOutLives && !gPlayEndLvlScoreAnimation)
        {
            if (player.acceptsInput)
                player.handleInput(event);
        }
        else
        {
            if (withOutLives)
            {
                auto controls = configManager.getControls();
                if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>())
                {
                    if (keyPressed->scancode == controls.down && position < 1)
                    {
                        position++;
                    }

                    if (keyPressed->scancode == controls.up && position > 0)
                    {
                        position--;
                    }

                    if (keyPressed->scancode == controls.enter)
                    {
                        switch (position)
                        {
                        case 0:
                            std::cout << "Let's play again" << std::endl;
                            withOutLives = false;
                            position = 0;
                            gameSoundManager.stopAllMusic();
                            setLevelMusic(currentLevel);
                            player.acceptsInput = true;
                            int numStages;
                            currentBossPhase = 0;
                            switch (currentLevel){
                                case 1:
                                    numStages = 5;
                                    break;
                                case 3:
                                    numStages = 5;
                                    break;
                                case 5:
                                    numStages = 6;
                                    break;
                                case 7:
                                    numStages = 2;
                                    break;
                                default:
                                    numStages = 6;
                                    break;
                            }
                            for(int i = 1; i <= numStages; i++){
                                enemyManager->restartEnemies(currentLevel, i);
                                bossManager->restartBosses(currentLevel, i);
                            }
                            break;
                        case 1:
                            std::cout << "Going back to the menu" << std::endl;
                            // Reset of the game for story mode
                            gStartingLevel = 1;
                            gStartingStage = 1;
                            withOutLives = false;
                            position = 0;
                            goBack = true;
                            currentBossPhase = 0;
                            enemyManager->restartEnemies(currentLevel, currentStage);
                            bossManager->restartBosses(currentLevel, currentStage);
                            // stateMachine->replaceState(std::make_unique<levelSelectorGS>(stateMachine));
                            break;
                        }
                    }

                    std::cout << "Position:" << position << withOutLives << std::endl;
                }
            }
        }
    }
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime, const sf::Vector2f &viewPosition, bool windowHasFocus)
{
    // std::cout << player.sprite->getPosition().x << "," << player.sprite->getPosition().y << std::endl; // no lo quitesis porfa (sirve para posicionar enemigos)

    if (gTriggerEndLvlScoreAnimation && !gPlayEndLvlScoreAnimation)
    { // Prepare for end level score animation

        if (!m_playedVictoryMusic)
        {
            gameSoundManager.stopAllMusic();
            auto audio = configManager.getAudio();
            if (currentLevel == 7)
            {
                gameSoundManager.playMusic("victoryDracula", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
            }
            else
            {
                gameSoundManager.playMusic("victoryBoss", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
            }
            m_playedVictoryMusic = true;
        }

        m_endLvlAnimationCoundown -= deltaTime;
        if (m_endLvlAnimationCoundown <= 0.f)
        {
            gPlayEndLvlScoreAnimation = true;
        }
    }

    if (!gPlayEndLvlScoreAnimation)
    {
        player.update(deltaTime, viewPosition, windowHasFocus);
    }
    else
    {
        player.acceptsInput = false;
        player.update(0.f, viewPosition, windowHasFocus);
    }

    enemyManager->update(deltaTime, currentLevel, currentStage, tilemaps[currentStage].getMapBounds());
    bossManager->update(deltaTime, currentLevel, currentStage, currentBossPhase, tilemaps[currentStage].getMapBounds());

    tilemaps[currentStage].updateItems(deltaTime);
    tilemaps[currentStage].updateMiscTiles(deltaTime);

    particleSystem.update(deltaTime);

    static float timeAccumulator = 0.0f;

    if (!withOutLives)
        timeAccumulator += deltaTime;

    updateGUIStage();
    if (gPlayEndLvlScoreAnimation) // Time reduction management for end level score animation
    {
        endLevelScoreAnimation(deltaTime);
    }
    else if (timeAccumulator >= 1.0f) // Default time reduction flow
    {
        if (time > 0)
            time -= static_cast<int>(timeAccumulator);
        if (time == 0 && !player.isDead)
        {
            player.setState(std::make_unique<PlayerDeadState>());
        }
        timeAccumulator = 0.0f;
        if (!showInter)
            updateGUITime();
    }

    if (gGoToNextLevel)
    {
        int aux_CurrentLevel = static_cast<int>(currentLevel);
        if(currentLevel == 7){
            std::cout << "Animation of the end of the game" << std::endl;
            goToEndAnimation = true;
            gStartingLevel = 1;
            currentLevel = gStartingLevel;
            gStartingStage = 1;
            currentStage = gStartingStage;
            time = 300; // To ensure Simon doesn't die
        }
        // Reset variables, flags and loads entities and the map to start next level
        prepareVariablesForLevel();
        updateGUITime();
        updateGUIStage();

        if (aux_CurrentLevel == 7 && gStartingLevel == 1)
            return;
    }

    if (showInter && !goToEndAnimation)
    {
        interAnimation(deltaTime);
    }
    else
    {
        if (viewPosition.y + gGameVisibleWorld_size_y + 50 < player.sprite->getPosition().y && !player.isDead && !player.upgradeWhip)
        {
            std::cout << "Falling " << std::endl;
            std::cout << viewPosition.y << ", " << gGameVisibleWorld_size_y << ", " << player.sprite->getPosition().y << std::endl;
            player.setState(std::make_unique<PlayerDeadState>());
        }

        sf::Sprite heart = gSprites.back();
        gSprites.pop_back();
        float heartX = 0;
        float heartY = 0;
        if (position <= 1)
        {
            heartX = deadScreenTexts[position + 1].getPosition().x - 15.f;
            heartY = deadScreenTexts[position + 1].getPosition().y - 3.f;
        }
        heart.setPosition(sf::Vector2f(heartX, heartY));

        gSprites.push_back(heart);

        // Update text's score
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

        checkCollisions(viewPosition);
        // checkCollisions();   // Previous function to check collisions

        // Update the subweapon sprite item
        guiSubWeaponSprite = getItemSprite(player.subWeaponType);
        

        if (player.deathRestart)
        {
            player.deathRestart = false;
            revivingClock.restart();
            gameSoundManager.stopAllMusic();
            auto audio = configManager.getAudio();

            gameSoundManager.playMusic("deadMusic", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
        }

        if (player.activateRosario)
        {
            enemyManager->restartEnemies(currentLevel, currentStage);
            isRosarioBlinking = true;
            rosarioBlinkClock.restart();
            player.activateRosario = false;
        }

        if (player.isDead && revivingClock.getElapsedTime().asSeconds() > gRevivingTime)
        {
            player.lives -= 1;
            std::cout << "Player lives: " << player.lives << std::endl;
            player.health = player.maxHealth;
            player.setState(std::make_unique<PlayerIdleState>());
            restartLoadingClock = true;

            if (player.lives >= 0)
            {
                restartStage();
            }
            else
            {
                withOutLives = true;
                player.acceptsInput = false;
                // LOGIC TO SHOW DEAD SCREEN
                // Black rectangle with 2 options and a heart as selector
                // -> RestartLevel (Continue)
                // -> Go back to Menu (End)
                restartLevel();
            }
        }

        if (beginStageEntrance)
        {

            if (player.isOnStairs)
            {
                if (player.stairStepDistance == 0.f)
                { // Wait until full step is moved
                    beginStageEntrance = false;
                    player.setState(std::make_unique<PlayerStairWalkState>());
                }
            }
            else
            {
                beginStageEntrance = false;
                player.setState(std::make_unique<PlayerIdleState>());
            }
        }

        // For killing bosses (destroying)
        if (gKilledBoss)
        {
            if (currentLevel == 1)
            {
                bossManager->killBoss(phantomBatID);
            }
            else if (currentLevel == 7 && currentBossPhase == 1)
            {
                currentBossPhase++;
                bossManager->killBoss(draculaID);
            }
            else if (currentLevel == 7 && currentBossPhase == 2)
            {
                bossManager->killBoss(draculaSpiritID);
            }
            gKilledBoss = false;
        }
    }
}

void Game::interAnimation(float deltaTime)
{
    float destiny = 0;
    sf::FloatRect pos(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
    if (currentLevel == 1)
    {
        pos = tilemaps[5].getMapBoundsBossFight();
    }
    if (currentLevel == 3)
    {
        pos = tilemaps[5].getMapBoundsBossFight();
    }
    if (currentLevel == 5)
    {
        pos = tilemaps[6].getMapBoundsBossFight();
    }
    // std::cout << pos.position.x << " " << pos.position.y << std::endl;
    // First map entering
    if (entering)
    {
        switch (currentLevel)
        {
        case 1:
            destiny = pos.position.x + 2;
            break;
        case 3:
            destiny = pos.position.x - 127;
            break;
        case 5:
            destiny = pos.position.x - 127;
            break;
        }

        sf::Vector2f posMap = map->getPosition();
        const float mapSpeed = 80.0f; // pixels per second
        std::cout << posMap.x << std::endl;
        if (posMap.x <= destiny)
        {
            map->setPosition(sf::Vector2f(destiny, posMap.y));
            entering = false; // Has reached to the x position
        }
        else
        {
            map->setPosition(sf::Vector2f(posMap.x - mapSpeed * deltaTime, posMap.y));
        }
    }
    else
    { // Show moving animated Simon, animated and placed Bat (big and little), animated and placed box
        switch (currentLevel)
        {
        case 1:
            cube->setPosition(sf::Vector2f(pos.position.x + 79, pos.position.y + 102));
            littleBat->setPosition(sf::Vector2f(pos.position.x + 167, pos.position.y + 81));
            littleBatManager->update(deltaTime);
            break;
        case 3:
            cube->setPosition(sf::Vector2f(pos.position.x + 156, pos.position.y + 106));
            littleBat->setPosition(sf::Vector2f(pos.position.x + 133, pos.position.y + 63));
            littleBatManager->update(deltaTime);
            break;
        case 5:
            cube->setPosition(sf::Vector2f(pos.position.x + 127, pos.position.y + 62));
            bigBat->setPosition(sf::Vector2f(pos.position.x + 41, pos.position.y + 40));
            bigBatManager->update(deltaTime);
            break;
        }
        // Simon movement
        sf::Vector2f posSimon = simon->getPosition();
        // std::cout << posSimon.x << std::endl;
        const float simonSpeed = 60.0f; // pixels per second
        destiny = pos.position.x + 250;
        if (posSimon.x >= destiny)
        {
            simon->setPosition(sf::Vector2f(destiny, posSimon.y));
            showInter = false;
            isInBossFight = false;

            // Change of level after the animation
            int aux_CurrentLevel = static_cast<int>(currentLevel);
            switch (aux_CurrentLevel)
            {
            case 1:
                gStartingLevel = 3;
                currentLevel = gStartingLevel;
                currentStage = 1;
                gStartingStage = currentStage;
                break;
            case 3:
                gStartingLevel = 5;
                currentLevel = gStartingLevel;
                currentStage = 1;
                gStartingStage = currentStage;
                break;
            case 5:
                gStartingLevel = 7;
                currentLevel = gStartingLevel;
                currentStage = 1;
                gStartingStage = currentStage;
                break;
            default:
                std::cout << "No more levels" << std::endl;
                break;
            }
            // if (aux_CurrentLevel == 7 && gStartingLevel == 1)
            //     return;
            
            player.acceptsInput = true;
            player.setState(std::make_unique<PlayerIdleState>());
            loadLevelAndEnemies();
        }
        else
        {
            timerSteps += deltaTime;
            if (timerSteps >= stepsInterval)
            {
                timerSteps = 0;
                auto audio = configManager.getAudio();
                gameSoundManager.playSound("between_lvl_steps", gameSoundManager.realVolume(audio.master_volume, audio.sound_volume));
            }
            simon->setPosition(sf::Vector2f(posSimon.x + simonSpeed * deltaTime, posSimon.y));
        }
        simonManager->update(deltaTime);
    }
}

void Game::prepareVariablesForLevel()
{
    if (gGoToNextLevel)
    {
        sf::FloatRect pos(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
        if (currentLevel == 1)
        {
            time = 500;
            pos = tilemaps[5].getMapBoundsBossFight();
        }
        if (currentLevel == 3)
        {
            time = 500;
            pos = tilemaps[5].getMapBoundsBossFight();
        }
        if (currentLevel == 5)
        {
            time = 700;
            pos = tilemaps[6].getMapBoundsBossFight();
        }
        timerSteps = 0;
        player.acceptsInput = false;
        showInter = true;
        entering = true;
        map->setPosition(sf::Vector2f(pos.position.x + 256, pos.position.y + 30));
        simon->setPosition(sf::Vector2f(pos.position.x - 10, pos.position.y + 150));
    }
    resetEndLevelScoreAnimation();
    gGoToNextLevel = false;
}

void Game::loadLevelAndEnemies()
{
    tilemaps.loadLevel(currentLevel);
    enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);
    bossManager->loadBossesFromLevel(currentLevel, tilemaps);
    startStage(currentStage);
}

void Game::updateGUITime()
{
    std::stringstream timeStream;
    timeStream << "TIME   " << std::setw(4) << std::setfill('0') << std::to_string(time);
    texts[1].setString(timeStream.str());
}

void Game::updateGUIStage(){
    std::stringstream stageStream;
    int stage = 0;
    switch (currentLevel)
    {
    case 1:
        stage = currentStage;
        break;
    
    case 3:
        stage = 5 + currentStage;
        break;
    
    case 5:
        stage = 10 + currentStage;
        break;

    case 7:
        stage = 16 + currentStage;
        break;
    }
    stageStream << "STAGE " << std::setw(2) << std::setfill('0') << std::to_string(stage);
    texts[2].setString(stageStream.str());
}

void Game::resetEndLevelScoreAnimation()
{
    m_playedVictoryMusic = false;
    m_endLvlAnimationCoundown = gEND_LVL_ANIMATION_COUNDOWN;
    gTriggerEndLvlScoreAnimation = false;
    gPlayEndLvlScoreAnimation = false;
    m_endLvlAnimation_TimeScoreAccumulator = 0.f;
    m_endLvlAnimation_HeartScoreAccumulator = 0.f;
}

void Game::endLevelScoreAnimation(const float deltaTime)
{
    static float countdownToPlayTimeTally = gTIME_POINTS_SFX_FRECUENCY;
    static float countdownToPlayHeartTally = gHEARTS_SFX_FRECUENCY;
    static bool playTimeTallyToggle = true;

    bool timeSoundPlayedOnce = false;
    bool heartSoundPlayedOnce = false;

    if (time > 0)
    {
        m_endLvlAnimation_TimeScoreAccumulator += deltaTime;
        while (m_endLvlAnimation_TimeScoreAccumulator >= (1.f / gTIME_POINTS_PER_SECOND) && time > 0)
        {
            time--;
            player.score += 10;
            m_endLvlAnimation_TimeScoreAccumulator -= (1.f / gTIME_POINTS_PER_SECOND);
        }

        countdownToPlayTimeTally -= deltaTime;
        if (countdownToPlayTimeTally <= 0.f)
        {
            countdownToPlayTimeTally = gTIME_POINTS_SFX_FRECUENCY;

            if (playTimeTallyToggle)
            { // Only play the sound once every 2 times
                auto audio = configManager.getAudio();
                gameSoundManager.playSound("time_tally", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
                timeSoundPlayedOnce = true;
            }

            playTimeTallyToggle = !playTimeTallyToggle;
        }

        // If "time_tally" was never played and we are about to finish, we play it once
        if (time == 0 && !timeSoundPlayedOnce)
        {
            auto audio = configManager.getAudio();
            gameSoundManager.playSound("time_tally", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        }
    }
    else if (player.hearts > 0)
    {
        m_endLvlAnimation_HeartScoreAccumulator += deltaTime;
        while (m_endLvlAnimation_HeartScoreAccumulator >= (1.f / gHEARTS_PER_SECOND) && player.hearts > 0)
        {
            player.hearts--;
            player.score += 100;
            countdownToPlayHeartTally--;
            m_endLvlAnimation_HeartScoreAccumulator -= (1.f / gHEARTS_PER_SECOND);

            if (countdownToPlayHeartTally <= 0.f)
            {
                auto audio = configManager.getAudio();
                gameSoundManager.playSound("heart_tally", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
                countdownToPlayHeartTally = gHEARTS_SFX_FRECUENCY;
                heartSoundPlayedOnce = true;
            }
        }

        if (player.hearts == 0 && !heartSoundPlayedOnce)
        {
            auto audio = configManager.getAudio();
            gameSoundManager.playSound("heart_tally", gameSoundManager.realVolume(audio.master_volume, audio.music_volume));
        }
    }
    else
    {                                                                // All hearts and time points have been consumed
        std::this_thread::sleep_for(std::chrono::milliseconds(550)); // Duration of the last played sound
        gGoToNextLevel = true;
    }

    updateGUITime();
}

// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow &window, Camera &camera)
{
    if (restartLoadingClock)
    {
        restartLoadingClock = false;
        loadingClock.restart();
    }

    if (isRosarioBlinking)
    {
        // std::cout << "Blinking" << std::endl;
        float elapsed = rosarioBlinkClock.getElapsedTime().asSeconds();

        if (elapsed < rosarioBlinkDuration)
        {
            if (static_cast<int>(elapsed / rosarioBlinkInterval) % 2 == 0)
            {
                sf::RectangleShape whiteScreen(sf::Vector2f{tilemaps[currentLevel].m_tilesPerRow * gTileSize,
                                                            tilemaps[currentLevel].m_tilesPerColumn * gTileSize});
                whiteScreen.setFillColor(sf::Color::White);
                window.draw(whiteScreen);
                window.display();
                sf::sleep(sf::milliseconds(10));
            }
        }
        else
        {
            isRosarioBlinking = false;
        }
    }

    if (loadingClock.getElapsedTime().asSeconds() < gLoadingTime)
    {
        isLoading = true;
        player.acceptsInput = false;

        sf::RectangleShape blackScreen(camera.getView(window.getSize()).getSize());
        blackScreen.setFillColor(sf::Color::Black);
        window.draw(blackScreen);
    }
    else
    {
        // Just finished loading
        if (isLoading)
        {
            isLoading = false;
            beginStageEntrance = true;
            if (!withOutLives)
            {
                player.acceptsInput = true;
            }
        }

        // camera.updateView(*player.sprite, tileMap.getMapBounds(), 100.f);
        tilemaps[currentStage].drawScene(window, camera);

        // DEBUG: Draw player and whip hitbox
        if (gDrawHitboxes)
        {
            window.draw(FloatRectToRectShape(player.sprite->getGlobalBounds()));
            if (player.isAttacking)
            {
                window.draw(FloatRectToRectShape(player.whip.sprite->getGlobalBounds()));
            }
            if (player.weaponIsActive)
            {
                window.draw(FloatRectToRectShape(player.subWeapon.sprite->getGlobalBounds()));
            }
            if (player.weaponIsActive2)
            {
                window.draw(FloatRectToRectShape(player.subWeapon2.sprite->getGlobalBounds()));
            }
            if (player.weaponIsActive3)
            {
                window.draw(FloatRectToRectShape(player.subWeapon3.sprite->getGlobalBounds()));
            }
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
        if (!isInBossFight)
        {
            drawHealthBars(window, player.health, 16, virtualWorldOffset);
        }
        else
        { // Boss fight
            int maxSquares = 16;
            int scaledBossLife = (currentBossLife * maxSquares) / maxLife;
            if (scaledBossLife > maxSquares)
                scaledBossLife = maxSquares;
            if (scaledBossLife < 0)
                scaledBossLife = 0;
            drawHealthBars(window, player.health, scaledBossLife, virtualWorldOffset);
        }

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
        if (player.subWeaponType != ItemType::NONE)
        {
            redBorderPosition.x += 5.f;
            // redBorderPosition.y -= 2.f;
            guiSubWeaponSprite->setPosition(redBorderPosition);
            window.draw(*guiSubWeaponSprite);
        }

        if (player.isDoubleShotActive && !player.isTripleShotActive)
        {
            redBorderPosition.x += 75.f;
            // redBorderPosition.y -= 2.f;
            guiDoubleShotSprite = getItemSprite(ItemType::DOUBLE_SHOT_GUI);
            guiDoubleShotSprite->setPosition(redBorderPosition);
            if (player.timeDoubleShotActiveCounter <= 3.0f)
            {
                if (static_cast<int>(player.timeDoubleShotActiveCounter * 5) % 2 == 0)
                {
                    window.draw(*guiDoubleShotSprite);
                }
            }
            else
            {
                // Dibujo normal si no está en el rango de parpadeo
                window.draw(*guiDoubleShotSprite);
            }
        }
        if (player.isTripleShotActive)
        {
            redBorderPosition.x += 75.f;
            // redBorderPosition.y -= 2.f;
            guiTripleShotSprite = getItemSprite(ItemType::TRIPLE_SHOT_GUI);
            guiTripleShotSprite->setPosition(redBorderPosition);
            if (player.timeTripleShotActiveCounter <= 3.0f)
            {
                if (static_cast<int>(player.timeTripleShotActiveCounter * 5) % 2 == 0)
                {
                    window.draw(*guiTripleShotSprite);
                }
            }
            else
            {
                // Dibujo normal si no está en el rango de parpadeo
                window.draw(*guiTripleShotSprite);
            }
        }

        // Draw the heart counter icon
        guiHeartSprite->setPosition(sf::Vector2f(gGUI_heartCounter_position_x, gGUI_heartCounter_position_y) + virtualWorldOffset);
        window.draw(*guiHeartSprite);

        // PLAYER and ENTITIES =====================================

        enemyManager->draw(window, currentLevel, currentStage);
        bossManager->draw(window, currentLevel, currentStage);

        player.draw(window);

        if (withOutLives)
        {
            sf::View view = window.getView();
            sf::Vector2f center = view.getCenter();
            sf::RectangleShape black(sf::Vector2f(400, 400));
            black.setFillColor(sf::Color::Black);
            black.setPosition(sf::Vector2f(center.x - 200, center.y - 72));
            window.draw(black);

            deadScreenTexts[0].setPosition(sf::Vector2f(center.x - 35, center.y - 10));
            window.draw(deadScreenTexts[0]);

            deadScreenTexts[1].setPosition(sf::Vector2f(center.x - 25, center.y + 25));
            window.draw(deadScreenTexts[1]);

            deadScreenTexts[2].setPosition(sf::Vector2f(center.x - 25, center.y + 45));
            window.draw(deadScreenTexts[2]);

            window.draw(gSprites[0]);
        }

        // MAP transition
        if (showInter)
        {
            sf::View view = window.getView();
            sf::Vector2f center = view.getCenter();
            sf::RectangleShape black(sf::Vector2f(400, 400));
            black.setFillColor(sf::Color::Black);
            black.setPosition(sf::Vector2f(center.x - 200, center.y - 72));
            window.draw(black);
            window.draw(*map);
            if(!entering){
                window.draw(*cube);
                switch (currentLevel){
                    case 1:
                        window.draw(*littleBat);
                        break;
                    case 3:
                        window.draw(*littleBat);
                        break;
                    case 5:
                        window.draw(*bigBat);
                        break;
                }
                window.draw(*simon);
            }
        }

        // window.draw(FloatRectToRectShape(player.gPlayerActivationZone));
        // window.draw(FloatRectToRectShape(player.gPlayerDeactivationZone));

        // collisionGrid.drawCells(window, virtualCoordOfUpperLeftCornerOfGame);
    }

    particleSystem.draw(window);
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
}

void Game::checkCollisions(const sf::Vector2f &viewPosition)
{
    // 1. Add tiles (static entities)
    staticEntities.clear();

    for (auto miscTile : tilemaps[currentStage].m_miscTiles)
        staticEntities.push_back(&*miscTile);
    for (auto &stairTile : tilemaps[currentStage].m_stairTiles)
        staticEntities.push_back(&stairTile);
    for (auto &waterZone : tilemaps[currentStage].m_waterZones)
        staticEntities.push_back(&waterZone);

    // 2. Add everything else (dynamic entities)
    dynamicEntities.clear();

    //      Add player and weapons
    dynamicEntities.push_back(&player);
    dynamicEntities.push_back(&player.whip);
    dynamicEntities.push_back(&player.subWeapon);
    dynamicEntities.push_back(&player.subWeapon2);
    dynamicEntities.push_back(&player.subWeapon3);

    //      Add enemies
    for (auto &enemy : enemyManager->getEnemies(currentLevel, currentStage))
        dynamicEntities.push_back(enemy);

    //      Add bosses
    for (auto &boss : bossManager->getBosses(currentLevel, currentStage))
        dynamicEntities.push_back(boss);

    for (auto &item : tilemaps[currentStage].m_items)
        dynamicEntities.push_back(item.get());

    // ... ADD THE REST OF ENTITIES

    checkPlayerMapBoundCollisions();

    checkSolidTileCollisions(dynamicEntities);

    checkDoorTileCollisions();

    if (tilemaps[currentStage].bossPosition != BossPos::NO_BOSS)
    {
        if (!isInBossFight &&
            ((tilemaps[currentStage].bossPosition == BossPos::BOSS_RIGHT && hasReachedEndStage) ||
             (tilemaps[currentStage].bossPosition == BossPos::BOSS_LEFT && hasReachedStartStage)))
        {
            enemyManager->restartEnemies(currentLevel, currentStage);

            isInBossFight = true;
            currentBossPhase = 1;

            // Starting boss music (DIFFENT FOR DRACULA)
            if (currentLevel != 7)
            {
                gameSoundManager.stopAllMusic();
                auto audio = configManager.getAudio();
                gameSoundManager.playMusic("boss", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), true);
            }

            // FALTA HACER QUE NO SPAWNEEN MÁS ENEMIGOS / QUE NO ENTREN EN PANTALLA
        }
    }

    collisionGrid.checkCollisions(staticEntities, dynamicEntities, viewPosition);
}

void Game::checkSolidTileCollisions(std::vector<Entity *> &dynamicEntities)
{
    player.isOnGround = false;
    for (auto &e : dynamicEntities)
    {
        for (auto &solidTileRow : tilemaps[currentStage].m_solidTiles)
        {
            for (auto &t : solidTileRow)
            {
                std::optional<sf::FloatRect> intersectionRect = checkIntersections(*e, t);
                if (intersectionRect)
                {
                    e->onCollision(t, *this, *intersectionRect);
                    t.onCollision(*e, *this, *intersectionRect);
                }
            }
        }
        // tilemaps[currentStage].solidTileBVH.queryAndCollide(*e, [&](Entity& tile) {
        //     e->onCollision(tile, *this);
        //     tile.onCollision(*e, *this);
        // });
    }
}

void Game::checkDoorTileCollisions()
{
    for (auto &d : tilemaps[currentStage].m_doorTiles)
    {
        std::optional<sf::FloatRect> intersectionRect = checkIntersections(player, d);
        if (intersectionRect)
        {
            player.onCollision(d, *this, *intersectionRect);
            d.onCollision(player, *this, *intersectionRect);
        }
    }
}

void Game::checkItemsCollisions()
{
    for (auto &item : tilemaps[currentStage].m_items)
    {
        int maxCollisions = 2;

        if (item->isOnGround)
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
                            item->isOnGround = true; // Set item to be on ground
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

    sf::FloatRect mapBounds;

    if (isInBossFight)
    {
        mapBounds = tilemaps[currentStage].getMapBoundsBossFight();
    }
    else
    {
        mapBounds = tilemaps[currentStage].getMapBounds();
    }

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
        if (overlapY < 7.66f)
        {
            simonIsTouchingGround = ((playerBounds.position.y + playerBounds.size.y) < (tileBounds.position.y + tileBounds.size.y));
        }
        if (overlapX < overlapY && (!simonIsTouchingGround)) // Horizontal collision
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
                if (overlapY >= 1.f)
                {
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

    // Misc tiles: first we check Simon collisions
    // for (auto &miscTile : tilemaps[currentStage].m_miscTiles)
    // {
    //     // Misc tiles only have 1 hitbox
    //     sf::FloatRect tileBounds = miscTile.hitboxes[0];

    //     // ignore hitboxless tiles
    //     if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f) continue;

    //     if (miscTile.isDestroyed) continue;

    //     computePlayerTileIntersection(hasCollided, tileBounds);
    // }

    // Misc tiles: second we check whip collisions
    if (player.isAttacking)
    {
        for (auto &miscTile : tilemaps[currentStage].m_miscTiles)
        {
            if (miscTile->isDestroyed)
                continue;

            sf::FloatRect tileBounds = miscTile->hitboxes[0]; // Misc tiles only have 1 hitbox

            // ignore hitboxless tiles
            if (tileBounds.size.x == 0.0f || tileBounds.size.y == 0.0f)
                continue;

            sf::FloatRect whipBounds = player.whip.sprite->getGlobalBounds();
            if (const std::optional<sf::FloatRect> intersection = whipBounds.findIntersection(tileBounds))
            {
                if (miscTile->isBreakable)
                {
                    miscTile->isDestroyed = true;
                    createDropItem(miscTile->dropType, miscTile->sprite->getPosition());
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
                // std::cout << "NEXT STAGE" << std::endl;
                restartLoadingClock = true;

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
                // std::cout << "PREVIOUS STAGE" << std::endl;
                restartLoadingClock = true;

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
                // std::cout << "NEXT LEVEL" << std::endl;
                restartLoadingClock = true;
                currentLevel += 1;
                tilemaps.loadLevel(currentLevel);
                enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);
                bossManager->loadBossesFromLevel(currentLevel, tilemaps);

                resetEndLevelScoreAnimation();
                gGoToNextLevel = false;
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
        // std::cout << "NEXT STAGE" << std::endl;
        restartLoadingClock = true;

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
        // std::cout << "PREVIOUS STAGE" << std::endl;
        restartLoadingClock = true;

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
        // std::cout << "NEXT LEVEL" << std::endl;
        restartLoadingClock = true;
        currentLevel += 1;
        tilemaps.loadLevel(currentLevel);
        enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);
        bossManager->loadBossesFromLevel(currentLevel, tilemaps);

        resetEndLevelScoreAnimation();
        gGoToNextLevel = false;
    }
    else
    {
        std::cout << "ERROR: Stage doesn't correspond to any door stages" << std::endl;
    }
}
// -------------------------------------------------------------------------------------
//                                    AUXILIARY FUNCTIONS
// -------------------------------------------------------------------------------------

void Game::createDropItem(DropType dropType, sf::Vector2f itemPosition)
{
    std::shared_ptr<Item> item = getDropItem(dropType, itemPosition, player.isWhipAtMaxLevel(),
                                             player.subWeaponType, player.isDoubleShotActive);
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
    else if (itemType == ItemType::PORK_CHOP)
    {
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
    else if (itemType == ItemType::MAGIC_CRYSTAL)
    {
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
    tilemaps[currentStage].regenerateMiscTiles();
    tilemaps[currentStage].m_items.clear();

    enemyManager->loadEnemiesFromLevel(currentLevel, tilemaps);
    bossManager->loadBossesFromLevel(currentLevel, tilemaps);

    player.acceptsInput = true;

    hasReachedEndStage = false;
    hasReachedStartStage = false;
    isInBossFight = false;
    gKilledBoss = false;
    currentBossPhase = 0;

    if (unsigned(stage) > tilemaps.tilemaps.size())
    {
        std::cerr << "ERROR: Level " << currentLevel << ", stage " << stage << " doesn't exist";
        return -1;
    }

    particleSystem.clear();

    player.verticalSpeed = 0.f;
    player.horizontalSpeed = 0.f;

    currentStage = stage;

    // switch (currentLevel)
    // {
    // case 1:
    //     time = 300;
    //     break;
    
    // case 3:
    //     time = 500;
    //     break;
    
    // case 5:
    //     time = 500;
    //     break;
    
    // case 7:
    //     time = 700;
    //     break;
    // }

    if (!withOutLives)
    {
        setLevelMusic(currentLevel);
    }
    else
    {
        auto audio = configManager.getAudio();
        gameSoundManager.playMusic("gameOver", gameSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
    }

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
                player.setState(std::make_unique<PlayerStairIdleState>());
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
    // std::cout << "Current stage: " << currentStage << std::endl;

    setLevelMusic(currentLevel);

    tilemaps[currentStage].m_items.clear();
    tilemaps.loadLevel(currentLevel);

    switch (currentLevel)
    {
    case 1:
        time = 300;
        break;
    
    case 3:
        time = 500;
        break;
    
    case 5:
        time = 500;
        break;
    
    case 7:
        time = 700;
        break;
    }
    updateGUITime();
    updateGUIStage();

    player.dir = PlayerDirection::RIGHT;
    player.health = player.maxHealth;
    player.subWeaponType = ItemType::NONE;
    player.subWeapon.sprite->setPosition({-100.f, 0.f});
    player.subWeapon.intersected = true;
    player.hearts = 5;

    player.acceptsInput = true;

    hasReachedEndStage = false;
    hasReachedStartStage = false;
    isInBossFight = false;
    gKilledBoss = false;
    currentBossPhase = 0;
    player.visible = true;

    player.sprite->setPosition(tilemaps[currentStage].initialPosition);

    enemyManager->restartEnemies(currentLevel, currentStage);
    bossManager->restartBosses(currentLevel, currentStage);

    particleSystem.clear();
}

void Game::restartLevel()
{
    // std::cout << "Current stage: " << currentStage << std::endl;

    particleSystem.clear();
    tilemaps[currentStage].m_items.clear();
    tilemaps.loadLevel(currentLevel);

    switch (currentLevel)
    {
    case 1:
        time = 300;
        break;
    
    case 3:
        time = 500;
        break;
    
    case 5:
        time = 500;
        break;
    
    case 7:
        time = 700;
        break;
    }
    updateGUITime();
    updateGUIStage();

    player.dir = PlayerDirection::RIGHT;
    player.health = player.maxHealth;
    player.subWeaponType = ItemType::NONE;
    player.subWeapon.intersected = true;
    player.hearts = 5;
    player.score = 0;
    player.lives = 3;

    player.acceptsInput = true;

    hasReachedEndStage = false;
    hasReachedStartStage = false;
    isInBossFight = false;
    gKilledBoss = false;
    currentBossPhase = 0;
    player.visible = true;

    startStage(1);
}

void Game::setLevelMusic(int level)
{
    // Music for the game
    auto audio = configManager.getAudio();
    float volume = gameSoundManager.realVolume(audio.master_volume, audio.music_volume);
    switch (level)
    {
    case 1:
        gameSoundManager.playMusic("vampireKiller", volume);
        break;

    case 2:
        gameSoundManager.playMusicSequence("stalker1", "stalker2", true, volume);
        break;

    case 3:
        gameSoundManager.playMusicSequence("wicked1", "wicked2", true, volume);
        break;

    case 4:
        gameSoundManager.playMusicSequence("edge1", "edge2", true, volume);
        break;

    case 5:
        gameSoundManager.playMusicSequence("heart1", "heart2", true, volume);
        break;

    case 6:

        break;
    case 7:
        std::cout << "Dracula" << std::endl;
        gameSoundManager.playMusic("dracula1", volume);
        break;
    }
}