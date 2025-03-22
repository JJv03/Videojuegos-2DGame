#include "tilemap.h"
#include <iostream>
#include <fstream>

sf::FloatRect TileMap::getHitboxForSolidTile(const int id) const
{
    switch (id) {
        case 1:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f)); // No collision
        case 2:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(m_tileSize, m_tileSize)); // Full collision
        case 4:
            return sf::FloatRect(sf::Vector2f(0.0f, m_tileSize/2.0f), sf::Vector2f(m_tileSize, m_tileSize / 2.0f)); // Bottom half of the tile has collision
        case 37:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(m_tileSize, m_tileSize / 2.0f)); // Top half of the tile has collision
        case 6:
            return sf::FloatRect(sf::Vector2f(0.0f, m_tileSize/2.0f), sf::Vector2f(0.0f, m_tileSize / 2.0f)); // Bottom left of the tile has collision
        case 7:
            return sf::FloatRect(sf::Vector2f(m_tileSize/2.0f, m_tileSize/2.0f), sf::Vector2f(m_tileSize/2.0f, m_tileSize / 2.0f)); // Bottom right of the tile has collision
        default:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f)); // Default: none collisionable hitbox
    }
}

sf::FloatRect TileMap::getHitboxForSpecialTile(const int id) const
{
    switch (id) {
        case 2:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(m_tileSize, m_tileSize)); // Full collision
        case 4:
            return sf::FloatRect(sf::Vector2f(0.0f, m_tileSize/2.0f), sf::Vector2f(m_tileSize, m_tileSize / 2.0f)); // Bottom half of the tile has collision
        default:
            return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f)); // Default: none collisionable hitbox
    }
}



bool TileMap::load(const std::string& tileset_path, const std::string& tilemap_path) {

    std::vector<int> tilemap;
    processFile(tilemap_path, tilemap);
    
    m_solidTiles.resize(m_tilesPerColumn);
    for (auto& row : m_solidTiles) {
        row.resize(m_tilesPerRow);
    }

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(m_tilesPerColumn * m_tilesPerRow * 6);

    if (!m_tileset.loadFromFile(tileset_path)) {
        return false;
    }

    m_tileset.setSmooth(false);

    int tilesPerRow = (m_tileset.getSize().x + 1) / (m_tileSize + 1); // Tiene en cuenta el pixel de margen entre tiles

    for (int i = 0; i < m_tilesPerRow; ++i)
    {
        for (int j = 0; j < m_tilesPerColumn; ++j)
        {
            int tileIndex = i + j * m_tilesPerRow;
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
            m_solidTiles[j][i].hitbox.position = sf::Vector2f(i * m_tileSize, j * m_tileSize);
            m_solidTiles[j][i].position = sf::Vector2f(i * m_tileSize, j * m_tileSize);
        }
    }
    return true;
}

void TileMap::drawHitboxes(sf::RenderWindow& window) const
{
    for (size_t i = 0; i < this->m_solidTiles.size(); ++i) {
        for (size_t j = 0; j < this->m_solidTiles[i].size(); ++j) {
            sf::RectangleShape rect = FloatRectToRectShape(this->m_solidTiles[i][j].hitbox);
            rect.setPosition({static_cast<float>(j) * this->m_tileSize, static_cast<float>(i) * this->m_tileSize});
            window.draw(rect);
        }
    }
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


void TileMap::drawScene(sf::RenderWindow& window, Camera& camera){
    sf::View view = camera.getView(window.getSize());

    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();

    float left   = center.x - size.x / 2.f; // Adding a 100.f offset will show how it works
    float right  = center.x + size.x / 2.f; // Subtracting a 100.f offset will show how it works
    float top    = center.y - size.y / 2.f;
    float bottom = center.y + size.y / 2.f;

    // Show coords
    //std::cout << "Left: " << left << ", Right: " << right << ", Top: " << top << ", Bottom: " << bottom << std::endl;
    
    
    // Draw only the tiles that are inside the camera
    for (size_t i = 0; i < m_vertices.getVertexCount(); i += 6) {  // Avanza de 6 en 6, ya que cada tile tiene 6 vértices

        bool tileVisible = false;

        // Obtain the tile index
        size_t tileIndex = i / 6;

        size_t row = tileIndex / m_tilesPerRow;

        size_t col = tileIndex % m_tilesPerRow;

        //std::cout << "index: " << i << ", Row: " << row << ", Col: " << col << std::endl;
        // Obtain the tile reference
        SolidTileAttributes& tile = m_solidTiles[row][col];
        
        // Going through each of the 6 vertices of the tile
        for (size_t j = 0; j < 6; ++j) {
            if(j == 3 || j == 4) continue; // Only needs to check 4 vertices, 2 of them are repeated
            sf::Vertex& vertex = m_vertices[i + j];
            float x = vertex.position.x;
            float y = vertex.position.y;

            // Verifies if vertex is visible
            if (x >= left && x <= right && y >= top && y <= bottom) {
                tileVisible = true;
                break;  // No need to keep searching if 1 out of 6 vertices is visible
            }
        }

        // If tile is visible, show and make visible
        if (tileVisible) {
            window.draw(&m_vertices[i], 6, sf::PrimitiveType::Triangles, &m_tileset);
            tile.isVisible = true;
            
        } else {
            tile.isVisible = false;
        }
    }

    drawHitboxes(window);
}


sf::FloatRect TileMap::getMapBounds() const{
    return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(m_tileSize * m_tilesPerRow, m_tileSize * m_tilesPerColumn));
}




// ------------------------ Auxiliar functions ------------------------

sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect)
{
    if (floatRect.size.x == 0.0f || floatRect.size.y == 0.0f)
    {
        return sf::RectangleShape();
    }

    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    rectShape.setOutlineColor(sf::Color::Red);
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

void TileMap::processFile(const std::string& archivo, std::vector<int>& solidTileNumberList) {
    std::ifstream file(archivo);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error opening tilemap file." << std::endl;
        return;
    }

    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string numberStr;

        try {
            std::getline(ss, numberStr, ',');
            m_tilesPerRow = std::stoi(numberStr);
            std::cout << "TILEMAP WIDTH: " << m_tilesPerRow << std::endl;

            std::getline(ss, numberStr, ',');
            m_tilesPerColumn = std::stoi(numberStr);
            std::cout << "TILEMAP HEIGHT: " << m_tilesPerColumn << std::endl;

            if (m_tilesPerRow <= 0 || m_tilesPerColumn <= 0) {
                std::cerr << "Invalid map dimensions: " << m_tilesPerRow << "x" << m_tilesPerColumn << std::endl;
                return;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing map dimensions: " << e.what() << std::endl;
            return;
        }
    } else {
        std::cerr << "Error: Empty or corrupted tilemap file." << std::endl;
        return;
    }


    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string numberStr;
        
        // Processes the solid tiles
        while (std::getline(ss, numberStr, ',')) {
            try {
                int numero = std::stoi(numberStr); // Convertir de string a int
                solidTileNumberList.push_back(numero); // Añadir el número a la lista
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid solidTile number in tilemap file: " << numberStr << std::endl;
            }
        }
    }

    file.close();
}
