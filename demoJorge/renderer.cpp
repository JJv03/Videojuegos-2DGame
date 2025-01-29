#include "renderer.h"
#include <vector>

Renderer::Renderer(sf::RenderTarget &target) : target(target)
{
}

// Método para dibujar una textura en una posición y tamaño especificados, con un recorte opcional.
void Renderer::DrawCut(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size, const sf::IntRect &cut)
{
    sf::Sprite sprite(texture);
    sprite.setTextureRect(cut);
    // Set origin??
    sprite.setPosition(position);
    sprite.setScale(sf::Vector2f(size.x / texture.getSize().x, size.y / texture.getSize().y));

    target.draw(sprite);
}

// Método para dibujar una textura en una posición y tamaño especificados, sin recorte.
void Renderer::Draw(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size)
{
    sf::Sprite sprite(texture);
    sprite.setTexture(texture);
    sprite.setOrigin((sf::Vector2f)texture.getSize() / 2.0f);
    sprite.setPosition(position);
    sprite.setScale(sf::Vector2f(size.x / texture.getSize().x, size.y / texture.getSize().y));

    target.draw(sprite);
}
