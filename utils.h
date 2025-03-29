#include <SFML/Graphics.hpp>

/**
 * @struct VisibleTileRange
 * @brief Represents the range of visible tiles in the current game view.
 * 
 * This struct stores the row and column indices that define the visible 
 * tile area on the screen, based on the camera's position and size.
 */
struct VisibleTileRange {
    int firstCol; ///< First visible column in the view
    int lastCol;  ///< Last visible column in the view
    int firstRow; ///< First visible row in the view
    int lastRow;  ///< Last visible row in the view
};


/**
 * Retrieves the virtual coordinates of the top-left corner of the game view.
 *
 * Calculates the pixel corresponding to the top-left corner of the view's rectangle
 * (which may be smaller than the window to maintain the aspect ratio) and converts it to game coordinates
 * using mapPixelToCoords.
 *
 * @param window The window containing the game view.
 * @return The virtual coordinates of the top-left corner of the visible game area.
 */
sf::Vector2f getVirtualUpperLeftCornerCoordOfGameView(sf::RenderWindow &window);

/**
 * @brief Calculates the range of tiles that are visible in the current game view.
 * 
 * This function determines the first and last visible tile indices (both rows and columns)
 * based on the position and size of the game view. It ensures that even partially visible 
 * tiles at the edges are included.
 * 
 * @param window Reference to the SFML render window.
 * @param m_tilesPerRow Total number of tiles per row in the tilemap.
 * @param m_tilesPerColumn Total number of tiles per column in the tilemap.
 * @return VisibleTileRange The calculated range of visible tiles.
 */
VisibleTileRange calculateVisibleTileRange(sf::RenderWindow& window, const int m_tilesPerRow, const int m_tilesPerColumn);