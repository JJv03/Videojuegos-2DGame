#pragma once
#include "player.h"
#include "tilemap.h"
#include "tilemapManager.h"
#include "soundManager.h"
#include "./enemies/enemyManager.h"
#include "./bosses/bossManager.h"
#include "item.h"
#include "globals.h"
#include "collisionGrid.h"
#include "configManager.h"
#include "particleSystem.h"


// Implements de logic of a castlevania game: Player, stats, enemies, map, etc.
class Game
{
public:
    int time = gLevelTimeLimit;
    configManager &configManager;

    Player player;
    TilemapManager tilemaps;
    CollisionGrid collisionGrid;
    EnemyManager *enemyManager;
    BossManager *bossManager;

    // Where all entities will be temporarily stored in order to
    // check their collisions
    std::vector<Entity *> staticEntities;
    std::vector<Entity *> dynamicEntities;

    size_t currentLevel = 0;
    size_t currentStage = 0;
    size_t currentBossPhase = 0;

    sf::Clock revivingClock;
    sf::Clock loadingClock;
    bool isLoading = false;
    bool restartLoadingClock = false;
    bool beginStageEntrance = false;

    std::vector<sf::Text> deadScreenTexts;
    std::vector<sf::Sprite> gSprites;

    ParticleSystem particleSystem;


    // Constructor, destructor
    Game();
    virtual ~Game() = default;

    // Initializes a new game from the beggining
    void init();

    // Loads the GUI of the game
    void loadGUI();

    // Loads the music of the game
    void loadMusic();

    // Loads the sounds of the game
    void loadSounds();

    // Effects changes depending on the input of the player
    void handleInput(sf::Event event);

    // Updates the game logic, graphics, etc
    void update(float deltaTime, const sf::Vector2f &viewPosition, bool windowHasFocus);

    // Updates in-game GUI time with <time> attribute
    void updateGUITime();

    // Updates in-game GUI stage with <time> attribute
    void updateGUIStage();

    // Prepares level variables to start a level
    void prepareVariablesForLevel();

    // Loads the level in the tilemap and the enemies
    void loadLevelAndEnemies();

    // Renders the game
    void draw(sf::RenderWindow &window, Camera &camera);

    // Gets the according view of the camera. Sticks to border if it exits mapBounds,
    // takes animations into account, etc
    // sf::View getView(sf::RenderWindow &window, Camera &camera);

    // Check all the possible collisions
    void checkCollisions();
    void checkCollisions(const sf::Vector2f &viewPosition);

    void checkSolidTileCollisions(std::vector<Entity *> &dynamicEntities);

    void checkDoorTileCollisions();

    // Check items collisions. An item only collides with what is below it and with a maximum of 2 tiles
    void checkItemsCollisions();

    // Check if the player is colliding with the tile that is passed
    // as argument "tileBounds". If it is, set hasCollided to true
    void computePlayerTileIntersection(bool &hasCollided, const sf::FloatRect &tileBounds);

    // Check map bound collisions
    void checkPlayerMapBoundCollisions();

    // Check collisions with the tilemap and items
    void checkPlayerCollisions();

    // Draw healthBar
    void drawHealthBars(sf::RenderWindow &window, int health, int bossHealth,
                        sf::Vector2f virtualWorldset);

    // Creates a drop item in a certain position
    void createDropItem(DropType dropType, sf::Vector2f itemPosition);

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
    std::shared_ptr<sf::Sprite> guiDoubleShotSprite;
    std::shared_ptr<sf::Sprite> guiTripleShotSprite;

    // Vectors with the diferents rectangles of each HealthBar
    std::vector<sf::RectangleShape> playerHealthBar;
    std::vector<sf::RectangleShape> enemyHealthBar;

    bool withOutLives;
    int position;

    // Blinking screen when rosary is used
    sf::Clock rosarioBlinkClock;
    bool isRosarioBlinking = false;
    float rosarioBlinkDuration = 1.0f;
    float rosarioBlinkInterval = 0.1f;

    // ----- All refered to the end level score animation -----
    bool m_playedVictoryMusic = false;      // To only play the victory music once
    float m_endLvlAnimationCoundown = gEND_LVL_ANIMATION_COUNDOWN;
    float m_endLvlAnimation_TimeScoreAccumulator = 0.f;     // To keep track of the speed at which Time is being deducted
    float m_endLvlAnimation_HeartScoreAccumulator = 0.f;    // To keep track of the speed at which Hearts are being deducted

    // Function that resets the variables that are used for the end level score animation
    void resetEndLevelScoreAnimation();

    // Animation that plays at the end of the level
    void endLevelScoreAnimation(const float deltaTime);

    AnimationManager *littleBatManager{nullptr};
    std::shared_ptr<sf::Sprite> littleBat;
    AnimationManager *bigBatManager{nullptr};
    std::shared_ptr<sf::Sprite> bigBat;
    AnimationManager *simonManager{nullptr};
    std::shared_ptr<sf::Sprite> simon;
    std::shared_ptr<sf::Sprite> map;
    std::shared_ptr<sf::Sprite> cube;
    bool entering = false;
    float timerSteps = 0.f;
    const float stepsInterval = 0.25f;
    void interAnimation(float deltaTime);
};
