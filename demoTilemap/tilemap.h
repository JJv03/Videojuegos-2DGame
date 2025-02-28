#pragma once

#include <SFML/Graphics.hpp>


class TileMap : public sf::Drawable, public sf::Transformable
{
private:
    // Guarda las posiciones de los vertices del tilemap
    sf::VertexArray m_vertices;

    // Guarda la textura del tilemap entero
    sf::Texture m_tileset;

    // Permite hacer window.draw(tilemap) directamente
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    
    
    bool load(const std::string& tileset, const std::vector<int> tiles, unsigned int width, unsigned int height);



};
