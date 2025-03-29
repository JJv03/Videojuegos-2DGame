#include "tilemap.h"
#include <iostream>
#include <fstream>

sf::Vector2f TileMap::getPositionForOffset(const int x, const int y, const int x_offset, const int y_offset) const {
    return sf::Vector2f(x * this->m_tileSize + x_offset, y * this->m_tileSize + y_offset);
}

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

sf::FloatRect TileMap::getHitboxForBreakableTile(const int id) const
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

    m_tileset.setSmooth(false);

    std::vector<int> tilemap;
    leerNumeros(tilemap_path, tilemap);
    
    solidTiles.resize(height);
    for (auto& row : solidTiles) {
        row.resize(width);
    }

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(width * height * 6);

    if (!m_tileset.loadFromFile(tileset_path)) {
        return false;
    }

    // Luego se hace bien
    sf::Image hoguera;
    if (!hoguera.loadFromFile("../../assets/sprites/items/itemsObjects.png")) {
        return false;
    }

    hoguera.createMaskFromColor(sf::Color(0x74, 0x74, 0x74)); // color key
    m_hoguera = sf::Texture(hoguera, false, sf::IntRect({175, 2}, {16, 31}));
    sf::Sprite s_hoguera(m_hoguera);

    // Primera hoguera
    s_hoguera.setPosition(getPositionForOffset(2, 3, 31, 33));
    m_specialTiles.push_back({s_hoguera, true, false, SpecialTile::Type::Bonfire});

    // Segunda hoguera
    s_hoguera.setPosition(getPositionForOffset(6, 3, 25, 33));
    m_specialTiles.push_back({s_hoguera, true, false, SpecialTile::Type::Bonfire});

    // Tercera hoguera
    s_hoguera.setPosition(getPositionForOffset(10, 3, 31, 33));
    m_specialTiles.push_back({s_hoguera, true, false, SpecialTile::Type::Bonfire});

    // Cuarta hoguera
    s_hoguera.setPosition(getPositionForOffset(14, 3, 31, 33));
    m_specialTiles.push_back({s_hoguera, true, false, SpecialTile::Type::Bonfire});

    // Quinta hoguera
    s_hoguera.setPosition(getPositionForOffset(18, 3, 31, 33));
    m_specialTiles.push_back({s_hoguera, true, false, SpecialTile::Type::Bonfire});


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
            solidTiles[j][i].hitbox = getHitboxForSolidTile(tileNumber);
            solidTiles[j][i].position = sf::Vector2f(i * m_tileSize, j * m_tileSize);
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


void TileMap::drawScene(sf::RenderWindow& window, const Camera& camera){
    sf::View view = camera.GetView(window.getSize());

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
        
        // Going through each of the 6 vertices of the tile
        for (size_t j = 0; j < 6; ++j) {
            if(j == 3 || j == 4) continue; // Only needs to check 4 vertices, 2 of them are repeated
            sf::Vertex& vertex = m_vertices[i + j];
            float x = vertex.position.x;
            float y = vertex.position.y;

            // Verifica si el vértice está dentro de la vista
            if (x >= left && x <= right && y >= top && y <= bottom) {
                tileVisible = true;
                break;  // No necesitamos seguir comprobando los otros vértices si ya encontramos uno dentro de la vista
            }
        }

        // Si el tile está visible, dibújalo
        if (tileVisible) {
            // Establecer la textura y dibujar el tile
            window.draw(&m_vertices[i], 6, sf::PrimitiveType::Triangles, &m_tileset);
        }
    }

    for (TileMap::SpecialTile& hoguera : m_specialTiles) {
        // Obtener la posición de la hoguera
        sf::Vector2f hogueraPos = hoguera.sprite.getPosition();

        // Comprobar si la hoguera está dentro de la vista de la cámara
        if (hogueraPos.x >= left && hogueraPos.x <= right &&
            hogueraPos.y >= top && hogueraPos.y <= bottom) {
            // Si está dentro, dibujamos la hoguera
            window.draw(hoguera.sprite);
        }
    }
    
}




// ------------------------ Auxiliar functions ------------------------

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
