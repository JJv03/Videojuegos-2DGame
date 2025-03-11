#include "tilemap.h"
#include <iostream>
#include <fstream>

sf::FloatRect TileMap::getHitboxForSolidTile(const int id) const
{
    switch (id) {
        case 1:
            return sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f}); // No collision
        case 2:
            return sf::FloatRect({0.0f, 0.0f}, {m_tileSize, m_tileSize}); // Full collision
        case 4:
            return sf::FloatRect({0.0f, m_tileSize/2.0f}, {m_tileSize, m_tileSize / 2.0f}); // Bottom half of the tile has collision
        case 37:
            return sf::FloatRect({0.0f, 0.0f}, {m_tileSize, m_tileSize / 2.0f}); // Top half of the tile has collision
        case 6:
            return sf::FloatRect({0.0f, m_tileSize/2.0f}, {0.0f, m_tileSize / 2.0f}); // Bottom left of the tile has collision
        case 7:
            return sf::FloatRect({m_tileSize/2.0f, m_tileSize/2.0f}, {m_tileSize/2.0f, m_tileSize / 2.0f}); // Bottom right of the tile has collision
        default:
            return sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f}); // Default: none collisionable hitbox
    }
}

sf::FloatRect TileMap::getHitboxForSpecialTile(const int id) const
{
    switch (id) {
        case 2:
            return sf::FloatRect({0.0f, 0.0f}, {m_tileSize, m_tileSize}); // Full collision
        case 4:
            return sf::FloatRect({0.0f, m_tileSize/2.0f}, {m_tileSize, m_tileSize / 2.0f}); // Bottom half of the tile has collision
        default:
            return sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f}); // Default: none collisionable hitbox
    }
}



bool TileMap::load(const std::string& tileset_path, const std::string& tilemap_path, unsigned int width, unsigned int height) {
    std::vector<int> tilemap;
    leerNumeros(tilemap_path, tilemap);
    
    m_solidTiles.resize(height);
    for (auto& row : m_solidTiles) {
        row.resize(width);
    }

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(width * height * 6);

    if (!m_tileset.loadFromFile(tileset_path)) {
        return false;
    }

    int tilesPerRow = (m_tileset.getSize().x + 1) / (m_tileSize + 1); // Tiene en cuenta el pixel de margen entre tiles

    for (unsigned int i = 0; i < width; ++i)
    {
        for (unsigned int j = 0; j < height; ++j)
        {
            int tileIndex = i + j * width;
            int tileNumber = tilemap[tileIndex];

            // Encontrar su posicion en el tileset
            int tileset_row = tileNumber % tilesPerRow;
            int tileset_column = tileNumber / tilesPerRow;

            // Calcular las coordenadas en el tilemap **saltando el píxel vacío**
            int texX = tileset_row * (m_tileSize + 1); // Sumamos el 1px de separación
            int texY = tileset_column * (m_tileSize + 1);

            // Ir al primer vértice del tile
            sf::Vertex* triangle = &m_vertices[tileIndex * 6];

            // Asignar las posiciones
            triangle[0].position = sf::Vector2f(i * m_tileSize, j * m_tileSize);
            triangle[1].position = sf::Vector2f((i + 1) * m_tileSize, j * m_tileSize);
            triangle[2].position = sf::Vector2f(i * m_tileSize, (j + 1) * m_tileSize);
            triangle[3].position = sf::Vector2f(i * m_tileSize, (j + 1) * m_tileSize);
            triangle[4].position = sf::Vector2f((i + 1) * m_tileSize, j * m_tileSize);
            triangle[5].position = sf::Vector2f((i + 1) * m_tileSize, (j + 1) * m_tileSize);

            // Asignar las coordenadas
            triangle[0].texCoords = sf::Vector2f(texX, texY);
            triangle[1].texCoords = sf::Vector2f(texX + m_tileSize, texY);
            triangle[2].texCoords = sf::Vector2f(texX, texY + m_tileSize);
            triangle[3].texCoords = sf::Vector2f(texX, texY + m_tileSize);
            triangle[4].texCoords = sf::Vector2f(texX + m_tileSize, texY);
            triangle[5].texCoords = sf::Vector2f(texX + m_tileSize, texY + m_tileSize);

            // Set the hitbox for the solid tile
            m_solidTiles[j][i].hitbox = getHitboxForSolidTile(tileNumber);
            m_solidTiles[j][i].position = sf::Vector2f(i * m_tileSize, j * m_tileSize);
        }
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




void leerNumeros(const std::string& archivo, std::vector<int>& listaNumeros) {
    std::ifstream file(archivo);
    std::string linea;
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return;
    }

    while (std::getline(file, linea)) {
        std::stringstream ss(linea);
        std::string numeroStr;
        
        // Procesar cada número separado por coma
        while (std::getline(ss, numeroStr, ',')) {
            try {
                int numero = std::stoi(numeroStr); // Convertir de string a int
                listaNumeros.push_back(numero); // Añadir el número a la lista
            } catch (const std::invalid_argument&) {
                // Si no se puede convertir el número, lo ignoramos
                std::cerr << "Número inválido en el archivo: " << numeroStr << std::endl;
            }
        }
    }

    file.close();
}
