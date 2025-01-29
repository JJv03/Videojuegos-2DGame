#pragma once

#include <SFML/Graphics.hpp>

// Inicializa los recursos y configuraciones necesarias
void Init(const sf::Window &window);

// Actualiza la lógica del programa en función del tiempo transcurrido
void Update(float deltaTime);

// Renderiza los elementos en la ventana
void Render(sf::RenderWindow &window);