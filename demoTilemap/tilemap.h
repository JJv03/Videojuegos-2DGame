#pragma once

#include <SFML/Graphics.hpp>


class TileMap : public sf::Drawable, public sf::Transformable
{
private:


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    // Guarda las posiciones de los vertices del tilemap
    sf::VertexArray m_vertices;

    // Guarda la textura del tilemap entero
    sf::Texture m_tileset;
    
    bool load(const std::string& tileset, const int* tiles, unsigned int width, unsigned int height);



};
