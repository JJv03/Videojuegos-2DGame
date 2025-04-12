#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "camera.h"
#include "globals.h"
#include "tile.h"
#include "item.h"

using BreakableType = BreakableTile::Type;



class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    // Number of tiles in the tilemap
    int m_tilesPerRow;
    int m_tilesPerColumn;

    // Initial position when player starts stage
    sf::Vector2f initialPosition;

    // Hash function for the breakable tile type
    struct BreakableTypeHash
    {
        std::size_t operator()(const BreakableType &t) const
        {
            return std::hash<int>()(static_cast<int>(t));
        }
    };

    // Dicctionary with the textures of the breakable tiles
    static std::unordered_map<BreakableType, std::shared_ptr<sf::Texture>, BreakableTypeHash> breakableTextures;

private:
    // Saves the map's tiles' vertex positions
    sf::VertexArray m_vertices;

    // Saves the map's whole tileset texture
    sf::Texture m_tileset;

    // Function to get the hitbox for a solid tile based on its ID
    sf::FloatRect getHitboxForSolidTile(const int level, const int id) const;

    // Function to get the hitbox for a breakable tile based on its ID
    sf::FloatRect getHitboxForBreakableTile(const int id) const;

    // Function to get the hitbox for a door tile based on its ID
    sf::FloatRect getHitboxForDoorTile(const int id) const;

    // Method that allows doing window.draw(tilemap) (although this is not used, drawScene is prefered)
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // Function that processes the tilemap file
    void processFile(const std::string &file_path, std::vector<int> &solidTileNumberList);

    // Function that processes the tilemap file's map dimentions section,
    // storing its tilesPerRow and tilesPerColumn
    void processFileMapDimensions(std::ifstream &file);

    // Function that processes the tilemap file's initial position section,
    // storing the players initial position when starting the stage for the first time
    void processFileInitialPosition(std::ifstream &file);

    // Function that processes the tilemap file's solidTiles section,
    // storing its numbers in <solitTileNumberList>
    void processFileSolidTiles(std::ifstream &file, std::vector<int> &solidTileNumberList);

    // Function that processes the tilemap file's door tiles section,
    // storing its doors (type,positionX,positionY) in <m_doorTiles>
    void processFileDoorTiles(std::ifstream &file);

    // Function that processes the tilemap file's breakable tiles section,
    // storing its breakableTiles (type,positionX,positionY) in <m_breakableTiles>
    void processFileBreakableTiles(std::ifstream &file);

    // Function to process the enemy section in tilemap file
    void processFileEnemies(std::ifstream &file);

    // Function that loads the textures of the breakable tiles
    static bool loadBreakableTextures();

public:
    TileMap();
    ~TileMap();

    struct EnemyData
    {
        enum class Type
        {
            ZOMBIE = 0,
            LEOPARD = 1,
            BAT = 2,
        };

        int type;              // Enemy type (0: Zombie, 1: Leopard, 2: Bat)
        sf::Vector2f position; // Position of the enemy
        float width;           // Optional: width of spawning area
        float height;          // Optional: height of spawning area
    };

    // Vector to store enemy data
    std::vector<EnemyData> m_enemyData;

    // Vector to store the items in the tilemap
    std::vector<std::shared_ptr<Item>> m_items;

    // Matrix with the properties of each solid tile.
    std::vector<std::vector<SolidTile>> m_solidTiles;

    // Vector with the properties of each breakable tile
    std::vector<BreakableTile> m_breakableTiles;

    // Vector with the properties of each door tile
    std::vector<DoorTile> m_doorTiles;

    // Loads the tilemap with the given tiles
    bool load(int level, int stage);

    // Function that updates the items in the tilemap and erases the ones that have no lifeTime left
    void updateItems(const float& deltaTime);

    // Function that draws on the window the section of the tilemap that is visible through the camera
    void drawScene(sf::RenderWindow &window, Camera &camera);

    // Function that return the limit coordinates of the map
    sf::FloatRect getMapBounds() const;

    // Function that draws the hitboxes of the solid tiles
    void drawHitboxes(sf::RenderWindow &window) const;
};

// Function that converts a FloatRect to a RectangleShape
sf::RectangleShape FloatRectToRectShape(const sf::FloatRect &floatRect, int color = 0);
