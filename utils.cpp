#include <iostream>
#include <cmath>
#include "utils.h"
#include "globals.h"


sf::Vector2f getVirtualUpperLeftCornerCoordOfGameView(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f gameViewPosition = window.getView().getViewport().position;
    sf::Vector2i windowPixelCoordOfUpperLeftCornerOfGameView(windowSize.x * gameViewPosition.x,
                                                             windowSize.y * gameViewPosition.y);
    return window.mapPixelToCoords(windowPixelCoordOfUpperLeftCornerOfGameView);;
}

VisibleTileRange calculateVisibleTileRange(sf::RenderWindow& window, const int m_tilesPerRow,
                                           const int m_tilesPerColumn) {
    // ONLY considering the tiles that are visible in the camera (even if not fully visible!!!)
    VisibleTileRange range;

    // Size of the visible game area
    sf::Vector2f upperLeft = getVirtualUpperLeftCornerCoordOfGameView(window);  
    
    // Size of the visible game area
    sf::Vector2f size(gGameVisibleWorld_size_x, gGameVisibleWorld_size_y);

    range.firstCol = std::max(0, static_cast<int>(upperLeft.x / gTileSize));
    range.lastCol = std::min(m_tilesPerRow - 1,
                             static_cast<int>(std::ceil((upperLeft.x + size.x) / gTileSize)) - 1);

    range.firstRow = std::max(0, static_cast<int>(upperLeft.y / gTileSize));
    range.lastRow = std::min(m_tilesPerColumn - 1,
                             static_cast<int>(std::ceil((upperLeft.y + size.y) / gTileSize)) - 1);
    
    return range;
}
