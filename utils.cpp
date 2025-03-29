#include <iostream>
#include "utils.h"

sf::Vector2f getVirtualUpperLeftCornerCoordOfGameView(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f gameViewPosition = window.getView().getViewport().position;
    sf::Vector2i windowPixelCoordOfUpperLeftCornerOfGameView(windowSize.x * gameViewPosition.x, windowSize.y * gameViewPosition.y);
    sf::Vector2f virtualCoordOfUpperLeftCornerOfGame = window.mapPixelToCoords(windowPixelCoordOfUpperLeftCornerOfGameView);
    //std::cout << "Upper left corner of game view: " << virtualCoordOfUpperLeftCornerOfGame.x << ", " << virtualCoordOfUpperLeftCornerOfGame.y << std::endl;
    return virtualCoordOfUpperLeftCornerOfGame;
}