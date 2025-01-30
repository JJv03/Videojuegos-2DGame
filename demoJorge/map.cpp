#include "Map.h"

// Constructor que inicializa el tamaño de la celda y la cuadrícula vacía
Map::Map(float cellSize) : grid(), cellSize(cellSize) {}

void Map::CreateFromImage(const sf::Image &image)
{
    grid.clear();

    grid = std::vector(image.getSize().x, std::vector(image.getSize().y, 0));

    for (size_t x = 0; x < grid.size(); x++)
    {
        for (size_t y = 0; y < grid[x].size(); y++)
        {
            sf::Vector2u pixelCoords(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
            sf::Color color = image.getPixel(pixelCoords);
            if (color == sf::Color::Black)
            {
                grid[x][y] = 1;
            }
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