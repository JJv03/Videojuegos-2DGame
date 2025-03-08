#include "gameState.h"
#include <iostream>
#include "gameStateMachine.h"

constexpr auto KEY_RIGHT = sf::Keyboard::Scancode::Right;
constexpr auto KEY_LEFT = sf::Keyboard::Scancode::Left;
constexpr auto KEY_DOWN = sf::Keyboard::Scancode::Down;
constexpr auto KEY_UP = sf::Keyboard::Scancode::Up;
constexpr auto KEY_JUMP = sf::Keyboard::Scancode::X;
constexpr auto KEY_ATTACK = sf::Keyboard::Scancode::Z;

const bool debug = true;
// ======================================================
//                      GAME STATE 
// ======================================================

void GameGS::init(){
    if(debug) std::cout << "ESTADO: Game" << std::endl;
}

void GameGS::handleInput(Game game, sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            stateMachine->replaceState(std::make_unique<MenuGS>(stateMachine));
        }
        else if (keyPressed->scancode == KEY_UP)
        {    
            stateMachine->addState(std::make_unique<PauseGS>(stateMachine));
        }
    }
}

void GameGS::update(Game game, float deltaTime){
    
}

void GameGS::draw(Game game, sf::RenderWindow& window){
    
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
// std::unordered_map<std::string, sf::Texture> menuTextures;
// std::vector<sf::Sprite> menuSprites;
// sf::Font font;
// std::vector<sf::Text> texts;

void MenuGS::init(){
    if(debug) std::cout << "ESTADO: Menu" << std::endl;
    if (!menuTexture.loadFromFile("./assets/sprites/menu/menu2.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    menuSprite.setTexture(menuTexture);

    // Cargar la fuente
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        throw std::runtime_error("No se pudo cargar la fuente.");
    }
    if(debug) std::cout<<"Tras font"<<std::endl;

    // Definir opciones del menú
    std::string textos[4] = {"HISTORY MODE", "LEVEL SELECT", "CONFIG", "EXIT"};
    for (int i = 0; i < 4; i++) {
        options[i] = sf::Text(font, textos[i], 20); // Crear e inicializar directamente
        options[i].setFillColor(sf::Color::White);
        options[i].setPosition(sf::Vector2f(50.f, 120.f + i * 40.f));
    }

    if(debug) std::cout<<"Tras text"<<std::endl;
}

void MenuGS::handleInput(Game game, sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            stateMachine->replaceState(std::make_unique<PauseGS>(stateMachine));
        }
    }
}

void MenuGS::update(Game game, float deltaTime){
    
}

void MenuGS::draw(Game game, sf::RenderWindow& window){
    std::cout<<"Print"<<std::endl;
    window.draw(menuSprite);
    for (const auto& text : options) {
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
}


MenuGS::~MenuGS() {}


// ======================================================
//                      PAUSE STATE 
// ======================================================

void PauseGS::init(){
    if(debug) std::cout << "ESTADO: Pause" << std::endl;
}

void PauseGS::handleInput(Game game, sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            stateMachine->replaceState(std::make_unique<ConfigGS>(stateMachine));
        }
        else if (keyPressed->scancode == KEY_DOWN)
        {    
            stateMachine->removeState();
        }
    }
}

void PauseGS::update(Game game, float deltaTime){
    
}

void PauseGS::draw(Game game, sf::RenderWindow& window){
    
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

void ConfigGS::handleInput(Game game, sf::Event event){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_RIGHT) {    
            stateMachine->replaceState(std::make_unique<GameGS>(stateMachine));
        }
    }
}

void ConfigGS::update(Game game, float deltaTime){
    
}

void ConfigGS::draw(Game game, sf::RenderWindow& window){
    
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
