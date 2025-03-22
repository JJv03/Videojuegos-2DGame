#include "gameState.h"
#include <iostream>
#include "gameStateMachine.h"
#include "globals.h"

#include <thread> // Necesario para sleep_for
#include <chrono>

constexpr sf::Keyboard::Scancode KEY_RIGHT = sf::Keyboard::Scancode::Right;
constexpr sf::Keyboard::Scancode KEY_LEFT = sf::Keyboard::Scancode::Left;
constexpr sf::Keyboard::Scancode KEY_DOWN = sf::Keyboard::Scancode::Down;
constexpr sf::Keyboard::Scancode KEY_UP = sf::Keyboard::Scancode::Up;
constexpr sf::Keyboard::Scancode KEY_ENTER = sf::Keyboard::Scancode::Enter;
constexpr sf::Keyboard::Scancode KEY_ESC = sf::Keyboard::Scancode::Escape;
constexpr sf::Keyboard::Scancode KEY_JUMP = sf::Keyboard::Scancode::X;
constexpr sf::Keyboard::Scancode KEY_ATTACK = sf::Keyboard::Scancode::Z;

sf::View GameState::getView(sf::RenderWindow& window, Camera& camera){
    return camera.getView(window.getSize());
}



const bool debug = false;
// ======================================================
//                      GAME STATE 
// ======================================================

void GameGS::init(){
    if(debug) std::cout << "ESTADO: Game" << std::endl;
    gameSoundManager.loadMusic("gameMusic", "./assets/music/03Vampire_Killer.mp3");
    gameSoundManager.playMusic("gameMusic", gMusicVolume);
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

void GameGS::update(float deltaTime){
    game.update(deltaTime);
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

sf::View GameGS::getView(sf::RenderWindow& window, Camera& camera){
    return game.getView(window, camera);
}


// ======================================================
//                      MENU STATE 
// ======================================================
std::unordered_map<std::string, sf::Texture> menuTextures;
std::vector<sf::Sprite> menuSprites;

void MenuGS::init(){
    if(debug) std::cout << "ESTADO: Menu" << std::endl;
    position = 0;
    if (!menuTextures["menu"].loadFromFile("./assets/sprites/menu/menu2.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite menu(menuTextures["menu"]);

    sf::FloatRect spriteBounds = menu.getLocalBounds();
    float spriteWidth = spriteBounds.size.x;
    float spriteHeight = spriteBounds.size.y;

    // Calcular el factor de escala basado en la altura máxima (250 píxeles)
    float scaleFactor = gWindowHeight / spriteHeight;

    // Aplicar el factor de escala al sprite para mantener la relación de aspecto
    menu.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    // Obtener las nuevas dimensiones del sprite escalado
    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    // Calcular la posición para centrar el sprite en la pantalla
    float xPosition = (gWindowWidth - scaledWidth) / 2;
    float yPosition = (gWindowHeight - scaledHeight) / 2;

    // Establecer la posición del sprite
    menu.setPosition(sf::Vector2f(xPosition, yPosition));

    menuSprites.push_back(menu);

    // Cargar la fuente 
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

    // Definir opciones del menú
    std::string textos[4] = {"STORY MODE", "LEVEL SELECT", "CONFIG", "EXIT"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 30);
        text.setFillColor(sf::Color::White);
        // text.setPosition(sf::Vector2f(330.f, 80.f + i * 40.f));
        sf::FloatRect textBounds = text.getLocalBounds();

        // Calcular posición centrada
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
    
    menuSoundManager.loadMusic("menuMusic", "./assets/music/08Out_of_Time.mp3");
    menuSoundManager.playMusic("menuMusic", gMusicVolume);
}

void MenuGS::handleInput(sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_DOWN && position < 3) {    
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

        if (keyPressed->scancode == KEY_UP && position > 0) {    
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

        if (keyPressed->scancode == KEY_ENTER) {
            menuSoundManager.playSound("menuEnter", gSoundVolume);
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

void MenuGS::update(float deltaTime){
    
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

void PauseGS::update(float deltaTime){
    
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

void ConfigGS::init(){
    if(debug) std::cout << "ESTADO: Config" << std::endl;
}

void ConfigGS::handleInput(sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
        }
    }
}

void ConfigGS::update(float deltaTime){
    
}

void ConfigGS::draw(sf::RenderWindow& window, Camera& camera){
    
}

void ConfigGS::pause(){
    if(debug) std::cout << "ESTADO: Config PAUSADO" << std::endl;
}

void ConfigGS::resume(){
    if(debug) std::cout << "ESTADO: Config REANUDADO" << std::endl;
}

void ConfigGS::close(){
    if(debug) std::cout << "ESTADO: Config CERRADO" << std::endl;
}

ConfigGS::~ConfigGS() {}
