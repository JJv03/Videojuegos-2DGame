#pragma once

#include <vector>
#include "renderer.h"
#include "resources.h"

class Map
{
public:
    Map(float cellSize = 32.0f);
    void createCheckerboard(size_t width, size_t height); // Crea un tablero de ajedrez en la cuadrícula
    void Draw(Renderer &renderer);                        // Dibuja el mapa usando un renderizador

    std::vector<std::vector<int>> grid; // Matriz que representa la cuadrícula del mapa
    float cellSize;
};