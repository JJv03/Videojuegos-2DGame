#pragma once
#include "player.h"
#include "tilemap.h"
#include "tilemapManager.h"
#include "soundManager.h"
#include "./enemies/createEnemies.h"
#include "item.h"

// Implements de logic of a castlevania game: Player, stats, enemies, map, etc.
class Game
{
private:
    SoundManager gameSoundManager;
    int time = 300;

public:
    Player player;
    TilemapManager tilemaps;
    size_t currentLevel = 0;
    size_t currentStage = 0;

    // Vectores de los enemigos
    std::vector<ZombieSpawner> zombiesSpawner;
    std::vector<Leopard> leopard;
    std::vector<Bat> bat;

    // std::vector<Item> items;

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
    void draw(sf::RenderWindow &window, Camera &camera);

    // Gets the according view of the camera. Sticks to border if it exits mapBounds,
    // takes animations into account, etc
    //sf::View getView(sf::RenderWindow &window, Camera &camera);

    // Check all the possible collisions
    void checkCollisions();

    // Check enemies collisions
    void checkEnemiesCollisions();

    // Check map bound collisions
    void checkPlayerMapBoundCollisions();

    // Check collisions with the tilemap
    void checkPlayerTileCollisions();

    // Draw healthBar
    void drawHealthBars(sf::RenderWindow &window, int health, int bossHealth,
                        sf::Vector2f virtualWorldset);

    // Starts stage number <stage> at current level
    int startStage(int stage, int fromDoor = 0);

    // Moves player from a door to another door
    int goToStage(int fromDoor);

private:
    // All refered to the GUI
    sf::Font font;
    std::vector<sf::Text> texts;
    std::vector<sf::Vector2f> textPositions;

    // Vectors with the diferents rectangles of each HealthBar
    std::vector<sf::RectangleShape> playerHealthBar;
    std::vector<sf::RectangleShape> enemyHealthBar;
};


/**
 * Obtiene las coordenadas virtuales de la esquina superior izquierda de la vista del juego.
 *
 * Calcula el píxel correspondiente a la esquina superior izquierda del rectángulo de la view
 * (que puede ser menor que la ventana por mantener el aspect-ratio) y lo convierte a coordenadas
 * del juego mediante mapPixelToCoords.
 *
 * @param window La ventana que contiene la view del juego.
 * @return Coordenadas virtuales de la esquina superior izquierda del área visible del juego.
 */
sf::Vector2f getVirtualUpperLeftCornerCoordOfGameView(sf::RenderWindow &window);