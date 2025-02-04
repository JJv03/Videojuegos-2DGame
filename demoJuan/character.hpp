// character.hpp
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Character
{
public:
    // Constructor
    Character(const std::string& textureFile, const sf::Vector2f& position);

    // Métodos para cambiar las animaciones
    void setIdleAnimation();
    void setJumpAnimation();

    // Actualizar el personaje
    void update(float deltaTime);

    // Mover el personaje
    void move(float dx, float dy);

    // Dibujar el personaje
    void draw(sf::RenderWindow& window) const;

    // Obtener el sprite
    sf::Sprite& getSprite();

private:
    // Miembros de la clase
    sf::Texture m_texture;  // Textura de la spritesheet
    sf::Sprite m_sprite;    // Sprite del personaje
    int m_frameWidth = 16;  // Ancho de un cuadro de la spritesheet
    int m_frameHeight = 32; // Alto de un cuadro de la spritesheet
    bool m_isJumping = false; // Estado del salto
};

#endif
