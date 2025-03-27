#include <iostream>
#include "camera.h"

Camera::Camera(sf::Vector2f _viewSize) : viewSize(_viewSize), startVertex(0.f, 0.f) {}

Camera::Camera(sf::FloatRect _viewRect) : viewSize(_viewRect.size), startVertex(_viewRect.position) {}

Camera::Camera(sf::Vector2f _startVertex, sf::Vector2f _viewSize) :
                viewSize(_viewSize), startVertex(_startVertex) {}


sf::View Camera::getView(const sf::Vector2u& windowSize)
{
    const float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;
    const float viewAspect = this->viewSize.x / this->viewSize.y;
    sf::Vector2f adjustedSize;

    if (windowAspect > viewAspect)
    {
        // If window is wider that original view: Set height and adjust width
        adjustedSize.y = this->viewSize.y;
        adjustedSize.x = this->viewSize.y * windowAspect;
        //this->startVertex.x = -(adjustedSize.x - this->viewSize.x) / 2.f;
    }
    else
    {
        // If window is thinner that original view: Set width and adjust height
        adjustedSize.x = this->viewSize.x;
        adjustedSize.y = this->viewSize.x / windowAspect;
        //this->startVertex.x = -(adjustedSize.x - this->viewSize.x) / 2.f;
    }

    // std::cout << "Start Vertex" << this->startVertex.x << ", " << this->startVertex.y << std::endl;
    // std::cout << "Adjusted size: " << adjustedSize.x << ", " << adjustedSize.y << std::endl;
    return sf::View(sf::FloatRect(this->startVertex, adjustedSize));
}
