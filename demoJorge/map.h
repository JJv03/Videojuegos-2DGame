#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "renderer.h"
#include "resources.h"

class Map
{
public:
    Map(float cellSize = 16.0f);
    void CreateFromImage(const sf::Image &image); // Crea el grid en base a una imagen

    void Draw(Renderer &renderer); // Dibuja el mapa usando un renderizador

    std::vector<std::vector<int>> grid; // Matriz que representa la cuadrícula del mapa
    float cellSize;
};