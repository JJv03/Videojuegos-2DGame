// character.cpp
#include "character.hpp"
#include <iostream>

Character::Character(const std::string& textureFile, const sf::Vector2f& position)
{
    // Cargar la spritesheet completa
    if (!m_texture.loadFromFile(textureFile))
    {
        std::cerr << "Error al cargar la textura de la spritesheet" << std::endl;
    }

    // Asignar la textura completa al sprite
    m_sprite.setTexture(m_texture);
    m_sprite.setPosition(position);

    // Asignar la textura inicial (Idle)
    setIdleAnimation();
}

void Character::setIdleAnimation()
{
    // Definir el recorte para la animación de "idle" (por ejemplo, el primer cuadro de la spritesheet)
    m_sprite.setTextureRect(sf::IntRect(sf::Vector2i(1, 21), sf::Vector2i(m_frameWidth, m_frameHeight)));
    m_isJumping = false;
}

void Character::setJumpAnimation()
{
    // Definir el recorte para la animación de "jump" (supongamos que el cuadro de salto está en la posición 1 de la spritesheet)
    m_sprite.setTextureRect(sf::IntRect(sf::Vector2i(84, 21), sf::Vector2i(m_frameWidth, m_frameHeight)));
    m_isJumping = true;
}

void Character::update(float deltaTime)
{
    // Aquí puedes actualizar la lógica de las animaciones o el movimiento del personaje

    // Si está saltando, usa la animación de salto
    if (m_isJumping)
    {
        setJumpAnimation();
    }
    else
    {
        setIdleAnimation();
    }
}

void Character::move(float dx, float dy)
{
    m_sprite.move(sf::Vector2f(dx, dy));  // Mueve el sprite
}

void Character::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);  // Dibuja el sprite en la ventana
}

sf::Sprite& Character::getSprite()
{
    return m_sprite;  // Retorna el sprite
}
