#pragma once
#include "player.h"
#include "tilemap.h"

// Implements de logic of a castlevania game: Player, stats, enemies, map, etc.
class Game{
public:
    Player player;
    TileMap tileMap; // temporary, will be substituted by a tilemap manager

    // Constructor, destructor
    Game();
    virtual ~Game() = default;

    // Initializes a new game from the beggining
    void init();

    // Effects changes depending on the input of the player
    void handleInput(sf::Event event);

    // Updates the game logic, graphics, etc
    void update(float deltaTime);

    // Renders the game
    void draw(sf::RenderWindow& window, Camera& camera);
    
    sf::View getView(sf::RenderWindow& window, Camera& camera);

private:
    // All refered to the GUI
    sf::Font font;
    std::vector<sf::Text> texts;
    // Vectors with the diferents rectangles of each HealthBar
    std::vector<sf::RectangleShape> playerHealthBar;
    std::vector<sf::RectangleShape> enemyHealthBar;
};