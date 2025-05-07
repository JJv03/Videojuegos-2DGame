#include "tilemap.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>

using BossPos = TileMap::BossPosition;

// Collision Types (only used here)
enum CollisionType
{
    NO_COLLISION,
    FULL_COLLISION,
    BOTTOM_HALF_COLLISION,
    TOP_HALF_COLLISION,
    LEFT_HALF_COLLISION,
    RIGHT_HALF_COLLISION,
    THIN_TOP_LEFT_COLLISION,
    TOP_LEFT_COLLISION,
    TOP_RIGHT_COLLISION,
    BOTTOM_LEFT_COLLISION,
    BOTTOM_RIGHT_COLLISION,
    TWO_VERTICAL_COLLISION,
    TWO_VERTICAL_RIGHT_COLLISION,
    THREE_VERTICAL_COLLISION,
};

std::unordered_map<CollisionType, sf::FloatRect> collisionTypes = {
    {NO_COLLISION, sf::FloatRect({0.0f, 0.0f}, {0.0f, 0.0f})},
    {FULL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize))},
    {BOTTOM_HALF_COLLISION, sf::FloatRect({0.0f, gTileSize / 2.0f}, sf::Vector2f(gTileSize, gTileSize / 2.0f))},
    {TOP_HALF_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize / 2.0f))},
    {LEFT_HALF_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize / 2.0f, gTileSize))},
    {RIGHT_HALF_COLLISION, sf::FloatRect({gTileSize / 2.0f, 0.0f}, sf::Vector2f(gTileSize / 2.0f, gTileSize))},
    {THIN_TOP_LEFT_COLLISION, sf::FloatRect({0.0f, 0.0f}, {gTileSize / 4.0f, gTileSize / 2.0f})},
    {TOP_LEFT_COLLISION, sf::FloatRect({0.0f, 0.0f}, {gTileSize / 2.0f, gTileSize / 2.0f})},
    {TOP_RIGHT_COLLISION, sf::FloatRect({gTileSize / 2.0f, 0.0f}, {gTileSize / 2.0f, gTileSize / 2.0f})},
    {BOTTOM_LEFT_COLLISION, sf::FloatRect({0.0f, gTileSize / 2.0f}, {gTileSize / 2.0f, gTileSize / 2.0f})},
    {BOTTOM_RIGHT_COLLISION, sf::FloatRect({gTileSize / 2.0f, gTileSize / 2.0f}, {gTileSize / 2.0f, gTileSize / 2.0f})},
    {TWO_VERTICAL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize * 2.f))},
    {TWO_VERTICAL_RIGHT_COLLISION, sf::FloatRect({gTileSize / 2.0f, 0.0f}, sf::Vector2f(gTileSize / 2.f, gTileSize * 2.f))},
    {THREE_VERTICAL_COLLISION, sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(gTileSize, gTileSize * 3.f))},
};

// ======================================================================================

TileMap::TileMap()
{
}

TileMap::~TileMap()
{
}

sf::FloatRect TileMap::getHitboxForSolidTile(const int level, const int id) const
{
    switch (level)
    {
    case 1:
        switch (id)
        {
        case 76:
            return collisionTypes.at(FULL_COLLISION);

        case 14:
        case 34:
        case 37: // Stage 1-1 outside floor
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
        case 62: // Bottom of door
        case 66:
        case 70:
        case 72:
        case 83:
            return collisionTypes.at(BOTTOM_HALF_COLLISION);
        case 2:
        //case 4:  // Escalera (quitar)
        //case 61: // Escalera (quitar)
            return collisionTypes.at(BOTTOM_RIGHT_COLLISION);
        case 6:
        case 23:
        case 80:
        case 82:
        //case 46: // Escalera (quitar)
        //case 71: // Escalera (quitar)
            return collisionTypes.at(BOTTOM_LEFT_COLLISION);
        case 81:
            return collisionTypes.at(TOP_LEFT_COLLISION);
        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 2:
        switch (id)
        {

        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 3:
        switch (id)
        {
        case 5:
        case 9:
        case 10:
        case 15:
            return collisionTypes.at(BOTTOM_HALF_COLLISION);
        case 30:
        case 36:
        case 45:
        case 47:
            return collisionTypes.at(TOP_HALF_COLLISION);
        case 16:
            return collisionTypes.at(FULL_COLLISION);
        case 29:
        case 41:
            return collisionTypes.at(TOP_RIGHT_COLLISION);
        case 33:
        case 35:    
            return collisionTypes.at(TOP_LEFT_COLLISION);
        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 4:
        switch (id)
        {

        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 5:
        switch (id)
        {
        case 2:
            return collisionTypes.at(TOP_RIGHT_COLLISION);
        case 8:
        case 50:
            return collisionTypes.at(TOP_LEFT_COLLISION);
        case 48:
            return collisionTypes.at(RIGHT_HALF_COLLISION);
        case 49:
            return collisionTypes.at(BOTTOM_HALF_COLLISION);
        case 14:
            return collisionTypes.at(FULL_COLLISION);
        case 15:
        case 26:
        case 27:
        case 30:
        case 32:
            return collisionTypes.at(TOP_HALF_COLLISION);
        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 6:
        switch (id)
        {

        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    case 7:
        switch (id)
        {
        case 6:
        case 28:
            return collisionTypes.at(FULL_COLLISION);

        default:
            return collisionTypes.at(NO_COLLISION);
        }
        break;

    default:
        return collisionTypes.at(NO_COLLISION);
    }
}

sf::FloatRect TileMap::getHitboxForMiscTile(const int id) const
{
    switch (id)
    {
    case 0: // Firepit
        return collisionTypes.at(LEFT_HALF_COLLISION);
    case 1: // Candelabrum
        return collisionTypes.at(THIN_TOP_LEFT_COLLISION);
    case 2: // Lvl 1 - Wall 1 square
    case 3: // Lvl 1 - Wall 4 square
    case 4: // Lvl 1 - Wall 3 square
    case 6: // Lvl 1 - Wall 1 square cracked
    case 7: // Lvl 1 - Wall 4 square cracked
    case 8: // Lvl 1 - Wall 3 square cracked
    case 9: // Lvl 3 - Wall 1 square
    case 10: // Lvl 5 - Wall 1 square
    case 11: // Lvl 3 - Wall 1 square cracked
    case 12: // Lvl 5 - Wall 1 square cracked
    case 99: // Invisible wall
        return collisionTypes.at(TOP_LEFT_COLLISION);   // Any 16x16 would do
    case 5: // Wall DROP_TRIGGER
        return collisionTypes.at(FULL_COLLISION);
    default:
        return collisionTypes.at(NO_COLLISION);
    }
}

sf::FloatRect TileMap::getHitboxForDoorTile(const int id) const
{
    try
    {
        switch (id)
        {
        case 0: // Castle Entrance
            return collisionTypes.at(THREE_VERTICAL_COLLISION);

        case 1: // Door
            return collisionTypes.at(TWO_VERTICAL_RIGHT_COLLISION);

        case 2: // Stairs
            return collisionTypes.at(FULL_COLLISION);

        default:
            return collisionTypes.at(NO_COLLISION);
        }
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: Key not found in collisionTypes while finding doorTile hitbox: " << id << std::endl;
        return sf::FloatRect();
    }
}


bool TileMap::load(int level, int stage)
{
    std::string tileset_path = "assets/tilesets/tileset_" + std::to_string(level) + ".png";
    std::string tilemap_path = "assets/tilemaps/level" + std::to_string(level) + "/tilemap_" +
                               std::to_string(level) + "_" + std::to_string(stage) + ".txt";

    if (!loadMiscTextures())
    {
        std::cerr << "Error loading miscellaneous tile textures" << std::endl;
        return false;
    }

    std::vector<int> tilemap;
    processFile(tilemap_path, tilemap);

    m_solidTiles.resize(m_tilesPerColumn);
    for (auto &row : m_solidTiles)
    {
        row.resize(m_tilesPerRow);
    }

    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(m_tilesPerColumn * m_tilesPerRow * 6);

    if (!m_tileset.loadFromFile(tileset_path))
    {
        return false;
    }

    m_tileset.setSmooth(false);

    int tilesPerRow = (m_tileset.getSize().x) / (static_cast<int>(gTileSize) + 4); // 2 pixel margin between tiles (on each side)

    for (int i = 0; i < m_tilesPerRow; ++i)
    {
        for (int j = 0; j < m_tilesPerColumn; ++j)
        {
            int tileIndex = i + j * m_tilesPerRow;
            int tileNumber = tilemap[tileIndex];

            // Finds position in tileset
            int tileset_row = tileNumber % tilesPerRow;
            int tileset_column = tileNumber / tilesPerRow;

            // Calculate tilemap coordinates (+1 for pixel margin between tiles)
            int texX = (tileset_row * (static_cast<int>(gTileSize) + 4)) + 2; // 1px of margin between tiles
            int texY = (tileset_column * (static_cast<int>(gTileSize) + 4)) + 2;

            // Points to the first vertex of the tile
            sf::Vertex *triangle = &m_vertices[tileIndex * 6];

            // Assigns positions
            triangle[0].position = sf::Vector2f(i * gTileSize, j * gTileSize);
            triangle[1].position = sf::Vector2f((i + 1) * gTileSize, j * gTileSize);
            triangle[2].position = sf::Vector2f(i * gTileSize, (j + 1) * gTileSize);
            triangle[3].position = sf::Vector2f(i * gTileSize, (j + 1) * gTileSize);
            triangle[4].position = sf::Vector2f((i + 1) * gTileSize, j * gTileSize);
            triangle[5].position = sf::Vector2f((i + 1) * gTileSize, (j + 1) * gTileSize);

            // Assigns texture coordinates
            triangle[0].texCoords = sf::Vector2f(texX, texY);
            triangle[1].texCoords = sf::Vector2f(texX + gTileSize, texY);
            triangle[2].texCoords = sf::Vector2f(texX, texY + gTileSize);
            triangle[3].texCoords = sf::Vector2f(texX, texY + gTileSize);
            triangle[4].texCoords = sf::Vector2f(texX + gTileSize, texY);
            triangle[5].texCoords = sf::Vector2f(texX + gTileSize, texY + gTileSize);

            // Set the hitbox for the solid tile
            sf::FloatRect newHitbox = getHitboxForSolidTile(level, tileNumber);
            newHitbox.position.x += i * gTileSize;
            newHitbox.position.y += j * gTileSize;

            // Horizontal hitbox merge
            if (i > 0)
            {
                for (auto &leftHitbox : m_solidTiles[j][i - 1].hitboxes)
                {
                    if (leftHitbox.position.y == newHitbox.position.y && leftHitbox.size.y == newHitbox.size.y &&
                        leftHitbox.position.x + leftHitbox.size.x == newHitbox.position.x)
                    {
                        // Expand hitbox to the right
                        newHitbox.position.x = leftHitbox.position.x;
                        newHitbox.size.x += leftHitbox.size.x;

                        // Left tile is now "empty" (the right-most tile will store the hitbox now)
                        leftHitbox = sf::FloatRect();

                        break;
                    }
                }
                
            }

            m_solidTiles[j][i].hitboxes.push_back(newHitbox);

            // Manage stairs
            StairTile::Type stairType;
            if(getStairType(level, tileNumber, stairType)){
                StairTile stairTile;
                stairTile.type = stairType;

                sf::FloatRect stairHitbox;
                stairHitbox.size.x = 16.f;
                stairHitbox.size.y = 30.f;

                stairHitbox.position.x += i * gTileSize + 8.f;
                stairHitbox.position.y += j * gTileSize - 16.f;

                stairTile.hitboxes.push_back(stairHitbox);
                m_stairTiles.push_back(stairTile);
            }
        }
    }

    // Vertical hitbox merge (Post-process)
    for (int j = m_tilesPerColumn - 2; j >= 0; --j) // From second-to-last row, up-wards
    {
        for (int i = 0; i < m_tilesPerRow; ++i)
        {
            for (auto &currentHitbox : m_solidTiles[j][i].hitboxes)
            {
                if (currentHitbox.size.x == 0 || currentHitbox.size.y == 0)
                    continue;

                for (auto &belowHitbox : m_solidTiles[j + 1][i].hitboxes)
                {
                    if (belowHitbox.size.x == 0 || belowHitbox.size.y == 0)
                        continue;

                    if (belowHitbox.position.x == currentHitbox.position.x &&
                        belowHitbox.size.x == currentHitbox.size.x &&
                        belowHitbox.position.y == currentHitbox.position.y + currentHitbox.size.y)
                    {
                        // Merge hitbox with the one below
                        currentHitbox.size.y += belowHitbox.size.y;

                        // Below-tile is now "empty" (the upper tile will store the hitbox now)
                        belowHitbox = sf::FloatRect();
                        break;
                    }
                }
            }
        }
    }

    
    // std::vector<Entity*> temporal_solidTiles;
    // for (int j = 0; j < m_tilesPerColumn; ++j) {
    //     for (int i = 0; i < m_tilesPerRow; ++i) {
    //         if (m_solidTiles[j][i].hitboxes.size() > 0 && m_solidTiles[j][i].hitboxes[0].size.x != 0.f && m_solidTiles[j][i].hitboxes[0].size.y != 0.f) {
    //             temporal_solidTiles.push_back(&m_solidTiles[j][i]);
    //         }
    //     }
    // }
    // solidTileBVH.build(temporal_solidTiles);

    return true;
}

void TileMap::updateItems(const float& deltaTime) {
    for (auto it = m_items.begin(); it != m_items.end(); ) {
        (*it)->update(deltaTime);

        if ((*it)->m_lifeTime == 0.f){
            it = m_items.erase(it);     // erase and move iterator
        } else {
            ++it;   // just move iterator
        }
    }
}

void TileMap::updateMiscTiles(const float& delfaTime) {
    for (auto tile : m_miscTiles) {
        if (!tile->isDestroyed) {
            tile->update(delfaTime);
        }
    }
}

void TileMap::drawHitboxes(sf::RenderWindow &window) const
{
    // Solid tiles
    for (size_t i = 0; i < this->m_solidTiles.size(); ++i)
    {
        for (size_t j = 0; j < this->m_solidTiles[i].size(); ++j)
        {
            for (auto hitbox : this->m_solidTiles[i][j].hitboxes)
            {
                sf::RectangleShape rect = FloatRectToRectShape(hitbox);
                window.draw(rect);
            }
        }
    }


    // Doors
    for (auto &doorEntry : this->m_doorTiles)
    {
        sf::RectangleShape rect = FloatRectToRectShape(doorEntry.hitboxes[0], 1);
        window.draw(rect);
    }

    // Stairs
    for (auto &stairTiles : this->m_stairTiles)
    {
        sf::RectangleShape rect = FloatRectToRectShape(stairTiles.hitboxes[0], 2);
        window.draw(rect);
    }

    // Items
    for (auto &item : this->m_items)
    {
        //sf::RectangleShape rect = FloatRectToRectShape(item->sprite->getGlobalBounds());
        sf::RectangleShape rect = FloatRectToRectShape(item->hitboxes[0]);
        window.draw(rect);
    }

    // Misc tiles
    for (size_t i = 0; i < this->m_miscTiles.size(); ++i)
    {
        if (this->m_miscTiles[i]->isDestroyed)
        {
            continue;
        }

        for (auto hitbox : this->m_miscTiles[i]->hitboxes)
        {
            sf::RectangleShape rect = FloatRectToRectShape(hitbox);
            window.draw(rect);
        }
    }
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
}

void TileMap::drawScene(sf::RenderWindow &window, Camera &camera)
{
    VisibleTileRange visibleTiles = calculateVisibleTileRange(window, m_tilesPerRow, m_tilesPerColumn);

    for (int row = visibleTiles.firstRow; row <= visibleTiles.lastRow; ++row)
    {
        for (int col = visibleTiles.firstCol; col <= visibleTiles.lastCol; ++col)
        {
            size_t tileIndex = row * m_tilesPerRow + col;
            size_t vertexIndex = tileIndex * 6; // Every tile has 6 vertices

            window.draw(&m_vertices[vertexIndex], 6, sf::PrimitiveType::Triangles, &m_tileset);
        }
    }

    for (size_t i = 0; i < m_miscTiles.size(); ++i)
    {
        if (m_miscTiles[i]->isDestroyed) continue;
        window.draw(*m_miscTiles[i]->sprite);
    }

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        window.draw(*m_items[i]->sprite);
    }

    if(gDrawHitboxes) drawHitboxes(window);
}

sf::FloatRect TileMap::getMapBounds() const
{
    return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(gTileSize * m_tilesPerRow, gTileSize * m_tilesPerColumn));
}

sf::FloatRect TileMap::getMapBoundsBossFight() const
{
    if(bossPosition == BossPos::BOSS_LEFT){
        return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(gTileSize * 8.f, gTileSize * m_tilesPerColumn));
    } else {
        return sf::FloatRect(sf::Vector2f(gTileSize  * (m_tilesPerRow - 8.f), 0.f), sf::Vector2f(gTileSize * 8.f, gTileSize * m_tilesPerColumn));
    }
}

// ------------------------ Auxiliar functions ------------------------

sf::RectangleShape FloatRectToRectShape(const sf::FloatRect &floatRect, int color)
{
    if (floatRect.size.x == 0.0f || floatRect.size.y == 0.0f)
    {
        return sf::RectangleShape();
    }

    sf::RectangleShape rectShape(floatRect.size);
    rectShape.setPosition(floatRect.position);

    rectShape.setFillColor(sf::Color::Transparent);

    switch (color)
    {
    case 1:
        rectShape.setOutlineColor(sf::Color::Yellow);
        break;
    case 2:
        rectShape.setOutlineColor(sf::Color::Blue);
        break;

    default:
        rectShape.setOutlineColor(sf::Color::Red);
        break;
    }
    rectShape.setOutlineThickness(2.f);

    return rectShape;
}

void TileMap::processFile(const std::string &file_path, std::vector<int> &solidTileNumberList)
{
    std::ifstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Error opening tilemap file." << std::endl;
        return;
    }

    processFileMapDimensions(file);

    processFileInitialPosition(file);

    processFileBossSettings(file);

    processFileSolidTiles(file, solidTileNumberList);

    processFileDoorTiles(file);

    processFileMiscTiles(file);

    processFileStairTiles(file);

    processFileEnemies(file);

    file.close();
}

void TileMap::processFileMapDimensions(std::ifstream &file)
{
    std::string line;

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string numberStr;

        try
        {
            std::getline(ss, numberStr, ',');
            m_tilesPerRow = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            m_tilesPerColumn = std::stoi(numberStr);

            if (m_tilesPerRow <= 0 || m_tilesPerColumn <= 0)
            {
                std::cerr << "Invalid map dimensions: " << m_tilesPerRow << "x" << m_tilesPerColumn << std::endl;
                return;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing map dimensions: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "Error: Empty or corrupted tilemap file." << std::endl;
        return;
    }
}

void TileMap::processFileInitialPosition(std::ifstream &file)
{
    std::string line;

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string numberStr;

        try
        {
            std::getline(ss, numberStr, ',');
            float posX = std::stof(numberStr);

            std::getline(ss, numberStr, ',');
            float posY = std::stof(numberStr);

            if (posX <= 0.f || posY <= 0.f)
            {
                std::cerr << "Invalid initial position: " << posX << "," << posY << std::endl;
                return;
            }

            initialPosition = sf::Vector2f(posX, posY);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing initial position: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "Error: Empty or corrupted tilemap file." << std::endl;
        return;
    }
}

void TileMap::processFileBossSettings(std::ifstream &file)
{
    std::string line;

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string numberStr;

        try
        {
            std::getline(ss, numberStr, ',');
            int bossPositionInt = std::stoi(numberStr);

            if (bossPositionInt < 0 || bossPositionInt > 2)
            {
                std::cerr << "Invalid boss position type (0 <= n <= 2): " << bossPositionInt << std::endl;
                return;
            }

            bossPosition = static_cast<BossPos>(bossPositionInt);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing initial position: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "Error: Empty or corrupted tilemap file." << std::endl;
        return;
    }
}

void TileMap::processFileSolidTiles(std::ifstream &file, std::vector<int> &solidTileNumberList)
{
    std::string line;

    while (std::getline(file, line))
    {
        if (line == "doors")
            return;

        std::stringstream ss(line);
        std::string numberStr;

        while (std::getline(ss, numberStr, ','))
        {
            try
            {
                int number = std::stoi(numberStr);
                solidTileNumberList.push_back(number);
            }
            catch (const std::invalid_argument &)
            {
                std::cerr << "Invalid solidTile format in tilemap file: " << numberStr << std::endl;
            }
        }
    }
}

void TileMap::processFileDoorTiles(std::ifstream &file)
{
    std::string line;

    while (std::getline(file, line))
    {
        if (line == "end_doors")
            return;

        std::stringstream ss(line);
        std::string numberStr;
        try
        {
            std::getline(ss, numberStr, ',');
            int doorId = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            int doorType = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            int posX = std::stoi(numberStr);

            std::getline(ss, numberStr, ',');
            int posY = std::stoi(numberStr);

            sf::Vector2f aparition;

            if (doorType == 2)
            { // if stairs
                std::getline(ss, numberStr, ',');
                int playerPosX = std::stoi(numberStr);

                std::getline(ss, numberStr, ',');
                int playerPosY = std::stoi(numberStr);

                aparition = sf::Vector2f(playerPosX, playerPosY);
            }
            else
            {
                aparition = initialPosition;
            }

            sf::FloatRect hitbox = getHitboxForDoorTile(doorType);

            hitbox.position.x += posX;
            hitbox.position.y += posY;

            DoorTile door;
            door.hitboxes.push_back(hitbox);
            door.type = static_cast<DoorTile::Type>(doorType);
            door.playerAparition = aparition;
            door.doorId = doorId;
            m_doorTiles.push_back(door);
        }
        catch (const std::invalid_argument &)
        {
            std::cerr << "Invalid doorTile number in tilemap file: " << numberStr << std::endl;
        }
        catch (const std::out_of_range &)
        {
            std::cerr << "Out of range doorTile number in tilemap file" << std::endl;
        }
    }
}

void TileMap::processFileMiscTiles(std::ifstream &file)
{
    std::string line;

    std::getline(file, line);
    if (line != "miscellaneous")
    {
        std::cerr << "[Error] Expected 'miscellaneous' but found: " << line << std::endl;
        return;
    }

    while (std::getline(file, line))
    { // Until we find "end_miscellaneous"
        if (line == "end_miscellaneous")
            break;
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string token;

        try
        {
            // Extraction of miscellaneous tile type (mandatory !!!)
            std::getline(ss, token, ',');
            int miscellaneousType = std::stoi(token);

            // Extraction of X position (mandatory !!!)
            std::getline(ss, token, ',');
            int posX = std::stoi(token);

            // Extraction of Y position (mandatory !!!)
            std::getline(ss, token, ',');
            int posY = std::stoi(token);

            // Optional values
            // By default, it is BREAKABLE and there is NO ITEM TO DROP (= NONE).
            bool isBreakable = true;
            DropType dropType = DropType::NONE;
            int dropItem_x = -1;
            int dropItem_y = -1;

            if (std::getline(ss, token, ','))       // Misc: Optional value
            {                                          
                isBreakable = (std::stoi(token) != 0); // If it is not 0, it is true.
            }
            if (std::getline(ss, token, ','))       // Drop item: Optional value
            {
                // Expecting an integer representing the type of item that can be dropped
                dropType = static_cast<DropType>(std::stoi(token));
            }
            if (std::getline(ss, token, ','))       // Drop item x position: Optional value
            {                                          
                dropItem_x = std::stoi(token);
                
                if (std::getline(ss, token, ','))       // Drop item y position: Optional value
                {
                    dropItem_y = std::stoi(token);
                }
                else
                {
                    std::cerr << "[Error] Expected drop item y position but found: " << token << std::endl;
                }
            }

            sf::FloatRect hitbox = getHitboxForMiscTile(miscellaneousType);
            hitbox.position.x += posX;
            hitbox.position.y += posY;

            std::shared_ptr<MiscellaneousTile> tile = getMiscTile(static_cast<MiscTileType>(miscellaneousType),
                                                                    hitbox, isBreakable, dropType, dropItem_x, dropItem_y);
            m_miscTiles.push_back(tile);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error al procesar miscellaneous tile en la línea: " << line
                      << ". Excepción: " << e.what() << std::endl;
        }
    }
}

void TileMap::processFileStairTiles(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    if (line != "stair")
    {
        std::cerr << "[Error] Expected 'stair' but found: " << line << std::endl;
        return;
    }

    while (std::getline(file, line))
    { // Until we find "end_stair"
        if (line == "end_stair")
            break;
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string token;

        try
        {
            // Extraction of stair tile type (mandatory !!!)
            std::getline(ss, token, ',');
            int stairType = std::stoi(token);

            // Extraction of X position (mandatory !!!)
            std::getline(ss, token, ',');
            int posX = std::stoi(token);

            // Extraction of Y position (mandatory !!!)
            std::getline(ss, token, ',');
            int posY = std::stoi(token);

            sf::FloatRect hitbox;
            hitbox.size.x = 16.f;
            hitbox.size.y = 30.f;

            hitbox.position.x = posX;
            hitbox.position.y = posY;

            StairTile tile;
            tile.hitboxes.push_back(hitbox);
            tile.type = static_cast<StairType>(stairType);

            m_stairTiles.push_back(tile);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error al procesar stair tile en la línea: " << line
                      << ". Excepción: " << e.what() << std::endl;
        }
    }
}

void TileMap::processFileEnemies(std::ifstream &file)
{
    std::string line;

    // For security, omit the lines until the "enemies" mark
    // while (std::getline(file, line))
    // {
    //     if (line == "enemies")
    //         break;
    // }
    
    std::getline(file, line);
    if (line != "enemies")
    {
        std::cerr << "[Error] Expected 'enemies' but found: " << line << std::endl;
        return;
    }

    while (std::getline(file, line))
    { // Until we find "end_enemies"
        if (line == "end_enemies")
            break;
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string token;

        try
        {
            // Extraction of enemy type (mandatory)
            std::getline(ss, token, ',');
            int enemyType = std::stoi(token);

            // Extraction of X position (mandatory)
            std::getline(ss, token, ',');
            float posX = std::stof(token);

            // Extraction of Y position (mandatory)
            std::getline(ss, token, ',');
            float posY = std::stof(token);

            // Optional values: width and height for spawning area
            float width = 0.0f;
            float height = 0.0f;

            if (std::getline(ss, token, ','))
            { // Optional width
                width = std::stof(token);
            }

            if (std::getline(ss, token, ','))
            { // Optional height
                height = std::stof(token);
            }

            EnemyData enemy;
            enemy.type = enemyType;
            enemy.position = sf::Vector2f(posX, posY);
            enemy.width = width;
            enemy.height = height;

            m_enemyData.push_back(enemy);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing enemy in line: " << line << ". Exception: " << e.what() << std::endl;
        }
    }
}

