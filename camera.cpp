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
        // La ventana es más ancha que la vista original: fijamos la altura
        adjustedSize.y = this->viewSize.y;
        adjustedSize.x = this->viewSize.y * windowAspect;
    }
    else
    {
        // La ventana es más estrecha: fijamos el ancho
        adjustedSize.x = this->viewSize.x;
        adjustedSize.y = this->viewSize.x / windowAspect;
    }

    return sf::View(sf::FloatRect(this->startVertex, adjustedSize));
}
