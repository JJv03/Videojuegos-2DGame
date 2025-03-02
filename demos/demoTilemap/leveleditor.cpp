#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>

const int TILE_SIZE = 32; // Tamaño de cada tile en píxeles
const int MAP_WIDTH = 24; // Ancho del mapa (en tiles)
const int MAP_HEIGHT = 7; // Alto del mapa (en tiles)
const int TILESET_WIDTH = 10; // Ancho del tileset (en tiles)
const int TILESET_HEIGHT = 8; // Alto del tileset (en tiles)
const int MARGIN = 1; // Margen de 1 píxel entre los tiles

// Clase para manejar el Tilemap y la interacción
class Game {
public:
    Game() : window(sf::VideoMode({TILE_SIZE*MAP_WIDTH + TILE_SIZE*TILESET_WIDTH + 100, TILE_SIZE*MAP_HEIGHT + TILE_SIZE*TILESET_HEIGHT + 100}), "Tilemap Game", sf::Style::Default) {
        // Cargar la textura del tileset
        if (!tilesetTexture.loadFromFile("../assets/tilesets/tileset_1.png")) {
            std::cerr << "Error loading tileset!" << std::endl;
            exit(-1);  // Si no se puede cargar el tileset, salimos
        }

        // Calcular el número de tiles por fila y por columna automáticamente
        tilesPerRow = (tilesetTexture.getSize().x + MARGIN) / (TILE_SIZE + MARGIN);
        tilesPerCol = (tilesetTexture.getSize().y + MARGIN) / (TILE_SIZE + MARGIN);

        // Inicializar el tilemap con índices de tiles en el tileset (por ejemplo, 0-31)
        int tileIndex = 0;
        for (int i = 0; i < TILESET_HEIGHT; ++i) {
            for (int j = 0; j < TILESET_WIDTH; ++j) {
                tilemap[i][j] = tileIndex++;
                if (tileIndex >= tilesPerRow * tilesPerCol) tileIndex = 0; // Para repetir el tileset
            }
        }

        // Inicializar el grid 10x10 (derecha) con valores predeterminados
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                grid[i][j] = -1; // Iniciar con valor -1 (sin seleccionar)
            }
        }

        // Configurar el botón de "aceptar"
        acceptButton.setSize(sf::Vector2f(100, 50));
        acceptButton.setPosition({650, 500});
        acceptButton.setFillColor(sf::Color::Green);

        // Variable para el tile seleccionado
        selectedTile = -1;
    }

    void run() {
        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }

                if (event->is<sf::Event::MouseButtonPressed>()) {
                    if (const auto *info = event->getIf<sf::Event::MouseButtonPressed>()) {
                        handleMouseClick(info->position.x, info->position.y);
                    }
                }
            }

            window.clear(sf::Color::White);
            drawTilemap();
            drawGrid();
            drawAcceptButton();
            window.display();
        }
    }

private:
    sf::RenderWindow window;
    sf::RectangleShape acceptButton;

    // Tilemap con índices que corresponden a los tiles en el tileset
    int tilemap[TILESET_HEIGHT][TILESET_WIDTH];  // Ejemplo de 5x5 tilemap (puedes hacer este tamaño más grande si lo necesitas)

    // Grid 10x10 para pintar
    int grid[MAP_HEIGHT][MAP_WIDTH];

    // Textura del tileset
    sf::Texture tilesetTexture;

    // Número de tiles por fila y por columna en el tileset
    int tilesPerRow;
    int tilesPerCol;

    // Variable para guardar el índice del tile seleccionado
    int selectedTile;

    // Función para dibujar el tilemap (a la izquierda)
    void drawTilemap() {
        sf::Sprite tileSprite(tilesetTexture);

        for (int i = 0; i < TILESET_HEIGHT; ++i) {
            for (int j = 0; j < TILESET_WIDTH; ++j) {

                // Obtener el número de tile en el tilemap
                int tileIndex = tilemap[i][j];

                // Calcular la fila (tv) y columna (tu) en el tileset
                int tu = tileIndex % tilesPerRow;
                int tv = tileIndex / tilesPerRow;

                // Calcular las coordenadas del tile en el tileset, saltando el margen
                int texX = tu * (TILE_SIZE + MARGIN);
                int texY = tv * (TILE_SIZE + MARGIN);

                // Configurar la textura para el sprite
                tileSprite.setTextureRect(sf::IntRect({texX, texY}, {TILE_SIZE, TILE_SIZE}));
                tileSprite.setPosition({j * TILE_SIZE, i * TILE_SIZE});
                window.draw(tileSprite);
            }
        }
    }

    // Función para dibujar el grid 10x10 (a la derecha)
    void drawGrid() {
        sf::Sprite tileSprite(tilesetTexture);

        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                square.setPosition({400 + j * TILE_SIZE, i * TILE_SIZE});

                // Si el tile está seleccionado, lo pintamos con el tile del tilemap
                if (grid[i][j] != -1) {
                    // Usamos el índice de tile para dibujar el sprite seleccionado
                    int tileIndex = grid[i][j];
                    int tu = tileIndex % tilesPerRow;
                    int tv = tileIndex / tilesPerRow;

                    int texX = tu * (TILE_SIZE + MARGIN);
                    int texY = tv * (TILE_SIZE + MARGIN);

                    tileSprite.setTextureRect(sf::IntRect({texX, texY}, {TILE_SIZE, TILE_SIZE}));
                    tileSprite.setPosition({400 + j * TILE_SIZE, i * TILE_SIZE});
                    window.draw(tileSprite);
                } else {
                    square.setFillColor(sf::Color(200, 200, 200)); // Cuadrado sin pintar
                    window.draw(square);
                }
                square.setOutlineThickness(1);
                square.setOutlineColor(sf::Color::Black);
            }
        }
    }

    // Función para dibujar el botón "Aceptar"
    void drawAcceptButton() {
        window.draw(acceptButton);
    }

    // Función para manejar clics del mouse
    void handleMouseClick(int x, int y) {
        // Comprobar si se hizo clic en el tilemap (a la izquierda)
        if (x < TILESET_WIDTH * TILE_SIZE && y < TILESET_HEIGHT * TILE_SIZE) {
            int tileX = x / TILE_SIZE;
            int tileY = y / TILE_SIZE;
            selectedTile = tilemap[tileY][tileX]; // Guardamos el índice del tile seleccionado
            std::cout << "Tile seleccionado: " << selectedTile << std::endl;
        }

        // Comprobar si se hizo clic en el grid 10x10 (a la derecha)
        if (x >= 400 && x < 400 + MAP_WIDTH * TILE_SIZE && y < MAP_HEIGHT * TILE_SIZE) {
            int gridX = (x - 400) / TILE_SIZE;
            int gridY = y / TILE_SIZE;
            if (selectedTile != -1) {
                // Pintar el cuadrado en el grid con el tile seleccionado
                grid[gridY][gridX] = selectedTile;
            }
        }

        // Comprobar si se hizo clic en el botón "Aceptar"
        if (x >= 650 && x <= 750 && y >= 500 && y <= 550) {
            saveGrid();
        }
    }

    // Guardar el estado del grid 10x10 en un archivo de texto
    void saveGrid() {
        std::ofstream outFile("grid.txt");
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                outFile << grid[i][j];
                if (j < MAP_WIDTH - 1) {
                    outFile << ",";
                }
            }
            outFile << std::endl;
        }
        outFile.close();
        std::cout << "Datos guardados en 'grid.txt'" << std::endl;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
