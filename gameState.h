#include <SFML/Graphics>
#pragma once

class GameState{
public:
    // Método que se llama al iniciarse el estado, es donde se inicializa todo
    virtual void init() = 0;

    // Método que gestiona los inputs del usuario
    virtual void handleInput(sf::Event event) = 0;

    // Método que gestiona la lógica del juego en cada frame
    virtual void update(float deltaTime) = 0;

    // Método que muestra en la ventana los gráficos
    virtual void draw(sf::Window window) = 0;

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



class GameGS{
public:
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::Window window) override;
    void pause() override;
    void resume() override;
    void close() override;
};


class MenuGS{
public:
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::Window window) override;
    void pause() override;
    void resume() override;
    void close() override;
};


class PauseGS{
public:
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::Window window) override;
    void pause() override;
    void resume() override;
    void close() override;
};


class ConfigGS{
public:
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void draw(sf::Window window) override;
    void pause() override;
    void resume() override;
    void close() override;
};


// AÑADIR LOS ESTADOS QUE FALTEN