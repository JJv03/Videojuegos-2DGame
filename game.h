#pragma once
#include "player.h"
#include "tilemap.h"
#include "tilemapManager.h"
#include "soundManager.h"
#include "./enemies/enemyManager.h"
#include "item.h"
#include "globals.h"
#include "collisionGrid.h"
#include "configManager.h"

// Implements de logic of a castlevania game: Player, stats, enemies, map, etc.
class Game
{
private:
    SoundManager gameSoundManager;
    int time = gLevelTimeLimit;
    configManager &configManager;

public:
    Player player;
    TilemapManager tilemaps;
    CollisionGrid collisionGrid;

    // Where all entities will be temporarily stored in order to
    // check their collisions
    std::vector<Entity*> staticEntities;
    std::vector<Entity*> dynamicEntities;

    size_t currentLevel = 0;
    size_t currentStage = 0;

    // Gestor de enemigos
    EnemyManager *enemyManager;


    sf::Clock revivingClock;
    sf::Clock loadingClock;
    bool isLoading = false;
    bool restartLoadingClock = false;
    bool beginStageEntrance = false;

    // Constructor, destructor
    Game();
    virtual ~Game() = default;

    // Initializes a new game from the beggining
    void init();

    // Effects changes depending on the input of the player
    void handleInput(sf::Event event);

    // Updates the game logic, graphics, etc
    void update(float deltaTime, const sf::Vector2f &viewPosition);

    // Updates in-game GUI time with <time> attribute
    void updateGUITime();

    // Renders the game
    void draw(sf::RenderWindow &window, Camera &camera);

    // Gets the according view of the camera. Sticks to border if it exits mapBounds,
    // takes animations into account, etc
    // sf::View getView(sf::RenderWindow &window, Camera &camera);

    // Check all the possible collisions
    void checkCollisions();
    void checkCollisions(const sf::Vector2f &viewPosition);

    void checkSolidTileCollisions(std::vector<Entity*> &dynamicEntities);

    void checkDoorTileCollisions();

    // Check items collisions. An item only collides with what is below it and with a maximum of 2 tiles
    void checkItemsCollisions();

    // Check if the player is colliding with the tile that is passed
    // as argument "tileBounds". If it is, set hasCollided to true
    void computePlayerTileIntersection(bool& hasCollided, const sf::FloatRect &tileBounds);

    // Check map bound collisions
    void checkPlayerMapBoundCollisions();

    // Check collisions with the tilemap and items
    void checkPlayerCollisions();

    // Draw healthBar
    void drawHealthBars(sf::RenderWindow &window, int health, int bossHealth,
                        sf::Vector2f virtualWorldset);

    // Creates a drop item in a certain position
    void createDropItem(sf::Vector2f itemPosition, DropType dropType);

    // Handles the interaction of Simon with an item
    void handleSimonInteractionWithItem(ItemType itemType);
    
    // Starts stage number <stage> at current level
    int startStage(int stage, int fromStairs = 0);

    // Moves player from a door to another door
    int goToStage(int fromDoor);

    void activateDoorTile(int doorId);

    void restartStage();

    void restartLevel();

    void setLevelMusic(int level);

    
private:
    // ----- All refered to the GUI -----
    sf::Font font;
    std::vector<sf::Text> texts;
    std::vector<sf::Vector2f> textPositions;

    // Subweapon sprite
    std::shared_ptr<sf::Sprite> guiSubWeaponSprite;
    std::shared_ptr<sf::Sprite> guiHeartSprite;

    // Vectors with the diferents rectangles of each HealthBar
    std::vector<sf::RectangleShape> playerHealthBar;
    std::vector<sf::RectangleShape> enemyHealthBar;
};
