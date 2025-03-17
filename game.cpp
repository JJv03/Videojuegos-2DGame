#include "game.h"
#include <iostream>

// Constructor, destructor
Game::Game(){
    player = Player();
    tileMap = TileMap();
}

// Initializes a new game from the beggining
void Game::init(){
    if (!tileMap.load("./assets/tilesets/tileset_1.png", "./assets/tilesets/tilemap_1_1.txt", 24, 7)){
        std::cerr << "Error al cargar el tilemap." << std::endl;
        return;
    }
}

// Effects changes depending on the input of the player
void Game::handleInput(sf::Event event){
    player.handleInput(event);
}

// Updates the game (logic, graphics, etc)
void Game::update(float deltaTime){

}

// Renders the game (player, tilemap, enemies, objects, etc)
void Game::draw(sf::RenderWindow& window, Camera& camera){
    tileMap.drawScene(window, camera);
    player.draw(window);
}