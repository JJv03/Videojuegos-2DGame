#include "gameState.h"
#include <iostream>
#include "gameStateMachine.h"

constexpr auto KEY_RIGHT = sf::Keyboard::Scancode::Right;
constexpr auto KEY_LEFT = sf::Keyboard::Scancode::Left;
constexpr auto KEY_DOWN = sf::Keyboard::Scancode::Down;
constexpr auto KEY_UP = sf::Keyboard::Scancode::Up;
constexpr auto KEY_ENTER = sf::Keyboard::Scancode::Enter;
constexpr auto KEY_JUMP = sf::Keyboard::Scancode::X;
constexpr auto KEY_ATTACK = sf::Keyboard::Scancode::Z;

const bool debug = true;

int screenWidth = 768;
int screenHeight = 250;
// ======================================================
//                      GAME STATE 
// ======================================================

void GameGS::init(){
    if(debug) std::cout << "ESTADO: Game" << std::endl;
}

void GameGS::handleInput(Game game, sf::Event event, sf::RenderWindow& window){
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
    float scaleFactor = screenHeight / spriteHeight;

    // Aplicar el factor de escala al sprite para mantener la relación de aspecto
    menu.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    // Obtener las nuevas dimensiones del sprite escalado
    float scaledWidth = spriteWidth * scaleFactor;
    float scaledHeight = spriteHeight * scaleFactor;

    // Calcular la posición para centrar el sprite en la pantalla
    float xPosition = (screenWidth - scaledWidth) / 2;
    float yPosition = (screenHeight - scaledHeight) / 2;

    // Establecer la posición del sprite
    menu.setPosition(sf::Vector2f(xPosition, yPosition));

    menuSprites.push_back(menu);

    // Cargar la fuente 
    if (!font.openFromFile("./assets/fonts/credits/castlevania-nes-end-credits.ttf")) {
        std::cout<<"No se ha encontrado la fuente"<<std::endl;
        throw std::runtime_error("No se pudo cargar la fuente.");
    }

    // Definir opciones del menú
    std::string textos[4] = {"HISTORY MODE", "LEVEL SELECT", "CONFIG", "EXIT"};
    for (int i = 0; i < 4; i++) {
        sf::Text text(font, textos[i], 20);
        text.setFillColor(sf::Color::White);
        // text.setPosition(sf::Vector2f(330.f, 80.f + i * 40.f));
        sf::FloatRect textBounds = text.getLocalBounds();

        // Calcular posición centrada
        float xPos = (screenWidth - textBounds.size.x) / 2;
        float yPos = 120.f + i * 32.f;

        text.setPosition(sf::Vector2f(xPos, yPos));
        options.push_back(text);
    }

    if (!menuTextures["torch"].loadFromFile("./assets/sprites/menu/selectorMenu.png")) {
        throw std::runtime_error("No se pudo cargar la imagen del menú.");
    }
    sf::Sprite torch(menuTextures["torch"]);

    torch.setScale(sf::Vector2f(torch.getScale().x * 1.1f, torch.getScale().y * 1.1f));

    float torchX = options[0].getPosition().x - 25.f;
    float torchY = options[0].getPosition().y + 2.f;
    torch.setPosition(sf::Vector2f(torchX, torchY));

    menuSprites.push_back(torch);
}

void MenuGS::handleInput(Game game, sf::Event event, sf::RenderWindow& window){
    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        if (keyPressed->scancode == KEY_DOWN && position < 3) {    
            if (!menuSprites.empty()) {
                sf::Sprite torch = menuSprites.back();  // Obtener el último sprite
                menuSprites.pop_back();                 // Eliminarlo del vector

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
                sf::Sprite torch = menuSprites.back();  // Obtener el último sprite
                menuSprites.pop_back();                 // Eliminarlo del vector

                position -= 1;
                std::cout<<position<<std::endl;

                float torchX = options[position].getPosition().x - 25.f;
                float torchY = options[position].getPosition().y + 2.f;
                torch.setPosition(sf::Vector2f(torchX, torchY));

                menuSprites.push_back(torch);
            }
        }

        if (keyPressed->scancode == KEY_ENTER) {
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
                    window.close();
                    break;
            }
            //stateMachine->replaceState(std::make_unique<PauseGS>(stateMachine));
        }
    }
}

void MenuGS::update(Game game, float deltaTime){
    
}

void MenuGS::draw(Game game, sf::RenderWindow& window){
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

void PauseGS::handleInput(Game game, sf::Event event, sf::RenderWindow& window){
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

void ConfigGS::handleInput(Game game, sf::Event event, sf::RenderWindow& window){
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
