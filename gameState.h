#include <SFML/Graphics.hpp>
#include "game.h"
#pragma once

// Para evitar dependencias cíclicas
class GameStateMachine;

class GameState{
protected:
    GameStateMachine* stateMachine;
    
public:
    explicit GameState(GameStateMachine* stateMachine) : stateMachine(stateMachine) {}
    virtual ~GameState() = default;

    // Método que se llama al iniciarse el estado, es donde se inicializa todo
    virtual void init() = 0;

    // Método que gestiona los inputs del usuario
    virtual void handleInput(Game game, sf::Event event) = 0;

    // Método que gestiona la lógica del juego en cada frame
    virtual void update(Game game, float deltaTime, float windowScaleFactor) = 0;

    // Método que muestra en la ventana los gráficos
    virtual void draw(Game game, sf::RenderWindow& window, float windowScaleFactor) = 0;

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
};

class GameGS : public GameState{
public:
    explicit GameGS(GameStateMachine* machine) : GameState(machine) {}
    ~GameGS() override;
    void init() override;
    void handleInput(Game game, sf::Event event) override;
    void update(Game game, float deltaTime, float windowScaleFactor) override;
    void draw(Game game, sf::RenderWindow& window, float windowScaleFactor) override;
    void pause() override;
    void resume() override;
    void close() override;
};

class MenuGS : public GameState{
private:
    sf::Font font;
    std::vector<sf::Text> options;
public:
    explicit MenuGS(GameStateMachine* machine) : GameState(machine) {}
    ~MenuGS() override;
    void init() override;
    void handleInput(Game game, sf::Event event) override;
    void update(Game game, float deltaTime, float windowScaleFactor) override;
    void draw(Game game, sf::RenderWindow& window, float windowScaleFactor) override;
    void pause() override;
    void resume() override;
    void close() override;
};

class PauseGS : public GameState{
public:
    explicit PauseGS(GameStateMachine* machine) : GameState(machine) {}
    ~PauseGS() override;
    void init() override;
    void handleInput(Game game, sf::Event event) override;
    void update(Game game, float deltaTime, float windowScaleFactor) override;
    void draw(Game game, sf::RenderWindow& window, float windowScaleFactor) override;
    void pause() override;
    void resume() override;
    void close() override;
};

class ConfigGS : public GameState{
public:
    explicit ConfigGS(GameStateMachine* machine) : GameState(machine) {}
    ~ConfigGS() override;
    void init() override;
    void handleInput(Game game, sf::Event event) override;
    void update(Game game, float deltaTime, float windowScaleFactor) override;
    void draw(Game game, sf::RenderWindow& window, float windowScaleFactor) override;
    void pause() override;
    void resume() override;
    void close() override;
};