#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

const int MAP_WIDTH = 32;
const int MAP_HEIGHT = 6;
const int TILESET_WIDTH = 6;
const int TILESET_HEIGHT = 9;

const int TILE_SIZE = 32;
const int MARGIN = 4;
const int OUTER_MARGIN = 2;

const int OFFSET_X = 300;
const int ALTURA_BOTON_GUARDAR = 400;

class Game {
public:
    Game() : window(sf::VideoMode({TILE_SIZE * MAP_WIDTH + TILE_SIZE * TILESET_WIDTH + 300, TILE_SIZE * MAP_HEIGHT + TILE_SIZE * TILESET_HEIGHT + 100}), "Tilemap Game", sf::Style::Default) {
        if (!tilesetTexture.loadFromFile("../../assets/tilesets/tileset_5.png")) {
            std::cerr << "Error loading tileset!" << std::endl;
            exit(-1);
        }

        tilesPerRow = (tilesetTexture.getSize().x + MARGIN) / (TILE_SIZE + MARGIN);
        tilesPerCol = (tilesetTexture.getSize().y + MARGIN) / (TILE_SIZE + MARGIN);

        for (int i = 0; i < TILESET_HEIGHT; ++i) {
            for (int j = 0; j < TILESET_WIDTH; ++j) {
                tilemap[i][j] = (i * TILESET_WIDTH + j) % (tilesPerRow * tilesPerCol);
            }
        }

        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                grid[i][j] = -1;
            }
        }

        acceptButton.setSize(sf::Vector2f(100, 50));
        acceptButton.setPosition({650, ALTURA_BOTON_GUARDAR});
        acceptButton.setFillColor(sf::Color::Green);

        selectedTile = -1;

        // Tries to load the existing grid (if it exists)
        loadGrid();
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
    int tilemap[TILESET_HEIGHT][TILESET_WIDTH];
    int grid[MAP_HEIGHT][MAP_WIDTH];
    sf::Texture tilesetTexture;
    int tilesPerRow;
    int tilesPerCol;
    int selectedTile;

    void drawTilemap() {
        sf::Sprite tileSprite(tilesetTexture);

        for (int i = 0; i < TILESET_HEIGHT; ++i) {
            for (int j = 0; j < TILESET_WIDTH; ++j) {
                int tileIndex = tilemap[i][j];
                int tu = tileIndex % tilesPerRow;
                int tv = tileIndex / tilesPerRow;

                int texX = OUTER_MARGIN + tu * (TILE_SIZE + MARGIN);
                int texY = OUTER_MARGIN + tv * (TILE_SIZE + MARGIN);

                tileSprite.setTextureRect(sf::IntRect({texX, texY}, {TILE_SIZE, TILE_SIZE}));
                tileSprite.setPosition({j * TILE_SIZE, i * TILE_SIZE});
                window.draw(tileSprite);
            }
        }
    }

    void drawGrid() {
        sf::Sprite tileSprite(tilesetTexture);

        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                square.setPosition({OFFSET_X + j * TILE_SIZE, i * TILE_SIZE});

                if (grid[i][j] != -1) {
                    int tileIndex = grid[i][j];
                    int tu = tileIndex % tilesPerRow;
                    int tv = tileIndex / tilesPerRow;

                    int texX = OUTER_MARGIN + tu * (TILE_SIZE + MARGIN);
                    int texY = OUTER_MARGIN + tv * (TILE_SIZE + MARGIN);

                    tileSprite.setTextureRect(sf::IntRect({texX, texY}, {TILE_SIZE, TILE_SIZE}));
                    tileSprite.setPosition({OFFSET_X + j * TILE_SIZE, i * TILE_SIZE});
                    window.draw(tileSprite);
                } else {
                    square.setFillColor(sf::Color(200, 200, 200));
                    window.draw(square);
                }
                square.setOutlineThickness(1);
                square.setOutlineColor(sf::Color::Black);
            }
        }
    }

    void drawAcceptButton() {
        window.draw(acceptButton);
    }

    void handleMouseClick(int x, int y) {
        if (x < TILESET_WIDTH * TILE_SIZE && y < TILESET_HEIGHT * TILE_SIZE) {
            int tileX = x / TILE_SIZE;
            int tileY = y / TILE_SIZE;
            selectedTile = tilemap[tileY][tileX];
            std::cout << "Tile seleccionado: " << selectedTile << std::endl;
        }

        if (x >= OFFSET_X && x < OFFSET_X + MAP_WIDTH * TILE_SIZE && y < MAP_HEIGHT * TILE_SIZE) {
            int gridX = (x - OFFSET_X) / TILE_SIZE;
            int gridY = y / TILE_SIZE;
            if (selectedTile != -1) {
                grid[gridY][gridX] = selectedTile;
            }
        }

        if (x >= 650 && x <= 750 && y >= ALTURA_BOTON_GUARDAR && y <= 550) {
            saveGrid();
        }
    }

    void saveGrid() {
        std::ofstream outFile("grid.txt");

        // Map dimensions
        outFile << MAP_WIDTH << "," << MAP_HEIGHT << std::endl;

        // Initial position
        outFile << 48 << "," << 64 << std::endl;
        outFile << 0 << std::endl;

        // Solid/background tiles
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                outFile << grid[i][j];
                if (j < MAP_WIDTH - 1) {
                    outFile << ",";
                }
            }
            outFile << std::endl;
        }
        
        // Other tiles
        outFile << "doors" << std::endl;
        outFile << "end_doors" << std::endl;
        outFile << "miscellaneous" << std::endl;
        outFile << "end_miscellaneous" << std::endl;
        outFile << "stair" << std::endl;
        outFile << "end_stair" << std::endl;
        outFile << "enemies" << std::endl;
        outFile << "end_enemies" << std::endl;

        outFile.close();
        std::cout << "Datos guardados en 'grid.txt'" << std::endl;
    }

    void loadGrid() {
        std::ifstream inFile("grid.txt");
        if (!inFile) {
            std::cerr << "No se pudo cargar el archivo 'grid.txt'. Se usara un mapa vacio." << std::endl;
            return;
        }

        std::string line;
        std::getline(inFile, line);  // Skips the first line (map's dimensions)
        std::getline(inFile, line);  // Skips the second line (initial position)
        std::getline(inFile, line);  // Skips the third line (boss position)

        for (int i = 0; i < MAP_HEIGHT; ++i) {
            if (!std::getline(inFile, line)) break;
            std::stringstream ss(line);
            std::string value;
            for (int j = 0; j < MAP_WIDTH; ++j) {
                if (!std::getline(ss, value, ',')) break;
                grid[i][j] = std::stoi(value);
            }
        }

        inFile.close();
        std::cout << "Mapa cargado correctamente desde 'grid.txt'." << std::endl;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
