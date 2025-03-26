#pragma once
#include <SFML/Graphics.hpp>
#include "game.h"
#include "soundManager.h"
#include "globals.h"

// Para evitar dependencias cíclicas
class GameStateMachine;

class GameState{
protected:
    GameStateMachine* stateMachine;
    
public:
    sf::Vector2f m_viewSize;
    explicit GameState(GameStateMachine* stateMachine) : stateMachine(stateMachine) {}
    virtual ~GameState() = default;

    // Método que se llama al iniciarse el estado, es donde se inicializa todo
    virtual void init() = 0;

    // Método que gestiona los inputs del usuario
    virtual void handleInput(sf::Event event) = 0;

    // Método que gestiona la lógica del juego en cada frame
    virtual void update(float deltaTime) = 0;

    // Método que muestra en la ventana los gráficos
    virtual void draw(sf::RenderWindow& window, Camera& camera) = 0;

    // Método que se llama al reemplazar un estado por otro.
    // Pausa las animaciones, enemigos, sonidos, músicas...
    virtual void pause(){}

    // Método que se llama al reanudar un estado que había sido pausado.
    // Reanuda las animaciones, enemigos, sonidos, músicas...
    virtual void resume(){}

    // Método que se llama al terminar el estado, se destruyen las instancias
    // que ocupan memoria, se detienen músicas/sonidos, etc.
    // No es virtual pura por si algún estado más simple no necesita hacer nada al cerrarse
    virtual void close(){}

    // Function that returns the camera's view
    virtual sf::View getView(sf::RenderWindow& window, Camera& camera);
};


class GameGS : public GameState{
public:
    Game game;

    explicit GameGS(GameStateMachine* machine) : GameState(machine), game() {}
    ~GameGS() override;
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window, Camera& camera) override;
    void pause() override;
    void resume() override;
    void close() override;
    sf::View getView(sf::RenderWindow& window, Camera& camera) override;
};


class MenuGS : public GameState{
private:
    // std::unordered_map<std::string, sf::Texture> menuTextures;
    // std::vector<sf::Sprite> menuSprites;
    sf::Font font;
    std::vector<sf::Text> options;
    int position;
    SoundManager menuSoundManager;
public:
    explicit MenuGS(GameStateMachine* machine) : GameState(machine) {}
    ~MenuGS() override;
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window, Camera& camera) override;
    void pause() override;
    void resume() override;
    void close() override;
    sf::View getView(sf::RenderWindow& window, Camera& camera) override;
};


class PauseGS : public GameState{
public:
    explicit PauseGS(GameStateMachine* machine) : GameState(machine) {}
    ~PauseGS() override;
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window, Camera& camera) override;
    void pause() override;
    void resume() override;
    void close() override;
};


class ConfigGS : public GameState{
public:
    explicit ConfigGS(GameStateMachine* machine) : GameState(machine) {}
    ~ConfigGS() override;
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window, Camera& camera) override;
    void pause() override;
    void resume() override;
    void close() override;
};


// AÑADIR LOS ESTADOS QUE FALTEN