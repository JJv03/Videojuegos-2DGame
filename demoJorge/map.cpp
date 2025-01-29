#include "Map.h"

// Constructor que inicializa el tamaño de la celda y la cuadrícula vacía
Map::Map(float cellSize) : grid(), cellSize(cellSize) {}

void Map::createCheckerboard(size_t width, size_t height)
{
    // Inicializa la cuadrícula con ceros
    grid = std::vector(width, std::vector(height, 0));

    // Llena la cuadrícula con unos
    for (auto &column : grid)
    {
        for (auto &cell : column)
        {
            cell = 1;
        }
    }
}

void Map::Draw(Renderer &renderer)
{
    int x = 0;

    // Recorre la cuadrícula y dibuja las celdas activas
    for (const auto &column : grid)
    {
        int y = 0;
        for (const auto &cell : column)
        {
            if (cell)
            {
                renderer.Draw(Resources::textures["square"],
                              sf::Vector2f(cellSize * x + cellSize / 2.0f, cellSize * y + cellSize / 2.0f), sf::Vector2f(cellSize, cellSize));
            }
            y++;
        }
        x++;
    }
}