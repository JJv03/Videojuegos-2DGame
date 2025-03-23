#include "tilemap.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>


using BreakableType = TileMap::BreakableTile::Type;
using DropType = TileMap::BreakableTile::DropType;

// Collision Types (only used here)
enum CollisionType {
    NO_COLLISION,
    FULL_COLLISION,
    BOTTOM_HALF_COLLISION,
    TOP_HALF_COLLISION,
    BOTTOM_LEFT_COLLISION,
    BOTTOM_RIGHT_COLLISION,
    TWO_VERTICAL_COLLISION,
    THREE_VERTICAL_COLLISION,
};

std::unordered_map<CollisionType, sf::FloatRect> collisionTypes = {
    { NO_COLLISION, sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f}) },
    { FULL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize)) },
    { BOTTOM_HALF_COLLISION, sf::FloatRect({0.0f, gTileSize / 2.0f}, sf::Vector2f(gTileSize, gTileSize / 2.0f)) },
    { TOP_HALF_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize / 2.0f)) },
    { BOTTOM_LEFT_COLLISION, sf::FloatRect({0.0f, gTileSize / 2.0f}, {gTileSize / 2.0f, gTileSize / 2.0f}) },
    { BOTTOM_RIGHT_COLLISION, sf::FloatRect({gTileSize / 2.0f, gTileSize / 2.0f}, {gTileSize / 2.0f, gTileSize / 2.0f}) },
    { TWO_VERTICAL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize * 2.f)) },
    { THREE_VERTICAL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize * 3.f)) },
};

// Hash function for the breakable tile type
struct BreakableTypeHash {
    std::size_t operator()(const BreakableType& t) const {
        return std::hash<int>()(static_cast<int>(t));
    }
};

// Dicctionary with the textures of the breakable tiles
std::unordered_map<BreakableType, std::shared_ptr<sf::Texture>, BreakableTypeHash> breakableTextures;


// ======================================================================================


TileMap::TileMap() {
}

TileMap::~TileMap(){

}

sf::FloatRect TileMap::getHitboxForSolidTile(const int level, const int id) const
{
    switch(level){
        case 1:
            switch (id) {
                case 76:
                            return collisionTypes.at(FULL_COLLISION);

                case 14:
                case 34:
                case 37:    // Stage 1-1 outside floor
                case 53:
                            return collisionTypes.at(TOP_HALF_COLLISION);
                case 7:
                case 13:
                case 16:
                case 21:
                case 22:
                case 31:
                case 45:
                case 55:
                case 56:
                case 62:    // Bottom of door
                case 66:
                case 70:
                case 72:
                            return collisionTypes.at(BOTTOM_HALF_COLLISION);
                case 2:
                case 4:     // Escalera (quitar)
                case 61:    // Escalera (quitar)
                            return collisionTypes.at(BOTTOM_RIGHT_COLLISION);
                case 6:
                case 23:
                case 46:    // Escalera (quitar)
                case 71:    // Escalera (quitar)
                            return collisionTypes.at(BOTTOM_LEFT_COLLISION);
                default:    
                            return collisionTypes.at(NO_COLLISION);
            }
            break;


        case 2:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;
            

        case 3:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;


        case 4:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;


        case 5:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;
        

        case 6:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;


        case 7:
            switch (id) {
                
                default:    return collisionTypes.at(NO_COLLISION);
            }
            break;

            
        default:
            return collisionTypes.at(NO_COLLISION);
    }
    
}

sf::FloatRect TileMap::getHitboxForBreakableTile(const int id) const
{
    switch (id) {
        case 0:
            
        case 2:
            return collisionTypes.at(FULL_COLLISION);
        case 4:
            return collisionTypes.at(BOTTOM_HALF_COLLISION);
        default:
            return collisionTypes.at(NO_COLLISION);
    }
}

sf::FloatRect TileMap::getHitboxForDoorTile(const int id) const
{
    try{
        switch (id) {
            case 0: // Castle Entrance
                return collisionTypes.at(THREE_VERTICAL_COLLISION);

            case 1: // Door
                return collisionTypes.at(TWO_VERTICAL_COLLISION);

            case 2: // Stairs
                return collisionTypes.at(FULL_COLLISION);

            default:
                return collisionTypes.at(NO_COLLISION);
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Key not found in collisionTypes while finding doorTile hitbox: " << id << std::endl;
        return sf::FloatRect();
    }
}




bool TileMap::load(int level, int stage) {
    std::string tileset_path = "assets/tilesets/tileset_" + std::to_string(level) + ".png";
    std::string tilemap_path = "assets/tilemaps/level" + std::to_string(level) + "/tilemap_" +
                                std::to_string(level) + "_" + std::to_string(stage) + ".txt";
    
    std::vector<int> tilemap;
    processFile(tilemap_path, tilemap);

    m_solidTiles.resize(m_tilesPerColumn);
    for (auto& row : m_solidTiles) {
        row.resize(m_tilesPerRow);
    }

    if (!loadBreakableTextures()) {
        std::cerr << "Error loading breakable textures" << std::endl;
        return false;
    }

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(m_tilesPerColumn * m_tilesPerRow * 6);

    if (!m_tileset.loadFromFile(tileset_path)) {
        return false;
    }

    m_tileset.setSmooth(false);

    int tilesPerRow = (m_tileset.getSize().x + 1) / (gTileSize + 1); // Tiene en cuenta el pixel de margen entre tiles

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
            int texX = tileset_row * (gTileSize + 1); // Sumamos el 1px de separación
            int texY = tileset_column * (gTileSize + 1);

            // Ir al primer vértice del tile
            sf::Vertex* triangle = &m_vertices[tileIndex * 6];

            // Asignar las posiciones
            triangle[0].position = sf::Vector2f(i * gTileSize, j * gTileSize);
            triangle[1].position = sf::Vector2f((i + 1) * gTileSize, j * gTileSize);
            triangle[2].position = sf::Vector2f(i * gTileSize, (j + 1) * gTileSize);
            triangle[3].position = sf::Vector2f(i * gTileSize, (j + 1) * gTileSize);
            triangle[4].position = sf::Vector2f((i + 1) * gTileSize, j * gTileSize);
            triangle[5].position = sf::Vector2f((i + 1) * gTileSize, (j + 1) * gTileSize);

            // Asignar las coordenadas
            triangle[0].texCoords = sf::Vector2f(texX, texY);
            triangle[1].texCoords = sf::Vector2f(texX + gTileSize, texY);
            triangle[2].texCoords = sf::Vector2f(texX, texY + gTileSize);
            triangle[3].texCoords = sf::Vector2f(texX, texY + gTileSize);
            triangle[4].texCoords = sf::Vector2f(texX + gTileSize, texY);
            triangle[5].texCoords = sf::Vector2f(texX + gTileSize, texY + gTileSize);

            // Set the hitbox for the solid tile
            m_solidTiles[j][i].hitbox = getHitboxForSolidTile(level, tileNumber);

            m_solidTiles[j][i].hitbox.position.x += i * gTileSize;
            m_solidTiles[j][i].hitbox.position.y += j * gTileSize;
        }
    }

    return true;
}

bool TileMap::loadBreakableTextures() {         
    sf::Image firepitAndCandelabrum;
    if (!firepitAndCandelabrum.loadFromFile("./assets/sprites/items/itemsObjects.png")) return false;
    firepitAndCandelabrum.createMaskFromColor(sf::Color(0x74, 0x74, 0x74));

    auto firepitTexture = std::make_shared<sf::Texture>(firepitAndCandelabrum, false, sf::IntRect({175, 2}, {16, 31}));
    breakableTextures[BreakableType::FIREPIT] = firepitTexture;
    
    auto candelabrumTexture = std::make_shared<sf::Texture>(firepitAndCandelabrum, false, sf::IntRect({157, 1}, {8, 16}));
    breakableTextures[BreakableType::CANDELABRUM] = candelabrumTexture;
    
    return true;
}


void TileMap::drawHitboxes(sf::RenderWindow& window) const
{
    // Solid tiles
    for (size_t i = 0; i < this->m_solidTiles.size(); ++i) {
        for (size_t j = 0; j < this->m_solidTiles[i].size(); ++j) {
            sf::RectangleShape rect = FloatRectToRectShape(this->m_solidTiles[i][j].hitbox);
            window.draw(rect);
        }
    }

    // Doors
    for (auto& doorEntry : this->m_doorTiles) {
        sf::RectangleShape rect = FloatRectToRectShape(doorEntry.second.hitbox, 1);
        window.draw(rect);
    }

    // Breakable tiles
    for (size_t i = 0; i < this->m_breakableTiles.size(); ++i) {
        if (this->m_breakableTiles[i].isDestroyed) {
            continue;
        }

        sf::RectangleShape rect = FloatRectToRectShape(this->m_breakableTiles[i].hitbox);
        window.draw(rect);
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
        SolidTile& tile = m_solidTiles[row][col];
        
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

    for (size_t i = 0; i < m_breakableTiles.size(); ++i) {
        if (m_breakableTiles[i].isDestroyed) {
            continue;
        }

        sf::Sprite sprite(*breakableTextures[BreakableType::FIREPIT]);
        sprite.setPosition(m_breakableTiles[i].hitbox.position);
        window.draw(sprite);
    }

    drawHitboxes(window);
}


sf::FloatRect TileMap::getMapBounds() const{
    return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(gTileSize * m_tilesPerRow, gTileSize * m_tilesPerColumn));
}




// ------------------------ Auxiliar functions ------------------------

sf::RectangleShape FloatRectToRectShape(const sf::FloatRect& floatRect, int color)
{
    if (floatRect.size.x == 0.0f || floatRect.size.y == 0.0f)
    {
        return sf::RectangleShape();
    }

    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);
    
    switch(color){
        case 1:
            rectShape.setOutlineColor(sf::Color::Yellow);
            break;

        default:
            rectShape.setOutlineColor(sf::Color::Red);
            break;
    }
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

void TileMap::processFile(const std::string& file_path, std::vector<int>& solidTileNumberList) {
    std::ifstream file(file_path);
    
    if (!file.is_open()) {
        std::cerr << "Error opening tilemap file." << std::endl;
        return;
    }

    processFileMapDimensions(file);

    processFileInitialPosition(file);

    processFileSolidTiles(file, solidTileNumberList);

    processFileDoorTiles(file);

    processFileBreakableTiles(file);

    file.close();
}



void TileMap::processFileMapDimensions(std::ifstream& file){
    std::string line;

    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string numberStr;

        try {
            std::getline(ss, numberStr, ',');
            m_tilesPerRow = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            m_tilesPerColumn = std::stoi(numberStr);

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
}


void TileMap::processFileInitialPosition(std::ifstream& file){
    std::string line;

    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string numberStr;

        try {
            std::getline(ss, numberStr, ',');
            int posX = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            int posY = std::stoi(numberStr);

            if (posX <= 0 || posY <= 0) {
                std::cerr << "Invalid initial position: " << posX << "," << posY << std::endl;
                return;
            }

            initialPosition = sf::Vector2f(posX, posY);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing initial position: " << e.what() << std::endl;
            return;
        }
    } else {
        std::cerr << "Error: Empty or corrupted tilemap file." << std::endl;
        return;
    }
}


void TileMap::processFileSolidTiles(std::ifstream& file, std::vector<int>& solidTileNumberList){
    std::string line;

    while (std::getline(file, line)) {
        if(line == "doors") return;

        std::stringstream ss(line);
        std::string numberStr;
        
        while (std::getline(ss, numberStr, ',')) {
            try {
                int number = std::stoi(numberStr);
                solidTileNumberList.push_back(number);
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid solidTile format in tilemap file: " << numberStr << std::endl;
            }
        }
    }
}

void TileMap::processFileDoorTiles(std::ifstream& file){
    std::string line;

    while (std::getline(file, line)) {
        if(line == "end_doors") return;

        std::stringstream ss(line);
        std::string numberStr;
        try {
            std::getline(ss, numberStr, ',');
            int doorId = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            int doorType = std::stoi(numberStr);
        
            std::getline(ss, numberStr, ',');
            int posX = std::stoi(numberStr);
        
            std::getline(ss, numberStr, ',');
            int posY = std::stoi(numberStr);

            sf::Vector2f aparition;

            if(doorType == 2){ // if stairs
                std::getline(ss, numberStr, ',');
                int playerPosX = std::stoi(numberStr);
            
                std::getline(ss, numberStr, ',');
                int playerPosY = std::stoi(numberStr);
                std::cout << "Stairs: " << playerPosX << ", " << playerPosY << std::endl;

                aparition = sf::Vector2f(playerPosX, playerPosY);
            } else {
                aparition = initialPosition;
            }

            sf::FloatRect hitbox = getHitboxForDoorTile(doorType);

            hitbox.position.x += posX;
            hitbox.position.y += posY;

            DoorTile door = {hitbox, static_cast<DoorTile::Type>(doorType), aparition};
            
            m_doorTiles[doorId] = door;

        } catch (const std::invalid_argument&) {
            std::cerr << "Invalid doorTile number in tilemap file: " << numberStr << std::endl;
        } catch (const std::out_of_range&){
            std::cerr << "Out of range doorTile number in tilemap file" << std::endl;
        }
    }
}

void TileMap::processFileBreakableTiles(std::ifstream& file) {
    std::string line;
    
    // For security, omit the lines until the "breakable" mark
    while (std::getline(file, line)) {
        if (line == "breakable")
            break;
    }
    
    while (std::getline(file, line)) {      // Until we find "end_breakable"
        if (line == "end_breakable")
            break;
        if (line.empty())
            continue;
        
        std::stringstream ss(line);
        std::string token;
        
        try {
            // Extraction of breakable tile type (mandatory !!!)
            std::getline(ss, token, ',');
            int breakableType = std::stoi(token);
            
            // Extraction of X position (mandatory !!!)
            std::getline(ss, token, ',');
            int posX = std::stoi(token);
            
            // Extraction of Y position (mandatory !!!)
            std::getline(ss, token, ',');
            int posY = std::stoi(token);

            // Optional values: isBreakable and dropItem.
            // By default, it is BREAKABLE and there is NO ITEM TO DROP (= NONE).
            bool isBreakable = true;
            BreakableTile::DropType dropItem = BreakableTile::DropType::NONE;
            
            if (std::getline(ss, token, ',')) {     // Optional value
                isBreakable = (std::stoi(token) != 0);      // If it is not 0, it is true.
            }
            if (std::getline(ss, token, ',')) {     // Optional value
                // Expecting an integer representing the type of item that can be dropped
                dropItem = static_cast<BreakableTile::DropType>(std::stoi(token));
            }
             
            //sf::Vector2u textureSize = breakableTextures[static_cast<BreakableType>(breakableType)]->getSize();
            //sf::FloatRect hitbox({posX, posY}, {textureSize.x, textureSize.y});
            sf::FloatRect hitbox = getHitboxForBreakableTile(breakableType);
            hitbox.position.x += posX;
            hitbox.position.y += posY;
            
            BreakableTile tile;
            tile.hitbox = hitbox;
            tile.type = static_cast<BreakableTile::Type>(breakableType);
            tile.isBreakable = isBreakable;
            tile.dropItem = dropItem;
            
            m_breakableTiles.push_back(tile);
        } catch (const std::exception& e) {
            std::cerr << "Error al procesar breakable tile en la línea: " << line
                      << ". Excepción: " << e.what() << std::endl;
        }
    }
}

