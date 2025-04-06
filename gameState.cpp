#include "gameState.h"
#include <iostream>
#include "gameStateMachine.h"
#include "globals.h"

#include <thread> // Necessary for using sleep_for
#include <chrono>
#include "configManager.h"

// constexpr sf::Keyboard::Scancode KEY_RIGHT = sf::Keyboard::Scancode::Right;
// constexpr sf::Keyboard::Scancode KEY_LEFT = sf::Keyboard::Scancode::Left;
// constexpr sf::Keyboard::Scancode KEY_DOWN = sf::Keyboard::Scancode::Down;
// constexpr sf::Keyboard::Scancode KEY_UP = sf::Keyboard::Scancode::Up;
// constexpr sf::Keyboard::Scancode KEY_ENTER = sf::Keyboard::Scancode::Enter;
constexpr sf::Keyboard::Scancode KEY_ESC = sf::Keyboard::Scancode::Escape;
// constexpr sf::Keyboard::Scancode KEY_JUMP = sf::Keyboard::Scancode::X;
// constexpr sf::Keyboard::Scancode KEY_ATTACK = sf::Keyboard::Scancode::Z;


const bool debug = true;



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
    view.setCenter({std::round(centerX), std::round(centerY)});


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
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_ESC)
        {    
            stateMachine->addState(std::make_unique<PauseGS>(stateMachine));
            return;
        }
    }

    game.handleInput(event);
}

void GameGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    game.update(deltaTime, viewPosition);
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

    // Loads text font
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

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
    float musicVol = (audio.master_volume * audio.music_volume)/100;

    menuSoundManager.loadMusic("menuMusic", "./assets/music/08Out_of_Time.mp3");
    menuSoundManager.playMusic("menuMusic", musicVol);
}

void MenuGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.down && position < 3) {    
            if (!menuSprites.empty()) {
                sf::Sprite torch = menuSprites.back();
                menuSprites.pop_back();

                position += 1;
                std::cout<<position<<std::endl;

                float torchX = options[position].getPosition().x - 25.f;
                float torchY = options[position].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                menuSprites.push_back(torch);
            }
        }

        if (keyPressed->scancode == controls.up && position > 0) {    
            if (!menuSprites.empty()) {
                sf::Sprite torch = menuSprites.back();
                menuSprites.pop_back();

                position -= 1;
                std::cout<<position<<std::endl;

                float torchX = options[position].getPosition().x - 25.f;
                float torchY = options[position].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                menuSprites.push_back(torch);
            }
        }

        if (keyPressed->scancode == controls.enter) {
            auto audio = configManager.getAudio();
            float soundVol = (audio.master_volume * audio.sound_volume)/100;
            menuSoundManager.playSound("menuEnter", soundVol);
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
            switch (position) {
                case 0:
                    stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
                    break;
                case 1:
                    std::cout<<"Not implemented yet :P"<<std::endl;
                    break;
                case 2:
                    stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
                    break;
                case 3:
                    std::cout<<"Bye bye :)"<<std::endl;
                    //window.close();
                    break;
            }
            //stateMachine->replaceState(std::make_unique<PauseGS>(stateMachine));
        }
    }
}

void MenuGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
}

void MenuGS::draw(sf::RenderWindow& window, Camera& camera){
    // std::cout<<"Print"<<std::endl;
    // std::cout<<menuSprites.size()<<std::endl;
    // std::cout<<menuTextures.size()<<std::endl;
    // std::cout<<options.size()<<std::endl;
    for (const auto& sprite : menuSprites) {
        window.draw(sprite);
    }
    for (const auto& text : options) {
        // std::cout << "Dibujando texto: " << text.getString().toAnsiString() << std::endl;
        window.draw(text);
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
}


MenuGS::~MenuGS() {}


// ======================================================
//                      PAUSE STATE 
// ======================================================

void PauseGS::init(){
    if(debug) std::cout << "ESTADO: Pause" << std::endl;
}

void PauseGS::handleInput(sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_ESC)
        {    
            stateMachine->removeState(); 
        }
    }
}

void PauseGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
}

void PauseGS::draw(sf::RenderWindow& window, Camera& camera){
    
}

void PauseGS::pause(){
    if(debug) std::cout << "ESTADO: Pause PAUSADO" << std::endl;
}

void PauseGS::resume(){
    if(debug) std::cout << "ESTADO: Pause REANUDADO" << std::endl;
}

void PauseGS::close(){
    if(debug) std::cout << "ESTADO: Pause CERRADO" << std::endl;
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

    sf::Text text(font, "Configuration", 35);
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
    float musicVol = (audio.master_volume * audio.music_volume)/100;

    configSoundManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");
    configSoundManager.playMusic("menuMusic", musicVol);
}

void ConfigGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.down && position < 3) {    
            if (!configSprites.empty()) {
                sf::Sprite torch = configSprites.back();
                configSprites.pop_back();

                position += 1;
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

                position -= 1;
                std::cout<<position<<std::endl;

                float torchX = configs[position+1].getPosition().x - 25.f;
                float torchY = configs[position+1].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                configSprites.push_back(torch);
            }
        }

        if (keyPressed->scancode == controls.enter) {
            auto audio = configManager.getAudio();
            float soundVol = (audio.master_volume * audio.sound_volume)/100;
            configSoundManager.playSound("menuEnter", soundVol);
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

void ConfigGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
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

void ControlsConfGS::init(){
    if(debug) std::cout << "ESTADO: Controls Config" << std::endl;
}

void ControlsConfGS::handleInput(sf::Event event){
    
}

void ControlsConfGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
}

void ControlsConfGS::draw(sf::RenderWindow& window, Camera& camera){
    
}

void ControlsConfGS::pause(){
    if(debug) std::cout << "ESTADO: Controls Config PAUSADO" << std::endl;
}

void ControlsConfGS::resume(){
    if(debug) std::cout << "ESTADO: Controls Config REANUDADO" << std::endl;
}

void ControlsConfGS::close(){
    if(debug) std::cout << "ESTADO: Controls Config CERRADO" << std::endl;
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
    std::string textos[4] = {"MASTER AUDIO", "MUSIC", "SOUNDS", "CONFIRM"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();

        // Centers position
        float xPos = (gWindowWidth - textBounds.size.x) / 2;
        float yPos = 115.f + i * 65.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        configs.push_back(text);
    }

    if (!volumeConfigTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(volumeConfigTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.5f, torch.getScale().y * 1.5f));

    float torchX = configs[1].getPosition().x - 25.f;
    float torchY = configs[1].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    volumeConfigSprites.push_back(torch);

    configSoundManager.loadSound("menuEnter", "./assets/sounds/05.wav");
    
    float musicVol = (audio.master_volume * audio.music_volume)/100;

    configSoundManager.loadMusic("menuMusic", "./assets/music/01Underground.mp3");
    configSoundManager.playMusic("menuMusic", musicVol);
}

void VolumeConfGS::handleInput(sf::Event event){
    auto controls = configManager.getControls();
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == controls.down && position < 3) {    
            if (!volumeConfigSprites.empty()) {
                sf::Sprite torch = volumeConfigSprites.back();
                volumeConfigSprites.pop_back();

                position += 1;
                std::cout<<position<<std::endl;

                float torchX = configs[position+1].getPosition().x - 25.f;
                float torchY = configs[position+1].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                volumeConfigSprites.push_back(torch);
            }
        }

        if (keyPressed->scancode == controls.up && position > 0) {    
            if (!volumeConfigSprites.empty()) {
                sf::Sprite torch = volumeConfigSprites.back();
                volumeConfigSprites.pop_back();

                position -= 1;
                std::cout<<position<<std::endl;

                float torchX = configs[position+1].getPosition().x - 25.f;
                float torchY = configs[position+1].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                volumeConfigSprites.push_back(torch);
            }
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
                    break;
            }
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
                    break;
            }
        }

        if (keyPressed->scancode == controls.enter) {
            if (position == 3){ // Back to configs and save files
                configManager::Audio newAudio;
                newAudio.master_volume = masterVol;
                newAudio.music_volume = musicVol;
                newAudio.sound_volume = soundVol;

                configManager.setAudio(newAudio);
                configManager.saveConfiguration("config.json");

                auto audio = configManager.getAudio();
                float soundVol = (audio.master_volume * audio.sound_volume)/100;
                configSoundManager.playSound("menuEnter", soundVol);

                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait until the sound has finished
                std::cout << "Road back to config menu" << std::endl;
                stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
            }
        }
    }
}

void VolumeConfGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
}

void drawVolumeBars(sf::RenderWindow& window, int actualVol, sf::Vector2f pos){
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

void GameplayConfGS::init(){
    if(debug) std::cout << "ESTADO: Gameplay Config" << std::endl;
}

void GameplayConfGS::handleInput(sf::Event event){
    
}

void GameplayConfGS::update(float deltaTime, const sf::Vector2f& viewPosition){
    
}

void GameplayConfGS::draw(sf::RenderWindow& window, Camera& camera){
    
}

void GameplayConfGS::pause(){
    if(debug) std::cout << "ESTADO: Gameplay Config PAUSADO" << std::endl;
}

void GameplayConfGS::resume(){
    if(debug) std::cout << "ESTADO: Gameplay Config REANUDADO" << std::endl;
}

void GameplayConfGS::close(){
    if(debug) std::cout << "ESTADO: Gameplay Config CERRADO" << std::endl;
}

GameplayConfGS::~GameplayConfGS() {}