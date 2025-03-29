#include <SFML/Graphics.hpp>

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
