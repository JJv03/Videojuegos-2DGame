#include "gameState.h"
#include <iostream>
#include "gameStateMachine.h"
#include "globals.h"

#include <thread> // Necessary for using sleep_for
#include <chrono>
#include "configManager.h"
#include "utils.h"

// constexpr sf::Keyboard::Scancode KEY_RIGHT = sf::Keyboard::Scancode::Right;
// constexpr sf::Keyboard::Scancode KEY_LEFT = sf::Keyboard::Scancode::Left;
// constexpr sf::Keyboard::Scancode KEY_DOWN = sf::Keyboard::Scancode::Down;
// constexpr sf::Keyboard::Scancode KEY_UP = sf::Keyboard::Scancode::Up;
// constexpr sf::Keyboard::Scancode KEY_ENTER = sf::Keyboard::Scancode::Enter;
// constexpr sf::Keyboard::Scancode KEY_ESC = sf::Keyboard::Scancode::Escape;
// constexpr sf::Keyboard::Scancode KEY_JUMP = sf::Keyboard::Scancode::X;
// constexpr sf::Keyboard::Scancode KEY_ATTACK = sf::Keyboard::Scancode::Z;

using BossPos = TileMap::BossPosition;


const bool debug = false;



sf::FloatRect getCenteredViewport(float windowAspect, float viewAspect) {
    float x_offset = 0.0f, y_offset = 0.0f;
    float width = 1.0f, height = 1.0f;

    if (windowAspect > viewAspect) {    // Window is wider than the view: adjust height
        width = viewAspect / windowAspect;
        x_offset = (1.0f - width) / 2.0f;
    } else {    // Window is taller than the view --> adjust height
        height = windowAspect / viewAspect;
        y_offset = (1.0f - height) / 2.0f;
    }

    return sf::FloatRect({x_offset, y_offset}, {width, height});
}

sf::View GameState::getView(sf::RenderWindow& window, Camera& camera) {
    // Update global window dimensions
    sf::Vector2u windowSize = window.getSize();
    const float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;


    sf::Vector2f viewSize(gMenuGS_size_x, gMenuGS_size_y);
    const float viewAspect = viewSize.x / viewSize.y;
    sf::View view(sf::FloatRect({0.f, 0.f}, viewSize));

    view.setViewport(getCenteredViewport(windowAspect, viewAspect));

    return view;
}

sf::View GameGS::getView(sf::RenderWindow& window, Camera& camera) {

    // Update global window dimensions
    sf::Vector2u windowSize = window.getSize();
    const float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;


    sf::Vector2f viewSize(static_cast<int>(this->m_viewSize.x), static_cast<int>(this->m_viewSize.y));
    const float viewAspect = viewSize.x / viewSize.y;
    sf::View view(sf::FloatRect({gGameGS_position_x, gGameGS_position_y}, viewSize));

    sf::Vector2f currentCenter = view.getCenter();
    view.setCenter({game.player.sprite->getPosition().x, currentCenter.y});


    // LIMIT WITH CURRENT MAP
    sf::FloatRect mapBounds = game.tilemaps[game.currentStage].getMapBounds();
    mapBounds.size.y = mapBounds.size.y + gGUI_size_y + gTileSize;
    mapBounds.position.y = mapBounds.position.y - gGUI_size_y;

    // Limits x-axis
    float minX = mapBounds.position.x + viewSize.x / 2.f;
    float maxX = mapBounds.position.x + mapBounds.size.x - viewSize.x / 2.f;

    // Limits y-axis
    float minY = mapBounds.position.y + viewSize.y / 2.f;
    float maxY = mapBounds.position.y + mapBounds.size.y - viewSize.y / 2.f;

    float centerX = std::max(minX, std::min(view.getCenter().x, maxX));
    float centerY = std::max(minY, std::min(view.getCenter().y, maxY));

    if(isInBossFight){
        if(game.tilemaps[game.currentStage].bossPosition == BossPos::BOSS_LEFT){
            view.setCenter({std::round(minX), std::round(centerY)});
        }
        else{
            view.setCenter({std::round(maxX), std::round(centerY)});
        }
    } else {
        view.setCenter({std::round(centerX), std::round(centerY)});

        if(centerX >= maxX && !hasReachedEndStage){
            hasReachedEndStage = true;
        } else if(centerX <= minX && !hasReachedStartStage){
            hasReachedStartStage = true;
        }
    }

    view.setViewport(getCenteredViewport(windowAspect, viewAspect)); 

    return view;
}

sf::View MenuGS::getView(sf::RenderWindow& window, Camera& camera) {
    sf::Vector2u windowSize = window.getSize();
    const float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;


    sf::Vector2f viewSize(static_cast<int>(this->m_viewSize.x), static_cast<int>(this->m_viewSize.y));
    sf::View view(sf::FloatRect({0.f, 0.f}, viewSize));
    const float viewAspect = viewSize.x / viewSize.y;

    view.setViewport(getCenteredViewport(windowAspect, viewAspect));

    return view;
}


// ======================================================
//                      GAME STATE 
// ======================================================

void GameGS::init(){
    this->m_viewSize.x = gGameGS_size_x;
    this->m_viewSize.y = gGameGS_size_y;
    if(debug) std::cout << "ESTADO: Game" << std::endl;
    game.init();
}

void GameGS::handleInput(sf::Event event){
    if(!gPlayEndLvlScoreAnimation && !showInter){
        auto controls = configManager.getControls();
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if (keyPressed->scancode == controls.escape)
            {
                auto audio = configManager.getAudio();
                gameSoundManager.playSound("pause", gameSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                stateMachine->addState(std::make_unique<PauseGS>(stateMachine));
                return;
            }
        }
    }

    game.handleInput(event);
}

void GameGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    game.update(deltaTime, viewPosition, windowHasFocus);

    if(goBack){
        gameSoundManager.stopAllMusic();
        stateMachine->replaceAllStates(std::make_unique<MenuGS>(stateMachine));
        goBack = false;
    }

    if(goToEndAnimation){
        gameSoundManager.stopAllMusic();
        stateMachine->replaceAllStates(std::make_unique<CreditsAnimationGS>(stateMachine));
        goToEndAnimation = false;
    }
}

void GameGS::draw(sf::RenderWindow& window, Camera& camera){
    game.draw(window, camera);
}

void GameGS::pause(){
    if(debug) std::cout << "ESTADO: Game PAUSADO" << std::endl;
}

void GameGS::resume(){
    if(debug) std::cout << "ESTADO: Game REANUDADO" << std::endl;
}

void GameGS::close(){
    if(debug) std::cout << "ESTADO: Game CERRADO" << std::endl;
    game.deadScreenTexts.clear();
    game.gSprites.clear();
}

GameGS::~GameGS() {}


// ======================================================
//                      MENU STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> menuTextures;
std::vector<sf::Sprite> menuSprites;

void MenuGS::init(){
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    if(debug) std::cout << "ESTADO: Menu" << std::endl;
    position = 0;
    showPopUp = false;
    colPopUp = 0;
    exit = false;
    enterPressed = false;
    if (!menuTextures["menu"].loadFromFile("./assets/sprites/menu/menu2.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite menu(menuTextures["menu"]);

    // Adjusts menu position
    sf::FloatRect spriteBounds = menu.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    menu.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    menu.setPosition(sf::Vector2f(xPosition, yPosition));

    menuSprites.push_back(menu);

    if (!menuTextures["popUpExit"].loadFromFile("./assets/sprites/menu/blankBox.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite popUpExit(menuTextures["popUpExit"]);

    // Adjusts menu position
    popUpExit.setScale(sf::Vector2f(0.65, 0.65));

    spriteBounds = popUpExit.getGlobalBounds();

    xPosition = (gWindowWidth - spriteBounds.size.x) / 2;
    yPosition = (gWindowHeight - spriteBounds.size.y) / 2;

    popUpExit.setPosition(sf::Vector2f(xPosition, yPosition));

    menuSprites.push_back(popUpExit);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    sf::Text textPopUp(font, "BACK", 30);
    textPopUp.setFillColor(sf::Color::White);
    sf::FloatRect textPopUpBounds = textPopUp.getLocalBounds();

    // Centers position
    float xPopPos = 70;
    float yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) + 20;

    textPopUp.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp);

    sf::Text textPopUp2(font, "EXIT", 30);
    textPopUp2.setFillColor(sf::Color(122, 71, 22));
    textPopUp2.setOutlineColor(sf::Color(255, 140, 0));
    textPopUp2.setOutlineThickness(1.5);

    textPopUpBounds = textPopUp2.getLocalBounds();

    // Centers position
    xPopPos = 250;
    yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) + 20;

    textPopUp2.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp2);

    sf::Text textPopUp3(font, "ARE YOU SURE...", 20);
    textPopUp3.setFillColor(sf::Color::White);
    textPopUpBounds = textPopUp3.getLocalBounds();

    // Centers position
    xPopPos = ((gWindowWidth - textPopUpBounds.size.x) / 2);
    yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) - 25;

    textPopUp3.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp3);
    
    // Defines menu options
    std::string textos[4] = {"STORY MODE", "LEVEL SELECT", "CONFIG", "EXIT"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        // text.setPosition(sf::Vector2f(330.f, 80.f + i * 40.f));
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 200.f + i * 45.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        options.push_back(text);
    }

    if (!menuTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(menuTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = options[0].getPosition().x - 25.f;
    float torchY = options[0].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    menuSprites.push_back(torch);

    // menuSoundManager.loadSound("menuEnter", "./assets/sounds/menuEnter.mp3");

    // menuSoundManager.loadMusic("menuMusic", "./assets/music/menuMusic.mp3");
    menuSoundManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    auto audio = configManager.getAudio();

    menuSoundManager.loadMusic("menuMusic", "./assets/music/08Out_of_Time.mp3");
    menuSoundManager.playMusic("menuMusic", menuSoundManager.realVolume(audio.master_volume, audio.music_volume));
}

void MenuGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if(!showPopUp){
                if (keyPressed->scancode == controls.down && position < 3) {
                    timeOut = 0;
                    if (!menuSprites.empty()) {
                        sf::Sprite torch = menuSprites.back();
                        menuSprites.pop_back();

                        position ++;
                        std::cout<<position<<std::endl;

                        float torchX = options[position].getPosition().x - 25.f;
                        float torchY = options[position].getPosition().y + 2.f;
                        torch.setPosition(sf::Vector2f(torchX, torchY));

                        menuSprites.push_back(torch);
                    }
                }

                if (keyPressed->scancode == controls.up && position > 0) {    
                    timeOut = 0;
                    if (!menuSprites.empty()) {
                        sf::Sprite torch = menuSprites.back();
                        menuSprites.pop_back();

                        position --;
                        std::cout<<position<<std::endl;

                        float torchX = options[position].getPosition().x - 25.f;
                        float torchY = options[position].getPosition().y + 2.f;
                        torch.setPosition(sf::Vector2f(torchX, torchY));

                        menuSprites.push_back(torch);
                    }
                }

                if (keyPressed->scancode == controls.enter) {
                    timeOut = 0;
                    enterPressed = true;
                    auto audio = configManager.getAudio();
                    menuSoundManager.playSound("menuEnter", menuSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    switch (position) {
                        case 0:
                            stateMachine->replaceState(std::make_unique<InitAnimationGS>(stateMachine));
                            break;
                        case 1:
                            stateMachine->replaceState(std::make_unique<levelSelectorGS>(stateMachine));
                            break;
                        case 2:
                            stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                            break;
                        case 3:
                            colPopUp = 0;
                            showPopUp = true;
                            enterPressed = false;
                            if (!menuSprites.empty()) {
                                timeOut = 0;
                                sf::Sprite torch = menuSprites.back();
                                menuSprites.pop_back();

                                float torchX = popUpText[colPopUp].getPosition().x - 25.f;
                                float torchY = popUpText[colPopUp].getPosition().y + 2.f;
                                torch.setPosition(sf::Vector2f(torchX, torchY));

                                menuSprites.push_back(torch);
                            }
                            break;
                    }
                }
            }
            else{
                if (keyPressed->scancode == controls.right && colPopUp == 0){
                    colPopUp = 1;
                }
                if (keyPressed->scancode == controls.left && colPopUp == 1){
                    colPopUp = 0;
                }
                if (!menuSprites.empty()) {
                    timeOut = 0;
                    sf::Sprite torch = menuSprites.back();
                    menuSprites.pop_back();

                    float torchX = popUpText[colPopUp].getPosition().x - 25.f;
                    float torchY = popUpText[colPopUp].getPosition().y + 2.f;
                    torch.setPosition(sf::Vector2f(torchX, torchY));

                    menuSprites.push_back(torch);
                }
                if (keyPressed->scancode == controls.enter){
                    if(colPopUp == 0){
                        showPopUp = false;
                        if (!menuSprites.empty()) {
                            sf::Sprite torch = menuSprites.back();
                            menuSprites.pop_back();

                            float torchX = options[position].getPosition().x - 25.f;
                            float torchY = options[position].getPosition().y + 2.f;
                            torch.setPosition(sf::Vector2f(torchX, torchY));

                            menuSprites.push_back(torch);
                        }
                    }
                    if(colPopUp == 1){
                        std::cout<<"Bye bye :)"<<std::endl;
                        exit = true;
                    }
                }
                std::cout << colPopUp << std::endl;
            }
        }
    }
}

void MenuGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    timeOut += deltaTime;

    if (timeOut >= timeInterval && !showPopUp) { // X seconds to go back
        std::cout << "Reproduce again the initAnimation" << std::endl;
        stateMachine->replaceState(std::make_unique<InitMenuGS>(stateMachine));
    }
}

void MenuGS::draw(sf::RenderWindow& window, Camera& camera){
    // std::cout<<"Print"<<std::endl;
    // std::cout<<menuSprites.size()<<std::endl;
    // std::cout<<menuTextures.size()<<std::endl;
    // std::cout<<options.size()<<std::endl;
    window.draw(menuSprites[0]);
    for (const auto& text : options) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    if(showPopUp){
        window.draw(menuSprites[1]);
        for (const auto& text : popUpText) {
            window.draw(text);
        }
    }
    window.draw(menuSprites[2]);

    if (exit){
        window.close();
    }
}
 
void MenuGS::pause(){
    if(debug) std::cout << "ESTADO: Menu PAUSADO" << std::endl;
}

void MenuGS::resume(){
    if(debug) std::cout << "ESTADO: Menu REANUDADO" << std::endl;
}

void MenuGS::close(){
    if(debug) std::cout << "ESTADO: Menu CERRADO" << std::endl;
    menuSprites.clear();
    menuTextures.clear();
    options.clear();
    popUpText.clear();
}

MenuGS::~MenuGS() {}


// ======================================================
//                      WALKING ANIMATION STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> walkingAnimTextures;
std::vector<sf::Sprite> walkingAnimSprites;

void walkingAnimGS::init(){
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    if(debug) std::cout << "ESTADO: WalkingAnim Menu" << std::endl;
    
    if (!walkingAnimTextures["bg"].loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(walkingAnimTextures["bg"]);

    bg.setTextureRect(sf::IntRect(sf::Vector2i(1, 1), sf::Vector2i(256, 192)));

    // Adjusts menu position
    float scaleFactorWidth = gWindowWidth / 256.f;
    float scaleFactorHeight = (gWindowHeight - gGUI_size_y*2) / 192.f;

    bg.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    sf::FloatRect spriteBounds = bg.getGlobalBounds();

    float xPosition = (gWindowWidth - spriteBounds.size.x) / 2;
    float yPosition = gGUI_size_y*2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    walkingAnimSprites.push_back(bg);

    if (!walkingAnimTextures["gui"].loadFromFile("./assets/sprites/menu/guiImg.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite gui(walkingAnimTextures["gui"]);
    
    sf::Vector2u textureSize = walkingAnimTextures["gui"].getSize();
    
    float desiredWidth = 256.0f;
    float desiredHeight = 192.0f;
    
    scaleFactorWidth = desiredWidth / textureSize.x;
    scaleFactorHeight = textureSize.y / desiredHeight;
    
    gui.setScale(sf::Vector2f(0.3785, 0.475));
    
    xPosition = 0.0f;
    yPosition = 0.0f;
    gui.setPosition(sf::Vector2f(xPosition, yPosition));
        
    walkingAnimSprites.push_back(gui);    

    sf::Image cloudImage;
    if (!cloudImage.loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png"))
    {
        std::cerr << "Error loading heart image" << std::endl;
    }
    cloudImage.createMaskFromColor(gColorKeyGrey);
    cloudImage.createMaskFromColor(gColorKeyGreen);

    walkingAnimTextures["cloud"] = sf::Texture(cloudImage, false);

    sf::Sprite cloudSprite(walkingAnimTextures["cloud"], sf::IntRect({19, 194}, {32, 16}));
    cloudSprite.setScale(sf::Vector2f(2, 2));
    cloudSprite.setPosition(sf::Vector2f(360, 115));
    walkingAnimSprites.push_back(cloudSprite);

    // --------------------------------------------------
    // Animations
    // --------------------------------------------------
    
    // Simon
    sf::Image simonImg;
    if (!simonImg.loadFromFile("./assets/sprites/player/simonBelmont.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    simonImg.createMaskFromColor(gColorKeyGrey);
    simonImg.createMaskFromColor(gColorKeyGreen);
    walkingAnimTextures["simon"] = sf::Texture(simonImg, false);

    auto simonSprite = std::make_shared<sf::Sprite>(walkingAnimTextures["simon"]);

    simonSprite->setTextureRect(sf::IntRect(sf::Vector2i(29, 21), sf::Vector2i(16, 32)));

    simonSprite->setScale(sf::Vector2f(1.65, 1.65));

    float simonX = 375;
    float simonY = 296;

    simonSprite->setPosition(sf::Vector2f(simonX, simonY));

    simon = simonSprite;

    // Animation of Simon
    std::vector<AnimationManager::Frame> walkFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(29, 22), sf::Vector2(16, 30)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 31)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(63, 22), sf::Vector2(16, 30)), 0.15f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(46, 21), sf::Vector2(16, 31)), 0.15f},
    };

    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(362, 21), sf::Vector2(16, 32)), 0.1f},
    };
    
    simonManager = new AnimationManager(*simon);
    
    simonManager->addAnimation(walkSimon, walkFrames, true);
    simonManager->addAnimation(idleSimon, idleFrames, true);

    simonManager->playAnimation(walkSimon);

    // Bats
    sf::Image batImg;
    if (!batImg.loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    batImg.createMaskFromColor(gColorKeyGrey);
    batImg.createMaskFromColor(gColorKeyGreen);
    walkingAnimTextures["bat"] = sf::Texture(batImg, false);

    auto batSprite1 = std::make_shared<sf::Sprite>(walkingAnimTextures["bat"]);
    auto batSprite2 = std::make_shared<sf::Sprite>(walkingAnimTextures["bat"]);

    batSprite1->setTextureRect(sf::IntRect(sf::Vector2i(1, 194), sf::Vector2i(8, 8)));
    batSprite2->setTextureRect(sf::IntRect(sf::Vector2i(1, 194), sf::Vector2i(8, 8)));

    batSprite1->setScale(sf::Vector2f(1.75, 1.75));
    batSprite2->setScale(sf::Vector2f(1.75, 1.75));

    batSprite1->setPosition(sf::Vector2f(225, 100));
    batSprite2->setPosition(sf::Vector2f(35, 220));

    bat1 = batSprite1;
    bat2 = batSprite2;

    // Animation of Simon
    std::vector<AnimationManager::Frame> flyingFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(1, 194), sf::Vector2i(8, 8)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(10, 194), sf::Vector2i(8, 8)), 0.05f},
    };

    batManager1 = new AnimationManager(*bat1);
    batManager2 = new AnimationManager(*bat2);
    
    batManager1->addAnimation(flyBat, flyingFrames, true);
    batManager2->addAnimation(flyBat, flyingFrames, true);

    batManager1->playAnimation(flyBat);
    batManager2->playAnimation(flyBat);

    auto audio = configManager.getAudio();

    walkingAnimSoundManager.loadMusic("animMusic", "./assets/music/02Prologue.mp3");
    walkingAnimSoundManager.playMusic("animMusic", walkingAnimSoundManager.realVolume(audio.master_volume, audio.music_volume), false);
}

void walkingAnimGS::handleInput(sf::Event event){
}

void walkingAnimGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    if(walkingAnimSoundManager.musicHasFinished("animMusic")){
        stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
    }

    // Simon things
    sf::Vector2f posSimon = simon->getPosition();
    const float simonSpeed = 60.0f; // pixels per second
    
    if (posSimon.x <= 186) {
        simon->setPosition(sf::Vector2f(186, posSimon.y));
        simonManager->playAnimation(idleSimon);
    } else {
        simon->setPosition(sf::Vector2f(posSimon.x - simonSpeed * deltaTime, posSimon.y));
    }
    simonManager->update(deltaTime);

    // Bats things

    elapsedTime += deltaTime;
    float t = elapsedTime / 6;
    float newX = 225 - 6.666 * elapsedTime;

    float curveOffset = 4 * (-25) * t * (1 - t);
    float newY = 100 - curveOffset;

    bat1->setPosition(sf::Vector2f(newX, newY));
    batManager1->update(deltaTime);

    sf::Vector2f posBat2 = bat2->getPosition();
    const float bat2SpeedX = 18.0f; // pixels per second (0.3 * 60fps)
    const float bat2SpeedY = -18.0f; // pixels per second (-0.3 * 60fps)
    bat2->setPosition(sf::Vector2f(posBat2.x + bat2SpeedX * deltaTime, posBat2.y + bat2SpeedY * deltaTime));
    batManager2->update(deltaTime);

    // Cloud things
    sf::Sprite cloud = walkingAnimSprites.back();
    walkingAnimSprites.pop_back();

    sf::Vector2f posCloud = cloud.getPosition();
    const float cloudSpeedX = -15.0f; // pixels per second (-0.25 * 60fps)
    cloud.setPosition(sf::Vector2f(posCloud.x + cloudSpeedX * deltaTime, posCloud.y));

    walkingAnimSprites.push_back(cloud);
}

void walkingAnimGS::draw(sf::RenderWindow& window, Camera& camera){
    if (restartLoadingClock)
    {
        restartLoadingClock = false;
        loadingClock.restart();
    }

    if (loadingClock.getElapsedTime().asSeconds() < gLoadingTime)
    {
        sf::RectangleShape blackScreen(camera.getView(window.getSize()).getSize());
        blackScreen.setFillColor(sf::Color::Black);
        window.draw(blackScreen);
    }
    else{
        for (const auto& sprite : walkingAnimSprites) {
            window.draw(sprite);
        }

        window.draw(*simon);
        window.draw(*bat1);
        window.draw(*bat2);
    }
}
 
void walkingAnimGS::pause(){
    if(debug) std::cout << "ESTADO: WalkingAnim PAUSADO" << std::endl;
}

void walkingAnimGS::resume(){
    if(debug) std::cout << "ESTADO: WalkingAnim REANUDADO" << std::endl;
}

void walkingAnimGS::close(){
    if(debug) std::cout << "ESTADO: WalkingAnim CERRADO" << std::endl;
    walkingAnimSprites.clear();
    walkingAnimTextures.clear();
}

walkingAnimGS::~walkingAnimGS() {}


// ======================================================
//                      PAUSE STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> pauseTextures;
std::vector<sf::Sprite> pauseSprites;

void PauseGS::init(){
    if(debug) std::cout << "ESTADO: Pause" << std::endl;
    // BG GAMEPLAY PAUSED
    // PRETTY EDGES
    // PAUSED GAME
    // VOLUMES
    // CLOSE / BACK TO MENU

    auto audio = configManager.getAudio();
    masterVol = audio.master_volume;
    musicVol = audio.music_volume;
    soundVol = audio.sound_volume;
    cheats = configManager.getCheats().enabled;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    position = 4;
    col = 1;
    showPopUp = false;
    colPopUp = 0;

    if (!pauseTextures["bg"].loadFromFile("./assets/sprites/menu/blankBox.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(pauseTextures["bg"]);

    bg.setScale(sf::Vector2f(0.5, 2));

    sf::FloatRect spriteBounds = bg.getGlobalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    bg.setPosition(sf::Vector2f((gWindowWidth - spriteWidth) / 2, (gWindowHeight - spriteHeight) / 2));

    pauseSprites.push_back(bg);

    if (!pauseTextures["popUpExit"].loadFromFile("./assets/sprites/menu/blankBox.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite popUpExit(pauseTextures["popUpExit"]);

    // Adjusts menu position
    popUpExit.setScale(sf::Vector2f(0.65, 0.65));

    spriteBounds = popUpExit.getGlobalBounds();

    float xPosition = (gWindowWidth - spriteBounds.size.x) / 2;
    float yPosition = (gWindowHeight - spriteBounds.size.y) / 2;

    popUpExit.setPosition(sf::Vector2f(xPosition, yPosition));

    pauseSprites.push_back(popUpExit);

    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    sf::Text textPopUp(font, "BACK", 30);
    textPopUp.setFillColor(sf::Color::White);
    sf::FloatRect textPopUpBounds = textPopUp.getLocalBounds();

    // Centers position
    float xPopPos = 70;
    float yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) + 20;

    textPopUp.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp);

    sf::Text textPopUp2(font, "EXIT", 30);
    textPopUp2.setFillColor(sf::Color(122, 71, 22));
    textPopUp2.setOutlineColor(sf::Color(255, 140, 0));
    textPopUp2.setOutlineThickness(1.5);

    textPopUpBounds = textPopUp2.getLocalBounds();

    // Centers position
    xPopPos = 250;
    yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) + 20;

    textPopUp2.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp2);

    sf::Text textPopUp3(font, "ARE YOU SURE...", 20);
    textPopUp3.setFillColor(sf::Color::White);
    textPopUpBounds = textPopUp3.getLocalBounds();

    // Centers position
    xPopPos = ((gWindowWidth - textPopUpBounds.size.x) / 2);
    yPopPos = ((gWindowHeight - textPopUpBounds.size.y) / 2) - 25;

    textPopUp3.setPosition(sf::Vector2f(xPopPos, yPopPos));
    popUpText.push_back(textPopUp3);

    if (!fontinputs.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    fontinputs.setSmooth(false);

    sf::Text text(font, "PAUSED GAME", 20);
    text.setFillColor(sf::Color(122, 71, 22));
    text.setOutlineColor(sf::Color(255, 140, 0));
    text.setOutlineThickness(1.5); 
    sf::FloatRect textBounds = text.getLocalBounds();

    // Centers position
    float xPos = (gWindowWidth - textBounds.size.x) / 2;
    float yPos = 80.f;

    text.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text);

    std::string textos[3] = {"MASTER", "MUSIC", "SOUNDS"};
    for (int i = 0; i < 3; i++) {
        sf::Text text(font, textos[i], 15);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 115.f + i * 50.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    // CHEATS
    sf::Text text2(font, "CHEATS", 15);
    text2.setFillColor(sf::Color::White);
    textBounds = text2.getLocalBounds();

    xPos = (gWindowWidth - textBounds.size.x) / 2.f  - 25.f;
    yPos = 270.f;

    text2.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text2);
    
    // BACK
    sf::Text text3(font, "EXIT", 20);
    text3.setFillColor(sf::Color::White);
    text3.setOutlineColor(sf::Color(128, 128, 128));
    text3.setOutlineThickness(1.5);
    textBounds = text3.getLocalBounds();

    xPos = (gWindowWidth - textBounds.size.x) / 2.f  - 75.f;
    yPos = 300.f;

    text3.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text3);

    // CONFIRM
    sf::Text text4(font, "RESUME", 20);
    text4.setFillColor(sf::Color(0, 190, 0));
    text4.setOutlineColor(sf::Color(0, 100, 0));
    text4.setOutlineThickness(1.5);
    textBounds = text4.getLocalBounds();

    xPos = (gWindowWidth - textBounds.size.x) / 2.f  + 65.f;
    yPos = 300.f;

    text4.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text4);

    if (!pauseTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(pauseTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.25f, torch.getScale().y * 1.25f));

    float torchX = configs[6].getPosition().x - 25.f;
    float torchY = configs[6].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    pauseSprites.push_back(torch);

    pauseVolumeManager.loadSound("menuEnter", "./assets/sounds/05.wav");
}

void PauseGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if(!showPopUp){
            if (keyPressed->scancode == controls.escape)
            {
                auto audio = configManager.getAudio();
                gameSoundManager.adjustAllMusicVolumes(pauseVolumeManager.realVolume(audio.master_volume, audio.music_volume));
                stateMachine->removeState();
            }

            if (keyPressed->scancode == controls.down && position < 4) {    
                position ++;
            }

            if (keyPressed->scancode == controls.up && position > 0) {    
                position --;
            }

            if (keyPressed->scancode == controls.right) {    
                switch (position){
                    case 0:
                        if(masterVol < 100){
                            masterVol += 10;
                        }
                        break;
                    case 1:
                        if(musicVol < 100){
                            musicVol += 10;
                        }
                        break;
                    case 2:
                        if(soundVol < 100){
                            soundVol += 10;
                        }
                        pauseVolumeManager.playSound("menuEnter", pauseVolumeManager.realVolume(masterVol, soundVol));
                        break;
                    case 4:
                        if (col == 0){
                            col = 1;
                        }
                        break;
                }
                pauseVolumeManager.adjustAllMusicVolumes(pauseVolumeManager.realVolume(masterVol, musicVol));
                gameSoundManager.adjustAllMusicVolumes(pauseVolumeManager.realVolume(masterVol, musicVol));
            }

            if (keyPressed->scancode == controls.left) {    
                switch (position){
                    case 0:
                        if(masterVol > 0){
                            masterVol -= 10;
                        }
                        break;
                    case 1:
                        if(musicVol > 0){
                            musicVol -= 10;
                        }
                        break;
                    case 2:
                        if(soundVol > 0){
                            soundVol -= 10;
                        }
                        pauseVolumeManager.playSound("menuEnter", pauseVolumeManager.realVolume(masterVol, soundVol));
                        break;
                    case 4:
                        if (col == 1){
                            col = 0;
                        }
                        break;
                }
                pauseVolumeManager.adjustAllMusicVolumes(pauseVolumeManager.realVolume(masterVol, musicVol));
                gameSoundManager.adjustAllMusicVolumes(pauseVolumeManager.realVolume(masterVol, musicVol));
            }

            if (!pauseSprites.empty()) {
                sf::Sprite torch = pauseSprites.back();
                pauseSprites.pop_back();
                float torchX = 0;
                float torchY = 0;
                if (position <= 3){
                    torchX = configs[position+1].getPosition().x - 25.f;
                    torchY = configs[position+1].getPosition().y + 2.f;
                }
                else{
                    torchX = configs[position+1+col].getPosition().x - 25.f;
                    torchY = configs[position+1+col].getPosition().y + 2.f;
                }
                torch.setPosition(sf::Vector2f(torchX, torchY));

                pauseSprites.push_back(torch);
            }

            if (keyPressed->scancode == controls.enter) {
                if (position == 3){ // Cheats
                    cheats = !cheats;
                }

                if (position == 4 && col == 0){
                    std::cout << "Road back to menu" << std::endl;
                    colPopUp = 0;
                    showPopUp = true;
                    if (!pauseSprites.empty()) {
                        sf::Sprite torch = pauseSprites.back();
                        pauseSprites.pop_back();

                        float torchX = popUpText[colPopUp].getPosition().x - 25.f;
                        float torchY = popUpText[colPopUp].getPosition().y + 2.f;
                        torch.setPosition(sf::Vector2f(torchX, torchY));

                        pauseSprites.push_back(torch);
                    }
                }

                if (position == 4 && col == 1){
                    configManager::Audio newAudio;
                    newAudio.master_volume = masterVol;
                    newAudio.music_volume = musicVol;
                    newAudio.sound_volume = soundVol;

                    configManager::Cheats newCheats;
                    newCheats.enabled = cheats;
                    
                    configManager.setAudio(newAudio);
                    configManager.setCheats(newCheats);
                    configManager.saveConfiguration("config.json");
                    std::cout << "Road back to game after saving" << std::endl;
                    stateMachine->removeState();
                }
            }
            std::cout<<"Position: "<< position << ", Col: " << col <<std::endl;
        }
        else{
            if (keyPressed->scancode == controls.right && colPopUp == 0){
                colPopUp = 1;
            }
            if (keyPressed->scancode == controls.left && colPopUp == 1){
                colPopUp = 0;
            }
            if (!pauseSprites.empty()) {
                sf::Sprite torch = pauseSprites.back();
                pauseSprites.pop_back();

                float torchX = popUpText[colPopUp].getPosition().x - 25.f;
                float torchY = popUpText[colPopUp].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                pauseSprites.push_back(torch);
            }
            if (keyPressed->scancode == controls.enter){
                if(colPopUp == 0){
                    showPopUp = false;
                    if (!pauseSprites.empty()) {
                        sf::Sprite torch = pauseSprites.back();
                        pauseSprites.pop_back();
                        float torchX = 0;
                        float torchY = 0;
                        if (position <= 3){
                            torchX = configs[position+1].getPosition().x - 25.f;
                            torchY = configs[position+1].getPosition().y + 2.f;
                        }
                        else{
                            torchX = configs[position+1+col].getPosition().x - 25.f;
                            torchY = configs[position+1+col].getPosition().y + 2.f;
                        }
                        torch.setPosition(sf::Vector2f(torchX, torchY));

                        pauseSprites.push_back(torch);
                    }
                }
                if(colPopUp == 1){
                    gameSoundManager.stopAllMusic();
                    // Reset of the game for story mode
                    gStartingLevel = 1;
                    gStartingStage = 1;
                    stateMachine->replaceAllStates(std::make_unique<MenuGS>(stateMachine));
                }
            }
        }
    }
}

void PauseGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    
}

void PauseGS::draw(sf::RenderWindow& window, Camera& camera){
    window.draw(pauseSprites[0]);
    for (const auto& text : configs) {
        window.draw(text);
    }

    std::string boolText = cheats ? "ON" : "OFF";

    sf::Text statusText(fontinputs, boolText, 10);
    statusText.setFillColor(sf::Color(255, 140, 0));

    // Posición a la derecha del texto base
    float x = configs[4].getPosition().x + configs[4].getLocalBounds().size.x + 20.f;
    float y = configs[4].getPosition().y + 2.5f;
    statusText.setPosition(sf::Vector2f(x, y));

    window.draw(statusText);

    drawVolumeBars(window, masterVol, sf::Vector2f(125, 140));
    drawVolumeBars(window, musicVol, sf::Vector2f(125, 190));
    drawVolumeBars(window, soundVol, sf::Vector2f(125, 240));

    if(showPopUp){
        window.draw(pauseSprites[1]);
        for (const auto& text : popUpText) {
            window.draw(text);
        }
    }
    window.draw(pauseSprites[2]);
}

void PauseGS::drawVolumeBars(sf::RenderWindow& window, int actualVol, sf::Vector2f pos){
    const int MAX_VOLUME = 10;
    const int SEGMENT_WIDTH = 13;
    const int SEGMENT_HEIGHT = 15;
    const int SPACING = 2.5;
    const float BORDER_THICKNESS = 1.0f;

    // Player
    for (int i = 0; i < MAX_VOLUME; ++i)
    {
        bool isFull = (i < actualVol/10);

        float width = SEGMENT_WIDTH - (isFull ? 0 : BORDER_THICKNESS * 2);
        float height = SEGMENT_HEIGHT - (isFull ? 0 : BORDER_THICKNESS * 2);
        sf::RectangleShape segment(sf::Vector2f(width, height));

        // Adjust the position to center the empty segments within the border
        float xOffset = isFull ? 0 : BORDER_THICKNESS;
        float yOffset = isFull ? 0 : BORDER_THICKNESS;
        segment.setPosition(sf::Vector2f(pos.x + i * (SEGMENT_WIDTH + SPACING) + xOffset, pos.y + yOffset));

        segment.setOutlineThickness(BORDER_THICKNESS);
        segment.setOutlineColor(isFull ? sf::Color::Black : sf::Color::White);
        segment.setFillColor(isFull ? sf::Color::Red : sf::Color::Black);

        window.draw(segment);
    }
}

void PauseGS::pause(){
    if(debug) std::cout << "ESTADO: Pause PAUSADO" << std::endl;
}

void PauseGS::resume(){
    if(debug) std::cout << "ESTADO: Pause REANUDADO" << std::endl;
}

void PauseGS::close(){
    if(debug) std::cout << "ESTADO: Pause CERRADO" << std::endl;
    pauseSprites.clear();
    pauseTextures.clear();
    configs.clear();
    popUpText.clear();
}

PauseGS::~PauseGS() {}


// ======================================================
//                      CONFIG STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> configTextures;
std::vector<sf::Sprite> configSprites;

void ConfigGS::init(){
    if(debug) std::cout << "ESTADO: Config" << std::endl;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    position = 0;
    enterPressed = false;
    if (!configTextures["bg"].loadFromFile("./assets/sprites/menu/menu1.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(configTextures["bg"]);

    // Adjusts menu position
    sf::FloatRect spriteBounds = bg.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    bg.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    configSprites.push_back(bg);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    sf::Text text(font, "CONFIGURATION", 35);
    text.setFillColor(sf::Color(122, 71, 22));
    text.setOutlineColor(sf::Color(255, 140, 0));
    text.setOutlineThickness(1.5); 
    sf::FloatRect textBounds = text.getLocalBounds();

    // Centers position
    float xPos = (gWindowWidth - textBounds.size.x) / 2;
    float yPos = 70.f;

    text.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text);

    // Defines menu options
    std::string textos[4] = {"CONTROLS", "SOUND", "GAMEPLAY", "BACK"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 150.f + i * 45.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    if (!configTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(configTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = configs[1].getPosition().x - 25.f;
    float torchY = configs[1].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    configSprites.push_back(torch);

    configSoundManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    auto audio = configManager.getAudio();

    configSoundManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");
    configSoundManager.playMusic("menuMusic", configSoundManager.realVolume(audio.master_volume, audio.music_volume));
}

void ConfigGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if (keyPressed->scancode == controls.down && position < 3) {    
                if (!configSprites.empty()) {
                    sf::Sprite torch = configSprites.back();
                    configSprites.pop_back();

                    position ++;
                    std::cout<<position<<std::endl;

                    float torchX = configs[position+1].getPosition().x - 25.f;
                    float torchY = configs[position+1].getPosition().y + 2.f;
                    torch.setPosition(sf::Vector2f(torchX, torchY));

                    configSprites.push_back(torch);
                }
            }

            if (keyPressed->scancode == controls.up && position > 0) {    
                if (!configSprites.empty()) {
                    sf::Sprite torch = configSprites.back();
                    configSprites.pop_back();

                    position --;
                    std::cout<<position<<std::endl;

                    float torchX = configs[position+1].getPosition().x - 25.f;
                    float torchY = configs[position+1].getPosition().y + 2.f;
                    torch.setPosition(sf::Vector2f(torchX, torchY));

                    configSprites.push_back(torch);
                }
            }

            if (keyPressed->scancode == controls.enter) {
                enterPressed = true;
                auto audio = configManager.getAudio();
                configSoundManager.playSound("menuEnter", configSoundManager.realVolume(audio.master_volume, audio.sound_volume));
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                switch (position) {
                    case 0: // Controls
                        std::cout << "Road to controls config" << std::endl;
                        stateMachine->replaceState(std::make_unique<ControlsConfGS>(stateMachine));
                        break;
                    case 1: // Sound
                        std::cout << "Road to sound config" << std::endl;
                        stateMachine->replaceState(std::make_unique<VolumeConfGS>(stateMachine));
                        break;
                    case 2: // Gameplay and video
                        std::cout << "Road to gameplay and video config" << std::endl;
                        stateMachine->replaceState(std::make_unique<GameplayConfGS>(stateMachine));
                        break;
                    case 3: // Back
                        std::cout << "Road back to menu" << std::endl;
                        stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
                        break;
                }
            }
            /* Add or remove???
            if (keyPressed->scancode == controls.escape){
                stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
            }
            */
        }
    }
}

void ConfigGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    
}

void ConfigGS::draw(sf::RenderWindow& window, Camera& camera){
    for (const auto& sprite : configSprites) {
        window.draw(sprite);
    }

    for (const auto& text : configs) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }
}

void ConfigGS::pause(){
    if(debug) std::cout << "ESTADO: Config PAUSADO" << std::endl;
}

void ConfigGS::resume(){
    if(debug) std::cout << "ESTADO: Config REANUDADO" << std::endl;
}

void ConfigGS::close(){
    if(debug) std::cout << "ESTADO: Config CERRADO" << std::endl;
    configSprites.clear();
    configTextures.clear();
    configs.clear();
}

ConfigGS::~ConfigGS() {}


// ======================================================
//                      CONTROLS CONFIG STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> controlsConfigTextures;
std::vector<sf::Sprite> controlsConfigSprites;
std::vector<sf::Sprite> popUpControlsConfigSprite;

void ControlsConfGS::init(){
    if(debug) std::cout << "ESTADO: Controls Config" << std::endl;
    auto controls = configManager.getControls();
    right = configManager.scancodeToString(controls.right);
    left = configManager.scancodeToString(controls.left);
    down = configManager.scancodeToString(controls.down);
    up = configManager.scancodeToString(controls.up);
    jump = configManager.scancodeToString(controls.jump);
    attack = configManager.scancodeToString(controls.attack);
    enter = configManager.scancodeToString(controls.enter);
    escape = configManager.scancodeToString(controls.escape);
    useSubWeapon = configManager.scancodeToString(controls.useSubWeapon);

    defRight = "Right";
    defLeft = "Left";
    defDown = "Down";
    defUp = "Up";
    defJump = "X";
    defAttack = "Z";
    defEnter = "Enter";
    defEscape = "Esc";
    defUseSubWeapon = "C";

    waitingInput = false;
    showPopUp = false;
    enterPressed = false;

    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    position = 0;
    col = 0;
    if (!controlsConfigTextures["bg"].loadFromFile("./assets/sprites/menu/menu1.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(controlsConfigTextures["bg"]);

    // Adjusts menu position
    sf::FloatRect spriteBounds = bg.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    bg.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    controlsConfigSprites.push_back(bg);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);
    
    if (!fontinputs.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    fontinputs.setSmooth(false);

    sf::Text text(font, "Controls", 35);
    text.setFillColor(sf::Color(122, 71, 22));
    text.setOutlineColor(sf::Color(255, 140, 0));
    text.setOutlineThickness(1.5); 
    sf::FloatRect textBounds = text.getLocalBounds();

    // Centers position
    float xPos = (gWindowWidth - textBounds.size.x) / 2;
    float yPos = 50.f;

    text.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text);

    // Defines menu options
    std::string textos[9] = {"RIGHT", "LEFT", "DOWN", "UP", "JUMP", "ATTACK", "SUBWEAPON", "ENTER", "ESCAPE"};
    for (int i = 0; i < 9; i++) {
        sf::Text text(font, textos[i], 20);
        text.setFillColor(sf::Color::White);
    
        int col = (i < 5) ? 0 : 1;
        int row = (i < 5) ? i : i - 5;
    
        float spacingY = 30.f;
        float xPos = (col == 0) ? 35.f : gWindowWidth / 2.f + 15;
        float yPos = 110.f + row * spacingY;
    
        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    // SET DEFAULT CONTROLS
    sf::Text text2(font, "DEFAULT", 30);
    text2.setFillColor(sf::Color(255, 165, 0));
    text2.setOutlineColor(sf::Color(200, 110, 0));
    text2.setOutlineThickness(1.5);
    textBounds = text2.getLocalBounds();

    xPos = (gWindowWidth - textBounds.size.x) / 2;
    yPos = 270.f;

    text2.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text2);

    // BACK
    sf::Text text3(font, "BACK", 30);
    text3.setFillColor(sf::Color::White);
    text3.setOutlineColor(sf::Color(128, 128, 128));
    text3.setOutlineThickness(1.5);
    textBounds = text3.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x - 35.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text3.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text3);

    // CONFIRM
    sf::Text text4(font, "CONFIRM", 30);
    text4.setFillColor(sf::Color(0, 190, 0));
    text4.setOutlineColor(sf::Color(0, 100, 0));
    text4.setOutlineThickness(1.5);
    textBounds = text4.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x + 175.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text4.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text4);

    if (!controlsConfigTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(controlsConfigTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = configs[1].getPosition().x - 25.f;
    float torchY = configs[1].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    controlsConfigSprites.push_back(torch);

    if (!controlsConfigTextures["popUpControls"].loadFromFile("./assets/sprites/menu/popUpControls.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite popUpControls(controlsConfigTextures["popUpControls"]);

    // Adjusts popUp position
    popUpControls.setScale(sf::Vector2f(0.6, 0.6));

    spriteBounds = popUpControls.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    popUpControls.setPosition(sf::Vector2f((gWindowWidth - spriteWidth) / 2, (gWindowHeight - spriteHeight) / 2));

    popUpControlsConfigSprite.push_back(popUpControls);

    configControlsManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    configControlsManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");

    auto audio = configManager.getAudio();
    configControlsManager.playMusic("menuMusic", configControlsManager.realVolume(audio.master_volume, audio.music_volume));
}

void ControlsConfGS::setDefault(){
    right = defRight;
    left = defLeft;
    down = defDown;
    up = defUp;
    jump = defJump; 
    attack = defAttack;
    enter = defEnter;
    escape = defEscape;
    useSubWeapon = defUseSubWeapon;
}

bool ControlsConfGS::checkInputErrors(){
    if (right == "Unknown" || left == "Unknown" || down == "Unknown" || 
        up == "Unknown" || jump == "Unknown" || attack == "Unknown" || 
        enter == "Unknown" || escape == "Unknown" || useSubWeapon == "Unknown") {
        return true; // Si alguna es "Unknown", retornamos true
    }

    // Check for duplicate keys
    std::unordered_set<std::string> inputs = {
        right, left, down, up, jump, attack, enter, escape, useSubWeapon
    };

    // If there are fewer unique items than the total, there are duplicates
    if (inputs.size() < 9) {
        return true;
    }

    return false;
}

void ControlsConfGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if (!waitingInput){ // If we are not waiting an input normal movement of the torch
                if(!showPopUp){  // If the popUp is displayed, do not respond
                    if (keyPressed->scancode == controls.enter){
                        auto audio = configManager.getAudio();
                        configControlsManager.playSound("menuEnter", configControlsManager.realVolume(audio.master_volume, audio.sound_volume));

                        if ((position <= 4 && col == 0) || (position <= 3 && col == 1)){
                            waitingInput = true;
                        }

                        if ((position == 5 && col == 0) || (position == 4 && col == 1)){
                            std::cout << "Default" << std::endl;
                            setDefault();
                            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                        }

                        if (position == 6 && col == 0){
                            enterPressed = true;
                            std::cout << "Back" << std::endl;
                            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                            stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                        }

                        if (position == 5 && col == 1){
                            std::cout << "Back and save :)" << std::endl;
                            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                            if (checkInputErrors()){
                                std::cout << "At least one with a bad input or a duplicated input, setting default" << std::endl;
                                showPopUp = true; // ADD POP UP AND DONT GO TO OTHER WINDOW
                            }
                            else{
                                enterPressed = true;
                                configManager::Controls newControls;
                                newControls.right = configManager.stringToScancode(right);
                                newControls.left = configManager.stringToScancode(left);
                                newControls.down = configManager.stringToScancode(down);
                                newControls.up = configManager.stringToScancode(up);
                                newControls.jump = configManager.stringToScancode(jump); 
                                newControls.attack = configManager.stringToScancode(attack);
                                newControls.enter = configManager.stringToScancode(enter);
                                newControls.escape = configManager.stringToScancode(escape);
                                newControls.useSubWeapon = configManager.stringToScancode(useSubWeapon);

                                configManager.setControls(newControls);
                                configManager.saveConfiguration("config.json");
                                stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                            }
                        }
                    }
                    
                    // Move down
                    if (keyPressed->scancode == controls.down){
                        int maxRows = (col == 0) ? 6 : 5;
                        if (position < maxRows){
                            position++;
                        }
                    }

                    // Move up
                    if (keyPressed->scancode == controls.up){
                        if (position > 0) {
                            position--;
                        }
                    }

                    // Move right
                    if (keyPressed->scancode == controls.right){
                        if (col == 0 && position <= 3){
                            col = 1;
                        }
                        else if (col == 0 && position == 4){
                            position = 3;
                            col = 1;
                        }
                        else if (col == 0 && position == 6){
                            position = 5;
                            col = 1;
                        }
                    }

                    // Move left
                    if (keyPressed->scancode == controls.left){
                        if (col == 1 && position <= 3){
                            col = 0;
                        }
                        else if(col == 1 && position == 5){
                            position = 6;
                            col = 0;
                        }
                    }

                    // Update torch position
                    if (!controlsConfigSprites.empty()){
                        sf::Sprite torch = controlsConfigSprites.back();
                        controlsConfigSprites.pop_back();

                        int index = 0;
                        if (position == 5 && col == 1){
                            index = 12;
                        }
                        else if (position == 6 && col == 0){
                            index = 11;
                        }
                        else if (position == 5){
                            index = 10;
                        }
                        else{
                            index = (col == 0) ? position : 5 + position;
                            index++;
                        }

                        float torchX = configs[index].getPosition().x - 25.f;
                        float torchY = configs[index].getPosition().y + 2.f;
                        torch.setPosition(sf::Vector2f(torchX, torchY));

                        controlsConfigSprites.push_back(torch);

                        std::cout << "Fila: " << position << ", Columna: " << col << ", Index: " << index << std::endl;
                    }
                }
            }
            else{
                if (col == 0){
                    switch (position){
                        case 0: // Right
                            right = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 1: // Left
                            left = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 2: // Down
                            down = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 3: // Up
                            up = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 4: // Jump
                            jump = configManager.scancodeToString(keyPressed->scancode);
                            break;
                    }
                }
                else{ // col == 1
                    switch (position){
                        case 0: // Attack
                            attack = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 1: // Subweapon
                            useSubWeapon = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 2: // Enter
                            enter = configManager.scancodeToString(keyPressed->scancode);
                            break;
                        case 3: // Escape
                            escape = configManager.scancodeToString(keyPressed->scancode);
                            break;
                    }
                }
                waitingInput = false;
            }
        }
    }
}

void ControlsConfGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    sf::Sprite torch = controlsConfigSprites.back();
    controlsConfigSprites.pop_back();
    if (waitingInput) {
        blinkTimer += deltaTime;

        if (blinkTimer >= blinkInterval) {
            torch.setColor(torch.getColor() == sf::Color::Transparent ? sf::Color::White : sf::Color::Transparent);
            blinkTimer = 0.0f;
        }
    } else {
        // Si no estamos esperando input, aseguramos que la antorcha esté visible
        torch.setColor(sf::Color::White);
    }
    controlsConfigSprites.push_back(torch);

    sf::Sprite popUp = popUpControlsConfigSprite.back();
    popUpControlsConfigSprite.pop_back();
    if (showPopUp){
        popUp.setColor(sf::Color::White);
        popUpTimer += deltaTime;

        if(popUpTimer >= popUpInterval){
            popUp.setColor(sf::Color::Transparent);
            popUpTimer = 0.0f;
            showPopUp = false;
        }
    }
    else{
        popUp.setColor(sf::Color::Transparent);
    }
    popUpControlsConfigSprite.push_back(popUp);
}

void ControlsConfGS::draw(sf::RenderWindow& window, Camera& camera){
    for (const auto& sprite : controlsConfigSprites) {
        window.draw(sprite);
    }

    std::vector<std::string> currentControls = {
        right, left, down, up, jump, attack, useSubWeapon, enter, escape
    };

    for (size_t i = 0; i < configs.size(); ++i) {
        const sf::Text& label = configs[i];
        window.draw(label);

        // Only add control text for options 1 to 9
        if (i >= 1 && i <= 9 && (i - 1) < currentControls.size()) {
            sf::Text controlText(fontinputs, currentControls[i - 1], 10);
            controlText.setFillColor(sf::Color(255, 140, 0));

            sf::Vector2f pos = label.getPosition();
            pos.x += label.getLocalBounds().size.x + 10;
            pos.y += 2.5;
            controlText.setPosition(pos);

            window.draw(controlText);
        }
    }

    // Draw the popUp
    window.draw(popUpControlsConfigSprite[0]);
}

void ControlsConfGS::pause(){
    if(debug) std::cout << "ESTADO: Controls Config PAUSADO" << std::endl;
}

void ControlsConfGS::resume(){
    if(debug) std::cout << "ESTADO: Controls Config REANUDADO" << std::endl;
}

void ControlsConfGS::close(){
    if(debug) std::cout << "ESTADO: Controls Config CERRADO" << std::endl;
    controlsConfigSprites.clear();
    controlsConfigTextures.clear();
    configs.clear();
}

ControlsConfGS::~ControlsConfGS() {}


// ======================================================
//                      VOLUME CONFIG STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> volumeConfigTextures;
std::vector<sf::Sprite> volumeConfigSprites;

void VolumeConfGS::init(){
    if(debug) std::cout << "ESTADO: Volume Config" << std::endl;
    auto audio = configManager.getAudio();
    masterVol = audio.master_volume;
    musicVol = audio.music_volume;
    soundVol = audio.sound_volume;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    position = 0;
    col = 0;
    enterPressed = false;
    if (!volumeConfigTextures["bg"].loadFromFile("./assets/sprites/menu/menu1.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(volumeConfigTextures["bg"]);

    // Adjusts menu position
    sf::FloatRect spriteBounds = bg.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    bg.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    volumeConfigSprites.push_back(bg);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    sf::Text text(font, "SOUND", 35);
    text.setFillColor(sf::Color(122, 71, 22));
    text.setOutlineColor(sf::Color(255, 140, 0));
    text.setOutlineThickness(1.5); 
    sf::FloatRect textBounds = text.getLocalBounds();

    // Centers position
    float xPos = (gWindowWidth - textBounds.size.x) / 2;
    float yPos = 50.f;

    text.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text);

    // Defines menu options
    std::string textos[3] = {"MASTER AUDIO", "MUSIC", "SOUNDS"};
    for (int i = 0; i < 3; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 115.f + i * 65.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    // BACK
    sf::Text text2(font, "BACK", 30);
    text2.setFillColor(sf::Color::White);
    text2.setOutlineColor(sf::Color(128, 128, 128));
    text2.setOutlineThickness(1.5);
    textBounds = text2.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x - 35.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text2.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text2);

    // CONFIRM
    sf::Text text3(font, "CONFIRM", 30);
    text3.setFillColor(sf::Color(0, 190, 0));
    text3.setOutlineColor(sf::Color(0, 100, 0));
    text3.setOutlineThickness(1.5);
    textBounds = text3.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x + 175.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text3.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text3);

    if (!volumeConfigTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(volumeConfigTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = configs[1].getPosition().x - 25.f;
    float torchY = configs[1].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    volumeConfigSprites.push_back(torch);

    configVolumeManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    configVolumeManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");
    configVolumeManager.playMusic("menuMusic", configVolumeManager.realVolume(audio.master_volume, audio.music_volume));
}

void VolumeConfGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if (keyPressed->scancode == controls.down && position < 3) {    
                position ++;
            }

            if (keyPressed->scancode == controls.up && position > 0) {    
                position --;
            }

            if (keyPressed->scancode == controls.right) {    
                switch (position){
                    case 0:
                        if(masterVol < 100){
                            masterVol += 10;
                        }
                        break;
                    case 1:
                        if(musicVol < 100){
                            musicVol += 10;
                        }
                        break;
                    case 2:
                        if(soundVol < 100){
                            soundVol += 10;
                        }
                        configVolumeManager.playSound("menuEnter", configVolumeManager.realVolume(masterVol, soundVol));
                        break;
                    case 3:
                        if (col == 0){
                            col = 1;
                        }
                        break;
                }
                configVolumeManager.adjustAllMusicVolumes(configVolumeManager.realVolume(masterVol, musicVol));
            }

            if (keyPressed->scancode == controls.left) {    
                switch (position){
                    case 0:
                        if(masterVol > 0){
                            masterVol -= 10;
                        }
                        break;
                    case 1:
                        if(musicVol > 0){
                            musicVol -= 10;
                        }
                        break;
                    case 2:
                        if(soundVol > 0){
                            soundVol -= 10;
                        }
                        configVolumeManager.playSound("menuEnter", configVolumeManager.realVolume(masterVol, soundVol));
                        break;
                    case 3:
                        if (col == 1){
                            col = 0;
                        }
                        break;
                }
                configVolumeManager.adjustAllMusicVolumes(configVolumeManager.realVolume(masterVol, musicVol));
            }

            if (keyPressed->scancode == controls.enter) {
                if (position == 3 && col == 0){
                    enterPressed = true;
                    auto audio = configManager.getAudio();
                    configVolumeManager.playSound("menuEnter", configVolumeManager.realVolume(audio.master_volume, audio.sound_volume));

                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    std::cout << "Road back to config menu" << std::endl;
                    stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                }

                if (position == 3 && col == 1){ // Back to configs and save files
                    enterPressed = true;
                    configManager::Audio newAudio;
                    newAudio.master_volume = masterVol;
                    newAudio.music_volume = musicVol;
                    newAudio.sound_volume = soundVol;

                    configManager.setAudio(newAudio);
                    configManager.saveConfiguration("config.json");

                    auto audio = configManager.getAudio();
                    configVolumeManager.playSound("menuEnter", configVolumeManager.realVolume(audio.master_volume, audio.sound_volume));

                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    std::cout << "Road back to config menu after saving" << std::endl;
                    stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                }
            }

            if (!volumeConfigSprites.empty()) {
                sf::Sprite torch = volumeConfigSprites.back();
                volumeConfigSprites.pop_back();
                float torchX = 0;
                float torchY = 0;
                if (position <= 2){
                    torchX = configs[position+1].getPosition().x - 25.f;
                    torchY = configs[position+1].getPosition().y + 2.f;
                }
                else{
                    torchX = configs[position+1+col].getPosition().x - 25.f;
                    torchY = configs[position+1+col].getPosition().y + 2.f;
                }
                torch.setPosition(sf::Vector2f(torchX, torchY));

                volumeConfigSprites.push_back(torch);
            }
            std::cout<<"Position: "<< position << ", Col: " << col <<std::endl;
        }
    }
}

void VolumeConfGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    
}

void VolumeConfGS::drawVolumeBars(sf::RenderWindow& window, int actualVol, sf::Vector2f pos){
    const int MAX_VOLUME = 10;
    const int SEGMENT_WIDTH = 13;
    const int SEGMENT_HEIGHT = 15;
    const int SPACING = 2.5;
    const float BORDER_THICKNESS = 1.0f;

    // Player
    for (int i = 0; i < MAX_VOLUME; ++i)
    {
        bool isFull = (i < actualVol/10);

        float width = SEGMENT_WIDTH - (isFull ? 0 : BORDER_THICKNESS * 2);
        float height = SEGMENT_HEIGHT - (isFull ? 0 : BORDER_THICKNESS * 2);
        sf::RectangleShape segment(sf::Vector2f(width, height));

        // Adjust the position to center the empty segments within the border
        float xOffset = isFull ? 0 : BORDER_THICKNESS;
        float yOffset = isFull ? 0 : BORDER_THICKNESS;
        segment.setPosition(sf::Vector2f(pos.x + i * (SEGMENT_WIDTH + SPACING) + xOffset, pos.y + yOffset));

        segment.setOutlineThickness(BORDER_THICKNESS);
        segment.setOutlineColor(isFull ? sf::Color::Black : sf::Color::White);
        segment.setFillColor(isFull ? sf::Color::Red : sf::Color::Black);

        window.draw(segment);
    }
}

void VolumeConfGS::draw(sf::RenderWindow& window, Camera& camera){
    for (const auto& sprite : volumeConfigSprites) {
        window.draw(sprite);
    }

    for (const auto& text : configs) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    drawVolumeBars(window, masterVol, sf::Vector2f(125, 155));
    drawVolumeBars(window, musicVol, sf::Vector2f(125, 220));
    drawVolumeBars(window, soundVol, sf::Vector2f(125, 285));
}

void VolumeConfGS::pause(){
    if(debug) std::cout << "ESTADO: Volume Config PAUSADO" << std::endl;
}

void VolumeConfGS::resume(){
    if(debug) std::cout << "ESTADO: Volume Config REANUDADO" << std::endl;
}

void VolumeConfGS::close(){
    if(debug) std::cout << "ESTADO: Volume Config CERRADO" << std::endl;
    volumeConfigSprites.clear();
    volumeConfigTextures.clear();
    configs.clear();
}

VolumeConfGS::~VolumeConfGS() {}


// ======================================================
//                      GAMEPLAY CONFIG STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> gameplayConfigTextures;
std::vector<sf::Sprite> gameplayConfigSprites;

void GameplayConfGS::init(){
    if(debug) std::cout << "ESTADO: Gameplay Config" << std::endl;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    position = 0;
    col = 0;
    enterPressed = false;
    video = configManager.getVideo().window_mode;
    difficulty = configManager.getDifficulty().hard_mode;
    cheats = configManager.getCheats().enabled;
    skins = configManager.getSkins().activated;

    if (!gameplayConfigTextures["bg"].loadFromFile("./assets/sprites/menu/menu1.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(gameplayConfigTextures["bg"]);

    // Adjusts menu position
    sf::FloatRect spriteBounds = bg.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    bg.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    gameplayConfigSprites.push_back(bg);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    if (!fontinputs.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    fontinputs.setSmooth(false);

    sf::Text text(font, "GAMEPLAY", 35);
    text.setFillColor(sf::Color(122, 71, 22));
    text.setOutlineColor(sf::Color(255, 140, 0));
    text.setOutlineThickness(1.5); 
    sf::FloatRect textBounds = text.getLocalBounds();

    // Centers position
    float xPos = (gWindowWidth - textBounds.size.x) / 2;
    float yPos = 70.f;

    text.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text);

    // Defines menu options
    std::string textos[4] = {"SCREEN MODE", "ENHANCED AI", "CHEATS", "CUSTOM SKINS"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2 - 45.f;
        float yPos = 125.f + i * 50.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    // BACK
    sf::Text text2(font, "BACK", 30);
    text2.setFillColor(sf::Color::White);
    text2.setOutlineColor(sf::Color(128, 128, 128));
    text2.setOutlineThickness(1.5);
    textBounds = text2.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x - 35.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text2.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text2);

    // CONFIRM
    sf::Text text3(font, "CONFIRM", 30);
    text3.setFillColor(sf::Color(0, 190, 0));
    text3.setOutlineColor(sf::Color(0, 100, 0));
    text3.setOutlineThickness(1.5);
    textBounds = text3.getLocalBounds();

    xPos = (gWindowWidth / 2.f) - textBounds.size.x + 175.f;
    yPos = 330.f; // 120.f + 3 * 65.f;

    text3.setPosition(sf::Vector2f(xPos, yPos));
    configs.push_back(text3);

    if (!gameplayConfigTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(gameplayConfigTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = configs[1].getPosition().x - 25.f;
    float torchY = configs[1].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    gameplayConfigSprites.push_back(torch);

    configGameplayManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    auto audio = configManager.getAudio();

    configGameplayManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");
    configGameplayManager.playMusic("menuMusic", configGameplayManager.realVolume(audio.master_volume, audio.music_volume));
}

void GameplayConfGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
            if (keyPressed->scancode == controls.down && position < 4) {    
                position ++;
            }

            if (keyPressed->scancode == controls.up && position > 0) {    
                position --;
            }

            if (keyPressed->scancode == controls.right && position == 4 && col == 0) {    
                col = 1;
            }

            if (keyPressed->scancode == controls.left && position == 4 && col == 1) {    
                col = 0;
            }

            if (keyPressed->scancode == controls.enter) {
                auto audio = configManager.getAudio();
                configGameplayManager.playSound("menuEnter", configGameplayManager.realVolume(audio.master_volume, audio.sound_volume));
                if(position == 0){ // Window mode
                    video = !video;
                }

                if(position == 1){ // Diff
                    difficulty = !difficulty;
                }

                if(position == 2){ // Cheats
                    cheats = !cheats;
                }

                if(position == 3){ // Skins
                    skins = !skins;
                }

                if (position == 4 && col == 0){
                    enterPressed = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    std::cout << "Road back to config menu" << std::endl;
                    stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                }

                if (position == 4 && col == 1){ // Back to configs and save files
                    enterPressed = true;
                    configManager::Video newVideo;
                    newVideo.window_mode = video;

                    configManager::Difficulty newDiff;
                    newDiff.hard_mode = difficulty;

                    configManager::Cheats newCheats;
                    newCheats.enabled = cheats;

                    configManager::Skins newSkins;
                    newSkins.activated = skins;

                    configManager.setVideo(newVideo);
                    configManager.setDifficulty(newDiff);
                    configManager.setCheats(newCheats);
                    configManager.setSkins(newSkins);
                    configManager.saveConfiguration("config.json");

                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    std::cout << "Road back to config menu after saving" << std::endl;
                    stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                }
            }

            if (!gameplayConfigSprites.empty()) {
                sf::Sprite torch = gameplayConfigSprites.back();
                gameplayConfigSprites.pop_back();
                float torchX = 0;
                float torchY = 0;
                if (position <= 2){
                    torchX = configs[position+1].getPosition().x - 25.f;
                    torchY = configs[position+1].getPosition().y + 2.f;
                }
                else{
                    torchX = configs[position+1+col].getPosition().x - 25.f;
                    torchY = configs[position+1+col].getPosition().y + 2.f;
                }
                torch.setPosition(sf::Vector2f(torchX, torchY));

                gameplayConfigSprites.push_back(torch);
            }
            std::cout<<"Position: "<< position << ", Col: " << col <<std::endl;
        }
    }
}

void GameplayConfGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    
}

void GameplayConfGS::draw(sf::RenderWindow& window, Camera& camera){
    for (const auto& sprite : gameplayConfigSprites) {
        window.draw(sprite);
    }

    for (const auto& text : configs) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    std::string boolTexts[4] = {
        video ? "FSCRN" : "WINDOW",
        difficulty ? "ON" : "OFF",
        cheats ? "ON" : "OFF",
        skins ? "ON" : "OFF"
    };

    for (int i = 0; i < 4; i++) {
        sf::Text statusText(fontinputs, boolTexts[i], 15);
        statusText.setFillColor(sf::Color(255, 140, 0));

        // Posición a la derecha del texto base
        float x = configs[i + 1].getPosition().x + configs[i + 1].getLocalBounds().size.x + 20.f;
        float y = configs[i + 1].getPosition().y + 5.f;
        statusText.setPosition(sf::Vector2f(x, y));

        window.draw(statusText);
    }
}

void GameplayConfGS::pause(){
    if(debug) std::cout << "ESTADO: Gameplay Config PAUSADO" << std::endl;
}

void GameplayConfGS::resume(){
    if(debug) std::cout << "ESTADO: Gameplay Config REANUDADO" << std::endl;
}

void GameplayConfGS::close(){
    if(debug) std::cout << "ESTADO: Gameplay Config CERRADO" << std::endl;
    gameplayConfigSprites.clear();
    gameplayConfigTextures.clear();
    configs.clear();
}

GameplayConfGS::~GameplayConfGS() {}


// ======================================================
//                      INIT MENU STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> initMenuTextures;
std::vector<sf::Sprite> initMenuSprites;

void InitMenuGS::init(){
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    if(debug) std::cout << "ESTADO: Init Menu" << std::endl;
    
    if (!initMenuTextures["bg"].loadFromFile("./assets/sprites/intro_ending/titleScreen.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite bg(initMenuTextures["bg"]);

    bg.setTextureRect(sf::IntRect(sf::Vector2i(1, 1), sf::Vector2i(256, 240)));

    // Adjusts menu position
    sf::FloatRect spriteBounds = bg.getGlobalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactorWidth = gWindowWidth / spriteWidth;
    float scaleFactorHeight = gWindowHeight / spriteHeight;

    bg.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    float scaledWidth = spriteWidth * scaleFactorWidth;
    float scaledHeight = spriteHeight * scaleFactorHeight;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    bg.setPosition(sf::Vector2f(xPosition, yPosition));

    initMenuSprites.push_back(bg);

    auto castleSprite = std::make_shared<sf::Sprite>(initMenuTextures["bg"]);

    castleSprite->setTextureRect(sf::IntRect(sf::Vector2i(1, 256), sf::Vector2i(72, 56)));

    castleSprite->setScale(sf::Vector2f(scaleFactorWidth+0.1, scaleFactorHeight+0.1));

    float castleX = 287;
    float castleY = 173;

    castleSprite->setPosition(sf::Vector2f(castleX, castleY));

    castle = castleSprite;

    // Animation of the bat
    std::vector<AnimationManager::Frame> commingFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(74, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(220, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(293, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(366, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(439, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(512, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(585, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(658, 256), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(74, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(220, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(293, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(366, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(439, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(512, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(585, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(658, 313), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(74, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(220, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(293, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(366, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(439, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(512, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(585, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(658, 370), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(1, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(74, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(147, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(220, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(293, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(366, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(439, 427), sf::Vector2(72, 56)), 0.05f}
    };

    std::vector<AnimationManager::Frame> zoomedFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2(512, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(585, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(658, 427), sf::Vector2(72, 56)), 0.05f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2(585, 427), sf::Vector2(72, 56)), 0.05f}
    };
    
    animationManager = new AnimationManager(*castle);
    
    animationManager->addAnimation(commingBat, commingFrames, false);
    animationManager->addAnimation(zoomedBat, zoomedFrames);

    animationManager->playAnimation(commingBat);

    sf::Sprite start(initMenuTextures["bg"]);
    sf::Sprite black(initMenuTextures["bg"]);

    start.setTextureRect(sf::IntRect(sf::Vector2i(558, 1), sf::Vector2i(112, 8)));
    black.setTextureRect(sf::IntRect(sf::Vector2i(558, 10), sf::Vector2i(112, 8)));

    start.setScale(sf::Vector2f(scaleFactorWidth+0.1, scaleFactorHeight+0.1));
    black.setScale(sf::Vector2f(scaleFactorWidth+0.1, scaleFactorHeight+0.1));

    float startX = 112;
    float startY = 226;

    start.setPosition(sf::Vector2f(startX, startY));
    black.setPosition(sf::Vector2f(startX, startY));
    black.setColor(sf::Color::Transparent);

    initMenuSprites.push_back(start);
    initMenuSprites.push_back(black);

    blinking = false;
    blinks = 0;
    maxBlinks = 9;
}

void InitMenuGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.enter) {
            std::cout << "Let's blink" << std::endl;
            blinking = true;
        }
    }
}

void InitMenuGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    animationManager->update(deltaTime);
    if (animationManager->isAnimationFinished()){
        animationManager->playAnimation(zoomedBat);
    }

    if(blinking){
        sf::Sprite black = initMenuSprites.back();
        initMenuSprites.pop_back();

        blinkTimer += deltaTime;

        if (blinkTimer >= blinkInterval) {
            blinks ++;
            black.setColor(black.getColor() == sf::Color::Transparent ? sf::Color::White : sf::Color::Transparent);
            blinkTimer = 0.0f;
        }
        initMenuSprites.push_back(black);
    }

    if(blinks > maxBlinks){
        std::cout << "Going to Init animation" << std::endl;
        stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
    }
}

void InitMenuGS::draw(sf::RenderWindow& window, Camera& camera){
    // std::cout<<"Print"<<std::endl;
    // std::cout<<menuSprites.size()<<std::endl;
    // std::cout<<menuTextures.size()<<std::endl;
    // std::cout<<options.size()<<std::endl;
    window.draw(initMenuSprites[0]);
    window.draw(*castle);
    window.draw(initMenuSprites[1]);
    window.draw(initMenuSprites[2]);
}
 
void InitMenuGS::pause(){
    if(debug) std::cout << "ESTADO: Init Menu PAUSADO" << std::endl;
}

void InitMenuGS::resume(){
    if(debug) std::cout << "ESTADO: Init Menu REANUDADO" << std::endl;
}

void InitMenuGS::close(){
    if(debug) std::cout << "ESTADO: Init Menu CERRADO" << std::endl;
    initMenuSprites.clear();
    initMenuTextures.clear();
}


InitMenuGS::~InitMenuGS() {}


// ======================================================
//                      INITIAL ANIMATION STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> initAnimationTextures;
std::vector<sf::Sprite> initAnimationSprites;

void InitAnimationGS::init(){
    if(debug) std::cout << "ESTADO: Init animation" << std::endl;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    slide = 1;

    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);
    sf::Text text(font, "Press Enter to skip...", 9);
    text.setFillColor(sf::Color(255, 255, 255, 125));

    // Centers position
    float xPos = 250;
    float yPos = 385;

    text.setPosition(sf::Vector2f(xPos, yPos));
    warning.push_back(text);
    // ======================================================
    //                      BACKGROUNDS [0-3]
    // ======================================================

    // CASTLE 0
    if (!initAnimationTextures["castle"].loadFromFile("./assets/sprites/initAnimation/castle.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite castle(initAnimationTextures["castle"]);

    sf::FloatRect spriteBounds = castle.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    castle.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    castle.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(castle);

    // BACK 1
    if (!initAnimationTextures["back"].loadFromFile("./assets/sprites/initAnimation/back.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite back(initAnimationTextures["back"]);

    spriteBounds = back.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    float scaleFactorWidth = gWindowWidth / spriteWidth;
    float scaleFactorHeight = gWindowHeight / spriteHeight;

    back.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    scaledWidth = spriteWidth * scaleFactorWidth;
    scaledHeight = spriteHeight * scaleFactorHeight;

    xPosition = (gWindowWidth - scaledWidth) / 2;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    back.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(back);

    // BACKDRACULA 2
    if (!initAnimationTextures["backDracula"].loadFromFile("./assets/sprites/initAnimation/backDracula.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite backDracula(initAnimationTextures["backDracula"]);

    spriteBounds = backDracula.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    scaleFactorWidth = gWindowWidth / spriteWidth;
    scaleFactorHeight = gWindowHeight / spriteHeight;

    backDracula.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    scaledWidth = spriteWidth * scaleFactorWidth;
    scaledHeight = spriteHeight * scaleFactorHeight;

    xPosition = (gWindowWidth - scaledWidth) / 2;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    backDracula.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(backDracula);

    // NIGHT 3
    if (!initAnimationTextures["night"].loadFromFile("./assets/sprites/initAnimation/night.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite night(initAnimationTextures["night"]);

    spriteBounds = night.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    scaleFactor = gWindowHeight / spriteHeight;

    night.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    scaledWidth = spriteWidth * scaleFactor;
    scaledHeight = spriteHeight * scaleFactor;

    xPosition = (gWindowWidth - scaledWidth) / 2;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    night.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(night);

    // ======================================================
    //                      TEXTS [4-8] 
    // ======================================================

    // TEXT1 4
    if (!initAnimationTextures["text1"].loadFromFile("./assets/sprites/initAnimation/text1.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite text1(initAnimationTextures["text1"]);

    text1.setScale(sf::Vector2f(0.5, 0.5));

    text1.setPosition(sf::Vector2f(5, 25));

    initAnimationSprites.push_back(text1);

    // TEXT2 5
    if (!initAnimationTextures["text2"].loadFromFile("./assets/sprites/initAnimation/text2.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite text2(initAnimationTextures["text2"]);

    text2.setScale(sf::Vector2f(0.5, 0.5));

    spriteBounds = text2.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;

    xPosition = (gWindowWidth - spriteWidth) / 2;
    yPosition = 300;

    text2.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(text2);

    // TEXT3 6
    if (!initAnimationTextures["text3"].loadFromFile("./assets/sprites/initAnimation/text3.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite text3(initAnimationTextures["text3"]);

    text3.setScale(sf::Vector2f(0.5, 0.5));

    spriteBounds = text3.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = (gWindowWidth - spriteWidth) / 2;
    yPosition = ((gWindowHeight - spriteHeight) / 2) - 100;

    text3.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(text3);

    // TEXT4 7
    if (!initAnimationTextures["text4"].loadFromFile("./assets/sprites/initAnimation/text4.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite text4(initAnimationTextures["text4"]);

    text4.setScale(sf::Vector2f(0.5, 0.5));

    text4.setPosition(sf::Vector2f(5, 25));

    initAnimationSprites.push_back(text4);

    // TEXT5 8
    if (!initAnimationTextures["text5"].loadFromFile("./assets/sprites/initAnimation/text5.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite text5(initAnimationTextures["text5"]);

    text5.setScale(sf::Vector2f(0.5, 0.5));

    spriteBounds = text5.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = (gWindowWidth - spriteWidth) / 2;
    yPosition = ((gWindowHeight - spriteHeight) / 2) + 125;

    text5.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(text5);

    // ======================================================
    //                      WITH CHROMA [9-14] 
    // ======================================================

    // COFFIN 9
    sf::Image coffinImg;
    if (!coffinImg.loadFromFile("./assets/sprites/initAnimation/coffin.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    coffinImg.createMaskFromColor(sf::Color(0, 128, 0));
    initAnimationTextures["coffin"] = sf::Texture(coffinImg, false);
    sf::Sprite coffin(initAnimationTextures["coffin"]);

    coffin.setScale(sf::Vector2f(2, 2));

    spriteBounds = coffin.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = (gWindowWidth - spriteWidth) / 2;
    yPosition = (gWindowHeight - spriteHeight) / 2 + 105;

    coffin.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(coffin);

    // DRACULA 10
    sf::Image draculaImg;
    if (!draculaImg.loadFromFile("./assets/sprites/initAnimation/dracula.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    draculaImg.createMaskFromColor(sf::Color(0, 128, 0));
    initAnimationTextures["dracula"] = sf::Texture(draculaImg, false);
    sf::Sprite dracula(initAnimationTextures["dracula"]);

    dracula.setScale(sf::Vector2f(2, 2));

    spriteBounds = dracula.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = -100;
    yPosition = (gWindowHeight - spriteHeight) / 2;

    dracula.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(dracula);

    // MOON 11
    sf::Image moonImg;
    if (!moonImg.loadFromFile("./assets/sprites/initAnimation/moon.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    moonImg.createMaskFromColor(sf::Color(0, 128, 0));
    initAnimationTextures["moon"] = sf::Texture(moonImg, false);
    sf::Sprite moon(initAnimationTextures["moon"]);

    moon.setScale(sf::Vector2f(1.3, 1.3));

    spriteBounds = moon.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = 325;
    yPosition = 25;

    moon.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(moon);

    // FOREST 12
    sf::Image forestImg;
    if (!forestImg.loadFromFile("./assets/sprites/initAnimation/forest.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    forestImg.createMaskFromColor(sf::Color(24, 60, 92));
    initAnimationTextures["forest"] = sf::Texture(forestImg, false);
    sf::Sprite forest(initAnimationTextures["forest"]);

    spriteBounds = forest.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    scaleFactor = gWindowWidth / spriteWidth;

    forest.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    xPosition = 0.f;
    yPosition = gWindowHeight - (spriteHeight * scaleFactor);

    forest.setPosition(sf::Vector2f(xPosition, yPosition));


    initAnimationSprites.push_back(forest);

    // FUME 13
    sf::Image fumeImg;
    if (!fumeImg.loadFromFile("./assets/sprites/initAnimation/fume.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    fumeImg.createMaskFromColor(sf::Color(84, 109, 142));
    initAnimationTextures["fume"] = sf::Texture(fumeImg, false);
    sf::Sprite fume(initAnimationTextures["fume"]);

    fume.setScale(sf::Vector2f(2, 2));

    spriteBounds = fume.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = 0;
    yPosition = ((gWindowHeight - spriteHeight) / 2) + 70;

    fume.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(fume);

    // HAND 14
    sf::Image handImg;
    if (!handImg.loadFromFile("./assets/sprites/initAnimation/hand.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    handImg.createMaskFromColor(sf::Color(116, 116, 116));
    initAnimationTextures["hand"] = sf::Texture(handImg, false);
    sf::Sprite hand(initAnimationTextures["hand"]);

    hand.setScale(sf::Vector2f(2.25, 2.25));

    spriteBounds = hand.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = (gWindowWidth - spriteWidth) / 2;
    yPosition = ((gWindowHeight - spriteHeight) / 2) + 150;

    hand.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(hand);

    // BELMONT 15
    sf::Image belmontImg;
    if (!belmontImg.loadFromFile("./assets/sprites/initAnimation/belmont.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    belmontImg.createMaskFromColor(sf::Color(84,109,142));
    initAnimationTextures["belmont"] = sf::Texture(belmontImg, false);
    sf::Sprite belmont(initAnimationTextures["belmont"]);

    spriteBounds = belmont.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    scaleFactorWidth = gWindowWidth / spriteWidth;
    scaleFactorHeight = gWindowHeight / spriteHeight;

    belmont.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    scaledWidth = spriteWidth * scaleFactorWidth;
    scaledHeight = spriteHeight * scaleFactorHeight;

    xPosition = (gWindowWidth - scaledWidth) / 2 + 250;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    belmont.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(belmont);

    // FIRE 16
    sf::Image fireImg;
    if (!fireImg.loadFromFile("./assets/sprites/initAnimation/fire.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    fireImg.createMaskFromColor(sf::Color(84,109,142));
    initAnimationTextures["fire"] = sf::Texture(fireImg, false);
    sf::Sprite fire(initAnimationTextures["fire"]);

    spriteBounds = fire.getLocalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    scaleFactorWidth = gWindowWidth / spriteWidth;
    scaleFactorHeight = gWindowHeight / spriteHeight;

    fire.setScale(sf::Vector2f(scaleFactorWidth, scaleFactorHeight));

    scaledWidth = spriteWidth * scaleFactorWidth;
    scaledHeight = spriteHeight * scaleFactorHeight;

    xPosition = (gWindowWidth - scaledWidth) / 2;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    fire.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(fire);

    // WHIP 17
    sf::Image whipImg;
    if (!whipImg.loadFromFile("./assets/sprites/initAnimation/whip.png"))
    {
        std::cerr << "Error cargando la imagen de Simon" << std::endl;
    }
    whipImg.createMaskFromColor(sf::Color(84,109,142));
    initAnimationTextures["whip"] = sf::Texture(whipImg, false);
    sf::Sprite whip(initAnimationTextures["whip"]);

    whip.setScale(sf::Vector2f(2.3, 2.3));

    spriteBounds = whip.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    xPosition = ((gWindowWidth - spriteWidth) / 2) - 150;
    yPosition = ((gWindowHeight - spriteHeight) / 2) + 15;

    whip.setPosition(sf::Vector2f(xPosition, yPosition));

    initAnimationSprites.push_back(whip);

    // ======================================================
    //                      SOUND AND MUSIC 
    // ======================================================
    
    auto audio = configManager.getAudio();

    initAnimSounds.loadMusic("initAnimMusic", "./assets/music/03AVisionofDarkSecrets.mp3");
    initAnimSounds.playMusic("initAnimMusic", initAnimSounds.realVolume(audio.master_volume, audio.music_volume));

    fadingOutMusic = false;
    currentMusicVolume = initAnimSounds.realVolume(audio.master_volume, audio.music_volume);
    fadeOutSpeed = 20.f;
}

void InitAnimationGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if(keyPressed->scancode == controls.enter){
            stateMachine->replaceState(std::make_unique<walkingAnimGS>(stateMachine));
            std::cout << "Skipping animation, going to game first anim" << std::endl;
        }
    }
}

void InitAnimationGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    timer += deltaTime;

    // Warning blink
    float alphaMin = 25.f;
    float alphaMax = 125.f;
    float amplitude = (alphaMax - alphaMin) / 2.f;
    float midpoint = (alphaMax + alphaMin) / 2.f;
    float speed = 2.f; // blink speed

    float oscillation = std::sin(timer * speed); // between -1 and 1
    float alpha = midpoint + amplitude * oscillation; // between 25 and 125

    alpha = std::clamp(alpha, 25.f, 125.f);
    warning[0].setFillColor(sf::Color(255, 255, 255, alpha));

    if(timer >= timerInterval){
        slide ++;
        timer = 0.0f;
        resetMovingSprites();
    }

    const float moveSpeed = 60.f;        // 1px per frame 60fps -> 60px per second
    const float moveHalfSpeed = 30.f;     // 0.5px per frame 60fps -> 30px per second
    const float fumeSpeed = 60.f;
    const float whipSpeed = 60.f;
    const float belmontReturnSpeed = 60.f;
    const float draculaRetreatSpeed = 60.f;

    switch (slide){
        case 1:{
            // Move Dracula
            sf::Sprite& dracula = initAnimationSprites[10];
            sf::Vector2f pos = dracula.getPosition();

            if (pos.x < 0.f) {
                pos.x += moveSpeed * deltaTime;
                if (pos.x > 0.f) pos.x = 0.f;
                dracula.setPosition(pos);
            }

            // Move Fume
            sf::Sprite& fume = initAnimationSprites[13];
            pos = fume.getPosition();
            pos.x -= fumeSpeed * deltaTime;
            fume.setPosition(pos);
            break;
        }

        case 2:{
            // Move Belmont
            sf::Sprite& belmont = initAnimationSprites[15];
            sf::Vector2f pos = belmont.getPosition();

            pos.x -= moveSpeed * deltaTime;
            belmont.setPosition(pos);
            break;
        }

        case 3:{
            // Move hand
            sf::Sprite& hand = initAnimationSprites[14];
            sf::Vector2f pos = hand.getPosition();

            if (pos.y > 242.f) {
                pos.y -= moveHalfSpeed * deltaTime;
                if (pos.y < 242.f) pos.y = 242.f;
                hand.setPosition(pos);
            }
            break;
        }

        case 4:{
            // Move Fume
            sf::Sprite& fume = initAnimationSprites[13];
            sf::Vector2f pos = fume.getPosition();
            pos.x -= fumeSpeed * deltaTime;
            fume.setPosition(pos);

            // Move Whip
            sf::Sprite& whip = initAnimationSprites[17];
            pos = whip.getPosition();
            float max = (gWindowWidth - pos.x) / 2.0f;

            if (pos.x < max) {
                pos.x += whipSpeed * deltaTime;
                if (pos.x > max) pos.x = max;
                whip.setPosition(pos);
            }
            break;
        }
        
        case 5:{
            // Move Fume
            sf::Sprite& fume = initAnimationSprites[13];
            sf::Vector2f pos = fume.getPosition();
            pos.x -= fumeSpeed * deltaTime;
            fume.setPosition(pos);

            // Move Belmont
            sf::Sprite& belmont = initAnimationSprites[15];
            pos = belmont.getPosition();
            if (pos.x < 165.f) {
                pos.x += belmontReturnSpeed * deltaTime;
                if (pos.x > 165.f) pos.x = 165.f;
                belmont.setPosition(pos);
            }

            // Move Dracula
            sf::Sprite& dracula = initAnimationSprites[10];
            pos = dracula.getPosition();
            if (pos.x > 400.f) {
                pos.x -= draculaRetreatSpeed * deltaTime;
                if (pos.x < 400.f) pos.x = 400.f;
                dracula.setPosition(pos);
            }

            // Fading out music
            if (fadingOutMusic && currentMusicVolume > 0.f) {
                currentMusicVolume -= fadeOutSpeed * deltaTime;
                if (currentMusicVolume < 0.f) currentMusicVolume = 0.f;
                initAnimSounds.adjustAllMusicVolumes(currentMusicVolume);
            }
            break;
        }

        case 6:
            stateMachine->replaceState(std::make_unique<walkingAnimGS>(stateMachine));
            std::cout << "Animation finished, going to game first anim" << std::endl;
            break;
    }
}

void InitAnimationGS::draw(sf::RenderWindow& window, Camera& camera){
    drawSlide(window);
    window.draw(warning[0]);
}

void InitAnimationGS::drawSlide(sf::RenderWindow& window){
    std::vector<int> spriteIndices;

    switch(slide){
        // Castle, Dracula, Fume, Forest, Moon, Text1
        case 1: spriteIndices = {0, 10, 13, 12, 11, 4}; break;

        // Back, Belmont, Fire, Text2
        case 2: spriteIndices = {1, 15, 16, 5}; break;

        // BackDracula, Hand, Coffin, Text3
        case 3: spriteIndices = {2, 14, 9, 6}; break;

        // Castle, Whip, Fume, Forest, Moon, Text4
        case 4: spriteIndices = {0, 17, 13, 12, 11, 7}; break;

        // Night, Moon, Dracula, Belmont, Fume, Text5
        case 5: spriteIndices = {3, 11, 10, 15, 13, 8}; break;
        
        default: return;
    }

    float fadeDuration = 1.5f;
    float alpha = 255.0f;

    if (timer < fadeDuration) {
        alpha = (timer / fadeDuration) * 255.0f; // Fade in
    } else if (timer > (timerInterval - fadeDuration)) {
        if (slide == 5) fadingOutMusic = true;
        alpha = ((timerInterval - timer) / fadeDuration) * 255.0f; // Fade out
    }

    alpha = std::clamp(alpha, 0.0f, 255.0f); // asegurar rango válido

    for (int idx : spriteIndices) {
        if (idx == 14){
            sf::Vector2f pos = initAnimationSprites[14].getPosition();
            if (pos.y < 275){
                sf::Sprite sprite = initAnimationSprites[14];
                sf::Color color = sprite.getColor();
                color.a = static_cast<uint8_t>(alpha);
                sprite.setColor(color);
                window.draw(sprite);
            }
            continue;
        }
        sf::Sprite sprite = initAnimationSprites[idx];
        sf::Color color = sprite.getColor();
        color.a = static_cast<uint8_t>(alpha);
        sprite.setColor(color);
        window.draw(sprite);
    }
}

void InitAnimationGS::resetMovingSprites(){
    sf::Sprite& fume = initAnimationSprites[13];

    float spriteHeight = fume.getGlobalBounds().size.y;

    float yPosition = ((gWindowHeight - spriteHeight) / 2) + 70;

    fume.setPosition(sf::Vector2f(0, yPosition));

    if(slide == 5){
        fume.setPosition(sf::Vector2f(0, yPosition - 25));

        sf::Sprite& dracula = initAnimationSprites[10];
        sf::Vector2f scale = dracula.getScale();
        dracula.setScale(sf::Vector2f(-scale.x, scale.y));
        float spriteHeight = dracula.getGlobalBounds().size.y - 55;
        dracula.setPosition(sf::Vector2f(gWindowHeight + 135, spriteHeight));
        
        sf::Sprite& belmont = initAnimationSprites[15];
        belmont.setScale(sf::Vector2f(-1.4, 1.4));
        spriteHeight = dracula.getGlobalBounds().size.y - 60;
        belmont.setPosition(sf::Vector2f(30, spriteHeight));
    }

}

void InitAnimationGS::pause(){
    if(debug) std::cout << "ESTADO: Init animation PAUSADO" << std::endl;
}

void InitAnimationGS::resume(){
    if(debug) std::cout << "ESTADO: Init animation REANUDADO" << std::endl;
}

void InitAnimationGS::close(){
    if(debug) std::cout << "ESTADO: Init animation CERRADO" << std::endl;
    initAnimationSprites.clear();
    initAnimationTextures.clear();
}

InitAnimationGS::~InitAnimationGS() {}


// ======================================================
//                      MENU STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> levelSelectorTextures;
std::vector<sf::Sprite> levelSelectorSprites;

void levelSelectorGS::init(){
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    // Loads menu texture
    if(debug) std::cout << "ESTADO: Menu" << std::endl;
    position = 0;
    enterPressed = false;
    pos = {
        sf::Vector2f(68.f, 272.f),
        // sf::Vector2f(154.f, 265.f),
        sf::Vector2f(78.f, 222.f),
        // sf::Vector2f(259.f, 247.f),     // 176, 224
        sf::Vector2f(292.f, 228.f),
        // sf::Vector2f(262.f, 160.f),
        sf::Vector2f(171.f, 124.f)
    };    

    sf::Image bgImg;
    if (!bgImg.loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    bgImg.createMaskFromColor(gColorKeyGrey);
    levelSelectorTextures["selector"] = sf::Texture(bgImg, false);

    sf::Sprite selector(levelSelectorTextures["selector"]);
    selector.setTextureRect(sf::IntRect(sf::Vector2i(262, 808), sf::Vector2i(384, 144)));

    // Adjusts selector position
    sf::FloatRect spriteBounds = selector.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowWidth / spriteWidth;

    selector.setScale(sf::Vector2f(scaleFactor, scaleFactor + 0.5));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = ((gWindowHeight - scaledHeight) / 2) - 15;

    selector.setPosition(sf::Vector2f(xPosition, yPosition));

    levelSelectorSprites.push_back(selector);

    levelSelectorTextures["obj"] = sf::Texture(bgImg, false);

    sf::Sprite obj(levelSelectorTextures["obj"]);
    obj.setTextureRect(sf::IntRect(sf::Vector2i(263, 962), sf::Vector2i(6, 7)));
    obj.setScale(sf::Vector2f(1.5, 1.5));
    obj.setPosition(pos[0]);

    levelSelectorSprites.push_back(obj);

    walkingAnimTextures["bat"] = sf::Texture(bgImg, false);

    auto batSprite = std::make_shared<sf::Sprite>(walkingAnimTextures["bat"]);

    batSprite->setTextureRect(sf::IntRect(sf::Vector2i(305, 953), sf::Vector2i(16, 16)));
    batSprite->setScale(sf::Vector2f(1.65, 1.65));
    batSprite->setPosition(pos[3]);
    batSprite->setColor(sf::Color::Transparent);

    bat = batSprite;

    // Animation of the bat
    std::vector<AnimationManager::Frame> idleFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(305, 953), sf::Vector2i(16, 16)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(322, 953), sf::Vector2i(16, 16)), 0.1f}
    };
    
    batManager = new AnimationManager(*bat);
    
    batManager->addAnimation(commingBat, idleFrames, true);

    batManager->playAnimation(commingBat);

    // Loads text font
    if (!font.openFromFile("./assets/fonts/NESfonts/nintendo-nes-font.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    font.setSmooth(false);

    // Defines menu options
    // std::string textos[7] = {"LVL 1 - ENTRANCE", "LVL 2 - MAIN HALL", "LVL 3 - OUTER TOWER",
    //                          "LVL 4 - CAVES", "LVL 5 - PRISON", "LVL 6 - CLOCK TOWER",
    //                          "LVL 7 - DRACULA'S CHAMBERS"};
    std::string textos[4] = {"LVL 1 - ENTRANCE", "LVL 2 - OUTER TOWER",
                                "LVL 3 - PRISON", "LVL 4 - DRACULA'S CHAMBERS"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 15);
        text.setFillColor(sf::Color::Transparent);
        // text.setPosition(sf::Vector2f(330.f, 80.f + i * 40.f));
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 50.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        levels.push_back(text);
    }

    std::string texts[4] = {"CONTROLS:", "ESCAPE", "<- / ->", "ENTER"};
    std::vector<float> posX = {15, 45, 180, 325};
    std::vector<float> posY = {340, 365, 365, 365};
    for (int i = 0; i < 4; i++) {
    sf::Text text(font, texts[i], 10);
    text.setFillColor(sf::Color::White);

    text.setPosition(sf::Vector2f(posX[i], posY[i]));
    infos.push_back(text);
    }
    
    showPopUp = false;

    if (!levelSelectorTextures["popUp"].loadFromFile("./assets/sprites/menu/warningLevel.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite popUp(levelSelectorTextures["popUp"]);
    popUp.setScale(sf::Vector2f(0.6, 0.6));

    spriteBounds = popUp.getGlobalBounds();
    spriteWidth = spriteBounds.size.x;
    spriteHeight = spriteBounds.size.y;

    popUp.setPosition(sf::Vector2f((gWindowWidth - spriteWidth) / 2, (gWindowHeight - spriteHeight) / 2));

    levelSelectorSprites.push_back(popUp);

    // menuSoundManager.loadSound("menuEnter", "./assets/sounds/menuEnter.mp3");

    // menuSoundManager.loadMusic("menuMusic", "./assets/music/menuMusic.mp3");
    levelSelectorSounds.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    auto audio = configManager.getAudio();

    levelSelectorSounds.loadMusic("menuMusic1", "./assets/music/05Wicked_Child(1).mp3");
    levelSelectorSounds.loadMusic("menuMusic2", "./assets/music/05Wicked_Child(2).mp3");
    levelSelectorSounds.playMusicSequence("menuMusic1", "menuMusic2", true, levelSelectorSounds.realVolume(audio.master_volume, audio.music_volume));
}

void levelSelectorGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(!enterPressed){
        if(!showPopUp){
            if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == controls.right && position < 3){
                    position ++;
                    std::cout << position << std::endl;
                }
                
                if (keyPressed->scancode == controls.left && position > 0){
                    position --;
                    std::cout << position << std::endl;
                }
    
                if (keyPressed->scancode == controls.escape){
                    stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
                }
    
                if (keyPressed->scancode == controls.enter) {
                    enterPressed = true;
                    auto audio = configManager.getAudio();
                    levelSelectorSounds.playSound("menuEnter", levelSelectorSounds.realVolume(audio.master_volume, audio.sound_volume));
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                    switch (position) {
                        case 0: // Lvl 1
                            gStartingLevel = 1;
                            gStartingStage = 1;
                            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
                            break;
                        case 1: // Lvl 3
                            gStartingLevel = 3;
                            gStartingStage = 1;
                            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
                            break;
                        case 2: // Lvl 5
                            // showPopUp = true;
                            gStartingLevel = 5;
                            gStartingStage = 1;
                            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
                            break;
                        case 3: // Lvl 7
                            gStartingLevel = 7;
                            gStartingStage = 1; 
                            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
                            break;
                    }
                }
            }
        }
        else{

        }
        
    }
}

void levelSelectorGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    batManager->update(deltaTime);

    int size = levels.size();
    for(int i = 0; i < size; i++){
        if(i == position){
            levels[i].setFillColor(sf::Color::White);
        }
        else{
            levels[i].setFillColor(sf::Color::Transparent);
        }
    }

    if(position < 3){
        levelSelectorSprites[1].setPosition(pos[position]);

        blinkTimer += deltaTime;

        if (blinkTimer >= blinkInterval) {
            levelSelectorSprites[1].setColor(levelSelectorSprites[1].getColor() == sf::Color::Transparent ? sf::Color::White : sf::Color::Transparent);
            blinkTimer = 0.0f;
        }

        bat->setColor(sf::Color::Transparent);
    }
    else{
        levelSelectorSprites[1].setColor(sf::Color::Transparent);
        bat->setColor(sf::Color::White);
    }
    

    sf::Sprite popUp = levelSelectorSprites.back();
    levelSelectorSprites.pop_back();
    if (showPopUp){
        popUp.setColor(sf::Color::White);
        popUpTimer += deltaTime;

        if(popUpTimer >= popUpInterval){
            popUp.setColor(sf::Color::Transparent);
            popUpTimer = 0.0f;
            showPopUp = false;
            enterPressed = false;
        }
    }
    else{
        popUp.setColor(sf::Color::Transparent);
    }
    levelSelectorSprites.push_back(popUp);
}

void levelSelectorGS::draw(sf::RenderWindow& window, Camera& camera){
    // std::cout<<"Print"<<std::endl;
    // std::cout<<menuSprites.size()<<std::endl;
    // std::cout<<menuTextures.size()<<std::endl;
    // std::cout<<options.size()<<std::endl;
    for (const auto& sprite : levelSelectorSprites) {
        window.draw(sprite);
    }
    for (const auto& text : levels) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    for (const auto& text : infos) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
    }

    window.draw(*bat);
}
 
void levelSelectorGS::pause(){
    if(debug) std::cout << "ESTADO: Menu PAUSADO" << std::endl;
}

void levelSelectorGS::resume(){
    if(debug) std::cout << "ESTADO: Menu REANUDADO" << std::endl;
}

void levelSelectorGS::close(){
    if(debug) std::cout << "ESTADO: Menu CERRADO" << std::endl;
    levelSelectorSprites.clear();
    levelSelectorTextures.clear();
    levels.clear();
    infos.clear();
}

levelSelectorGS::~levelSelectorGS() {}

// ======================================================
//                      CREDITS ANIMATION STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> creditsAnimationTextures;
std::vector<sf::Sprite> creditsAnimationSprites;

void CreditsAnimationGS::init(){
    if(debug) std::cout << "ESTADO: Init animation" << std::endl;
    this->m_viewSize.x = gMenuGS_size_x;
    this->m_viewSize.y = gMenuGS_size_y;

    castleElapsedTime = 0.f;
    startCastleMovement = false;
    startCredits = false;
    castleSoundTimer = 0.f;
    musicStarted = false;
    waitElapsedTime = 0.f;

    // CASTLE
    sf::Image base;
    if (!base.loadFromFile("./assets/sprites/intro_ending/cutscenesCredits.png"))
    {
        std::cerr << "Error loading heart image" << std::endl;
    }
    base.createMaskFromColor(gColorKeyGrey);
    base.createMaskFromColor(gColorKeyGreen);

    creditsAnimationTextures["base"] = sf::Texture(base, false);

    sf::Sprite back(creditsAnimationTextures["base"]);
    back.setTextureRect(sf::IntRect(sf::Vector2i(262, 242), sf::Vector2i(256, 240)));

    sf::FloatRect spriteBounds = back.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    float scaleFactor = gWindowHeight / spriteHeight;

    back.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    back.setPosition(sf::Vector2f(xPosition, yPosition));

    creditsAnimationSprites.push_back(back);

    sf::Sprite front(creditsAnimationTextures["base"]);
    front.setTextureRect(sf::IntRect(sf::Vector2i(262, 483), sf::Vector2i(256, 240)));
    
    front.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    xPosition = (gWindowWidth - scaledWidth) / 2;
    yPosition = (gWindowHeight - scaledHeight) / 2;

    front.setPosition(sf::Vector2f(xPosition, yPosition));

    creditsAnimationSprites.push_back(front);
    
    sf::Sprite castle(creditsAnimationTextures["base"]);
    castle.setTextureRect(sf::IntRect(sf::Vector2i(262, 728), sf::Vector2i(56, 64)));

    castle.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    castle.setPosition(sf::Vector2f(261, 82));

    creditsAnimationSprites.push_back(castle);

    castleStartPosition = castle.getPosition();
    castleEndPosition = castleStartPosition + sf::Vector2f(0.f, 100.f);
    
    auto cred = std::make_shared<sf::Sprite>(creditsAnimationTextures["base"]);

    cred->setTextureRect(sf::IntRect(sf::Vector2i(523, 731), sf::Vector2i(128, 72)));

    cred->setScale(sf::Vector2f(scaleFactor+0.1, scaleFactor));

    cred->setPosition(sf::Vector2f(13, 80));

    credits = cred;

    std::vector<AnimationManager::Frame> creditsFrames{
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 1), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 1), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 1), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 1), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 74), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 74), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 74), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 74), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 147), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 147), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 147), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 147), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 220), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 220), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 220), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 220), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 293), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 293), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 293), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 293), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 366), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 366), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 366), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 366), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 439), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 439), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 439), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 439), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 512), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 512), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 512), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 512), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 585), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 585), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 585), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 585), sf::Vector2i(128, 72)), 4.9f},

        AnimationManager::Frame{sf::IntRect(sf::Vector2i(523, 658), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(652, 658), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(781, 658), sf::Vector2i(128, 72)), 0.1f},
        AnimationManager::Frame{sf::IntRect(sf::Vector2i(910, 658), sf::Vector2i(128, 72)), 4.9f}
    };

    creditsManager = new AnimationManager(*cred);
    
    creditsManager->addAnimation(creditsMovement, creditsFrames, false);

    // ======================================================
    //                      SOUND AND MUSIC 
    // ======================================================

    creditsAnimSounds.loadMusic("creditsAnimMusic", "./assets/music/12Voyager.mp3");
    creditsAnimSounds.loadSound("explosion1", "./assets/sounds/31.wav");
    creditsAnimSounds.loadSound("explosion2", "./assets/sounds/32.wav");
}

void CreditsAnimationGS::handleInput(sf::Event event){
    // auto controls = configManager.getControls();
    // if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
    //     if(keyPressed->scancode == controls.enter){
    //         stateMachine->replaceState(std::make_unique<walkingAnimGS>(stateMachine));
    //         std::cout << "Skipping animation, going to game first anim" << std::endl;
    //     }
    // }
}

void CreditsAnimationGS::update(float deltaTime, const sf::Vector2f& viewPosition, bool windowHasFocus){
    creditsManager->update(deltaTime);

    castleElapsedTime += deltaTime;

    auto audio = configManager.getAudio();
    if (castleElapsedTime >= 2.f && !startCastleMovement) {
        startCastleMovement = true;
        creditsAnimSounds.playSound("explosion2", creditsAnimSounds.realVolume(audio.master_volume, audio.sound_volume));
        castleSoundTimer = 0.f;
    }

    if (startCastleMovement) {
        float moveTime = castleElapsedTime - 2.f;

        // Movimiento vertical
        float progress = std::min(moveTime / castleMoveDuration, 1.f);
        sf::Vector2f newPos = castleStartPosition + (castleEndPosition - castleStartPosition) * progress;

        // Temblor horizontal
        float shakeAmplitude = 1.5f; // Pixel range for shaking
        float shake = std::sin(castleElapsedTime * 20.f) * shakeAmplitude;

        newPos.x += shake;

        creditsAnimationSprites[2].setPosition(newPos); // Index 2 = castle sprite
    }

    castleSoundTimer += deltaTime;

    if (castleSoundTimer >= 1.f && startCastleMovement && !startCredits) {
        creditsAnimSounds.playSound("explosion2", creditsAnimSounds.realVolume(audio.master_volume, audio.sound_volume));
        castleSoundTimer = 0.f;
    }

    if(castleElapsedTime >= castleMoveDuration + 2){
        if (!startCredits) startCredits = true;
        if(castleElapsedTime >= castleMoveDuration + 4 && !musicStarted){
            creditsManager->playAnimation(creditsMovement);
            creditsAnimSounds.playMusic("creditsAnimMusic", creditsAnimSounds.realVolume(audio.master_volume, audio.sound_volume), false);
            musicStarted = true;
        }
    }

    if(musicStarted && creditsAnimSounds.musicHasFinished("creditsAnimMusic")){
        waitElapsedTime += deltaTime;
        if(waitDuration >= waitDuration){
            stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
        }
    }
}

void CreditsAnimationGS::draw(sf::RenderWindow& window, Camera& camera){
    // Back
    window.draw(creditsAnimationSprites[0]);

    // Castle
    window.draw(creditsAnimationSprites[2]);

    // Black rectangle to hide castle
    // sf::RectangleShape black(sf::Vector2f(74, 46));
    // black.setFillColor(sf::Color::Black);
    // black.setPosition(sf::Vector2f(261, 163));
    // window.draw(black);

    // Front
    window.draw(creditsAnimationSprites[1]);

    // Credits
    window.draw(*credits);
}

void CreditsAnimationGS::pause(){
    if(debug) std::cout << "ESTADO: Init animation PAUSADO" << std::endl;
}

void CreditsAnimationGS::resume(){
    if(debug) std::cout << "ESTADO: Init animation REANUDADO" << std::endl;
}

void CreditsAnimationGS::close(){
    if(debug) std::cout << "ESTADO: Init animation CERRADO" << std::endl;
    creditsAnimationSprites.clear();
    creditsAnimationTextures.clear();
}

CreditsAnimationGS::~CreditsAnimationGS() {}