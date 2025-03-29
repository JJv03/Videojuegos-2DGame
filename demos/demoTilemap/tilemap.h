#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "camera.h"

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    struct SolidTile {
        sf::Vector2f position;          // Position of the tile in global coord.
        bool hasCollision = false;      // If <true>, the tile has hitbox.
        sf::FloatRect hitbox;           // Hitbox of the tile. In local coord.
    };

    struct SpecialTile {
        enum class Type {   // Types of special tiles. Only in this scope
            Candelabrum,
            BreakableWall,
            Bonfire
        };

        sf::Sprite sprite;              // Sprite of the special tile
        bool isBreakable = false;       // Enabling the tile to be destroyed
        bool isDestroyed = false;       // If the tile is destroyed
        Type type;
    };

    // Size of each tile (square) in pixels
    int m_tileSize = 32;

    // Number of tiles in the tilemap
    int tilesPerRow;
    int tilesPerColumn;


private:
    // Guarda las posiciones de los vertices del tilemap
    sf::VertexArray m_vertices;

    // Guarda la textura del tilemap entero
    sf::Texture m_tileset;

    sf::Texture m_hoguera;

    // Permite hacer window.draw(tilemap) directamente
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    // Matrix with the properties of each solid tile.
    std::vector<std::vector<SolidTile>> solidTiles;

    // Vector with the all the special tiles in the tilemap
    std::vector<SpecialTile> m_specialTiles;

    //std::vector<sf::Sprite> vectHoguera;
    
    // Loads the tilemap with the given tiles
    bool load(const std::string& tileset_path, const std::string& tilemap_path, unsigned int width, unsigned int height);

    // Function to get the position of a tile based on its offset and position in the tilemap
    sf::Vector2f getPositionForOffset(const int x, const int y, const int x_offset, const int y_offset) const;

    // Function to get the hitbox for a solid tile based on its ID
    sf::FloatRect getHitboxForSolidTile(const int id) const;

    // Function to get the hitbox for a special tile based on its ID
    sf::FloatRect getHitboxForBreakableTile(const int id) const;

    // Function that draws on the window the section of the tilemap that is visible through the camera
    void drawScene(sf::RenderWindow& window, const Camera& camera);
};


void leerNumeros(const std::string& archivo, std::vector<int>& listaNumeros);