#include "tilemap.h"
#include <iostream>

int tileSize = 32;


bool TileMap::load(const std::string& tileset, const std::vector<int> tiles, unsigned int width, unsigned int height){
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(width * height * 6);

    if (!m_tileset.loadFromFile(tileset)) {
        return false;
    }

    int tilesPerRow = (m_tileset.getSize().x + 1) / (tileSize + 1); // Tiene en cuenta el pixel de margen entre tiles

    for (unsigned int i = 0; i < width; ++i)
            for (unsigned int j = 0; j < height; ++j)
            {
                int tileIndex = i + j * width;
                int tileNumber = tiles[tileIndex];

                // Encontrar su posicion en el tileset
                int tu = tileNumber % tilesPerRow;
                int tv = tileNumber / tilesPerRow;

                // Calcular las coordenadas en el tilemap **saltando el píxel vacío**
                int texX = tu * (tileSize + 1); // Sumamos el 1px de separación
                int texY = tv * (tileSize + 1);

                // Ir al primer vértice del tile
                sf::Vertex* triangle = &m_vertices[tileIndex * 6];

                
                // Asignar las posiciones
                triangle[0].position = sf::Vector2f(i * tileSize, j * tileSize);
                triangle[1].position = sf::Vector2f((i + 1) * tileSize, j * tileSize);
                triangle[2].position = sf::Vector2f(i * tileSize, (j + 1) * tileSize);
                triangle[3].position = sf::Vector2f(i * tileSize, (j + 1) * tileSize);
                triangle[4].position = sf::Vector2f((i + 1) * tileSize, j * tileSize);
                triangle[5].position = sf::Vector2f((i + 1) * tileSize, (j + 1) * tileSize);

                // Asignar las coordenadas
                triangle[0].texCoords = sf::Vector2f(texX, texY);
                triangle[1].texCoords = sf::Vector2f(texX + tileSize, texY);
                triangle[2].texCoords = sf::Vector2f(texX, texY + tileSize);
                triangle[3].texCoords = sf::Vector2f(texX, texY + tileSize);
                triangle[4].texCoords = sf::Vector2f(texX + tileSize, texY);
                triangle[5].texCoords = sf::Vector2f(texX + tileSize, texY + tileSize);
            }

        return true;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
}