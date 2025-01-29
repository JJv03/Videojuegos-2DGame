#pragma once

#include <SFML/Graphics.hpp>

class Renderer
{
public:
    Renderer(sf::RenderTarget &target);

    // Método para dibujar una textura en una posición y tamaño especificados, con un recorte opcional.
    void Draw(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size, const sf::IntRect &cut);

private:
    sf::RenderTarget &target; // Referencia al objetivo de renderizado.
};