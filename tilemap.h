#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "camera.h"
#include "globals.h"

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

    // Number of tiles in the tilemap
    int m_tilesPerRow;
    int m_tilesPerColumn;

    // Initial position when player starts stage
    sf::Vector2f initialPosition;

    // Tiles with hitboxes
    struct SolidTile {
        bool hasCollision = false;      // If <true>, the tile has hitbox.
        sf::FloatRect hitbox;           // Hitbox of the tile. In global coord.
        bool isVisible = false;         // If <true>, the tile is visible.
    };

    // Breakable tiles
    struct BreakableTile {
        enum class Type {   // Types of breakable tiles. Only in this scope
            FIREPIT = 0,
            CANDELABRUM = 1,
            BREAKABLE_WALL = 2,
        };

        enum class DropType {   // Types of items that can drop from breakable tiles. Only in this scope
            NONE = 0,                // No item drops
            DEFAULT = 1,            // Default item drop (whip, secondary weapon, etc). Should be handled in game logic
            CROWN = 2,
            MEAT = 3,
        };

        Type type;                      // Breakable type
        sf::Vector2f position;          // Position of the tile. In global coord.
        sf::FloatRect hitbox;           // Hitbox of the tile. In global coord.
        bool isBreakable = true;        // Enabling the tile to be destroyed
        bool isDestroyed = false;       // If the tile is destroyed
        DropType dropItem = DropType::NONE;          // If the tile drops an item
    };

    struct DoorTile {
        enum class Type{   // Types of door tiles. Only in this scope
            CASTLE_ENTRANCE = 0,
            DOOR = 1,
            STAIRS = 2,
        };

        sf::FloatRect hitbox;           // Hitbox of the tile. In global coord.
        Type type;                      // Door type
        sf::Vector2f playerAparition;           // Hitbox of the tile. In global coord.
    };


private:
    // Saves the map's tiles' vertex positions
    sf::VertexArray m_vertices;

    // Saves the map's whole tileset texture
    sf::Texture m_tileset;

    // Method that allows doing window.draw(tilemap) (although this is not used, drawScene is prefered)
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Function that processes the tilemap file
    void processFile(const std::string& file_path, std::vector<int>& solidTileNumberList);

    // Function that processes the tilemap file's map dimentions section, 
    // storing its tilesPerRow and tilesPerColumn
    void processFileMapDimensions(std::ifstream& file);

    // Function that processes the tilemap file's initial position section, 
    // storing the players initial position when starting the stage for the first time
    void processFileInitialPosition(std::ifstream& file);

    // Function that processes the tilemap file's solidTiles section, 
    // storing its numbers in <solitTileNumberList>
    void processFileSolidTiles(std::ifstream& file, std::vector<int>& solidTileNumberList);

    // Function that processes the tilemap file's door tiles section, 
    // storing its doors (type,positionX,positionY) in <m_doorTiles>
    void processFileDoorTiles(std::ifstream& file);

    // Function that processes the tilemap file's breakable tiles section, 
    // storing its breakableTiles (type,positionX,positionY) in <m_breakableTiles>
    void processFileBreakableTiles(std::ifstream& file);

    // Function that loads the textures of the breakable tiles
    bool loadBreakableTextures();

public:
    TileMap();
    ~TileMap();

    // Matrix with the properties of each solid tile.
    std::vector<std::vector<SolidTile>> m_solidTiles;

    // Vector with the properties of each breakable tile
    std::vector<BreakableTile> m_breakableTiles;

    // Vector with the properties of each door tile
    std::unordered_map<int,DoorTile> m_doorTiles;
    
    // Loads the tilemap with the given tiles
    bool load(int level, int stage);

    // Function to get the hitbox for a solid tile based on its ID
    sf::FloatRect getHitboxForSolidTile(const int level, const int id) const;

    // Function to get the hitbox for a breakable tile based on its ID
    sf::FloatRect getHitboxForBreakableTile(const int id) const;

    // Function to get the hitbox for a door tile based on its ID
    sf::FloatRect getHitboxForDoorTile(const int id) const;

    // Function that draws on the window the section of the tilemap that is visible through the camera
    void drawScene(sf::RenderWindow& window, Camera& camera);

    // Function that return the limit coordinates of the map
    sf::FloatRect getMapBounds() const;

    // Function that draws the hitboxes of the solid tiles
    void drawHitboxes(sf::RenderWindow& window) const;
};

// Function that converts a FloatRect to a RectangleShape
sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect, int color = 0);
