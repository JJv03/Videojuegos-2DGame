#pragma once

#include <SFML/Graphics.hpp>
#include <vector>


class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    struct SolidTileAttributes {
        bool hasCollision = false;      // If <true>, the tile has hitbox.
        sf::FloatRect hitbox;           // Hitbox of the tile. In local coord.
    };

    struct SpecialTileAttributes {
        enum class Type {   // Types of special tiles. Only in this scope
            Candelabrum,
            BreakableWall
        };

        sf::Vector2f position;          // Position of the tile in global coord.
        sf::FloatRect hitbox;           // Hitbox of the tile. In local coord.
        bool isBreakable = false;       // Enabling the tile to be destroyed
        bool isDestroyed = false;       // If the tile is destroyed
        Type type;
    };

    // Size of each tile (square) in pixels
    int m_tileSize = 32;

private:
    // Guarda las posiciones de los vertices del tilemap
    sf::VertexArray m_vertices;

    // Guarda la textura del tilemap entero
    sf::Texture m_tileset;

    // Permite hacer window.draw(tilemap) directamente
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    // Matrix with the properties of each solid tile.
    std::vector<std::vector<SolidTileAttributes>> m_solidTiles;

    // Vector with the properties of each special tile
    std::vector<SpecialTileAttributes> m_specialTiles;
    
    // Carga el tilemap con los tiles dados
    bool load(const std::string& tileset, const std::vector<int>& tiles, unsigned int width, unsigned int height);

    // Function to get the hitbox for a solid tile based on its ID
    sf::FloatRect getHitboxForSolidTile(const int id) const;

    // Function to get the hitbox for a special tile based on its ID
    sf::FloatRect getHitboxForSpecialTile(const int id) const;
};
