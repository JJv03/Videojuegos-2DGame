#pragma once
#include "player.h"
#include "tilemap.h"
#include "tilemapManager.h"
#include "soundManager.h"

// Implements de logic of a castlevania game: Player, stats, enemies, map, etc.
class Game{
private:
    SoundManager gameSoundManager;
    int time = 300;
public:
    Player player;
    TilemapManager tilemaps;
    size_t currentLevel = 0;
    size_t currentStage = 0;

    sf::Clock loadingClock;
    bool isLoading = false;

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
    
    // Gets the according view of the camera. Sticks to border if it exits mapBounds, 
    // takes animations into account, etc
    sf::View getView(sf::RenderWindow& window, Camera& camera);

    // Check all the possible collisions
    void checkCollisions();

    // Check map bound collisions
    void checkPlayerMapBoundCollisions();

    // Check collisions with the tilemap
    void checkPlayerTileCollisions();

    // Draw healthBar
    void drawHealthBars(sf::RenderWindow& window, int health, int bossHealth);

    // Starts stage number <stage> at current level
    int startStage(int stage);

    // Moves player from a door to another door
    int goToStage(int fromDoor);


private:
    // All refered to the GUI
    sf::Font font;
    std::vector<sf::Text> texts;
    // Vectors with the diferents rectangles of each HealthBar
    std::vector<sf::RectangleShape> playerHealthBar;
    std::vector<sf::RectangleShape> enemyHealthBar;
};