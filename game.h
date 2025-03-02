#pragma once

#include <SFML/Graphics.hpp>

// Inicializa los recursos y configuraciones necesarias
bool Init();

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(sf::RenderWindow &window, float deltaTime);

// Renderiza los elementos en la ventana
void Render(sf::RenderWindow &window, const sf::Text &text);