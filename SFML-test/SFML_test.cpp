#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <windows.h> // Incluir el encabezado para usar MessageBoxA

using namespace sf;
using namespace std;

const int BOARD_SIZE = 10; // Tamaño del tablero 10x10
const int TILE_SIZE = 30; // Tamaño de cada casilla
const int BOARD_OFFSET_X = 50; // Desplazamiento del primer tablero
const int BOARD_OFFSET_Y = 50;
const int BOARD2_OFFSET_X = BOARD_OFFSET_X + BOARD_SIZE * TILE_SIZE + 50; // Desplazamiento del segundo tablero

const int SHIP_SIZES[] = {5, 4, 3, 3, 2}; // Tamaños de los barcos según las reglas del juego
const int NUM_SHIPS = sizeof(SHIP_SIZES) / sizeof(SHIP_SIZES[0]);

struct Ship {
    RectangleShape shape;
    bool isDragging = false;
    Vector2f offset;
    bool isHorizontal = true;

    Ship(int size, float x, float y) {
        shape.setSize(Vector2f(size * TILE_SIZE, TILE_SIZE));
        shape.setFillColor(Color::Green);
        shape.setOutlineColor(Color::Black);
        shape.setOutlineThickness(2);
        shape.setPosition(x, y);
    }

    void rotate() {
        if (isHorizontal) {
            shape.setSize(Vector2f(TILE_SIZE, shape.getSize().x));
        } else {
            shape.setSize(Vector2f(shape.getSize().y, TILE_SIZE));
        }
        isHorizontal = !isHorizontal;
    }

    void alignToGrid(int offsetX, int offsetY) {
        Vector2f pos = shape.getPosition();
        int gridX = static_cast<int>((pos.x - offsetX + TILE_SIZE / 2) / TILE_SIZE);
        int gridY = static_cast<int>((pos.y - offsetY + TILE_SIZE / 2) / TILE_SIZE);

        if (isHorizontal) {
            if (gridX + static_cast<int>(shape.getSize().x / TILE_SIZE) <= BOARD_SIZE && gridY < BOARD_SIZE) {
                shape.setPosition(offsetX + gridX * TILE_SIZE, offsetY + gridY * TILE_SIZE);
            }
        } else {
            if (gridY + static_cast<int>(shape.getSize().y / TILE_SIZE) <= BOARD_SIZE && gridX < BOARD_SIZE) {
                shape.setPosition(offsetX + gridX * TILE_SIZE, offsetY + gridY * TILE_SIZE);
            }
        }
    }

    void keepInBoard(int offsetX, int offsetY) {
        Vector2f pos = shape.getPosition();
        int gridX = static_cast<int>((pos.x - offsetX) / TILE_SIZE);
        int gridY = static_cast<int>((pos.y - offsetY) / TILE_SIZE);

        if (isHorizontal) {
            if (gridX < 0) gridX = 0;
            if (gridX + static_cast<int>(shape.getSize().x / TILE_SIZE) > BOARD_SIZE) gridX = BOARD_SIZE - static_cast<int>(shape.getSize().x / TILE_SIZE);
            if (gridY < 0) gridY = 0;
            if (gridY >= BOARD_SIZE) gridY = BOARD_SIZE - 1;
            shape.setPosition(offsetX + gridX * TILE_SIZE, offsetY + gridY * TILE_SIZE);
        } else {
            if (gridX < 0) gridX = 0;
            if (gridX >= BOARD_SIZE) gridX = BOARD_SIZE - 1;
            if (gridY < 0) gridY = 0;
            if (gridY + static_cast<int>(shape.getSize().y / TILE_SIZE) > BOARD_SIZE) gridY = BOARD_SIZE - static_cast<int>(shape.getSize().y / TILE_SIZE);
            shape.setPosition(offsetX + gridX * TILE_SIZE, offsetY + gridY * TILE_SIZE);
        }
    }

    bool intersects(const Ship& other) const {
        return shape.getGlobalBounds().intersects(other.shape.getGlobalBounds());
    }
};

bool canPlaceShip(const Ship& ship, const vector<Ship>& ships) {
    for (const auto& otherShip : ships) {
        if (&ship != &otherShip && ship.intersects(otherShip)) {
            return false;
        }
    }
    return true;
}

void drawBoard(RenderWindow& window, const vector<vector<bool>>& board, int offsetX, int offsetY, const Font& font) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            RectangleShape tile(Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setPosition(offsetX + i * TILE_SIZE, offsetY + j * TILE_SIZE);
            tile.setOutlineColor(Color::Black);
            tile.setOutlineThickness(1);

            if (board[i][j]) {
                tile.setFillColor(Color::Blue); // Casilla con barco
            } else {
                tile.setFillColor(Color::White); // Casilla vacía
            }

            window.draw(tile);
        }
    }

    // Dibujar numeros y letras
    for (int i = 1; i <= BOARD_SIZE; ++i) {
        Text number;
        number.setFont(font);
        number.setString(to_string(i));
        number.setCharacterSize(16);
        number.setFillColor(Color::Black);
        number.setPosition(offsetX + (i - 1) * TILE_SIZE + TILE_SIZE / 2 - 5, offsetY - TILE_SIZE / 2 - 10);

        Text letter;
        letter.setFont(font);
        letter.setString(static_cast<char>('A' + i - 1));
        letter.setCharacterSize(16);
        letter.setFillColor(Color::Black);
        letter.setPosition(offsetX - TILE_SIZE / 2 - 10, offsetY + (i - 1) * TILE_SIZE + TILE_SIZE / 2 - 10);

        window.draw(number);
        window.draw(letter);
    }
}

void markBoard(RenderWindow& window, const vector<vector<bool>>& board, int offsetX, int offsetY, const Font& font) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j]) {
                Text mark;
                mark.setFont(font);
                mark.setString("X");
                mark.setCharacterSize(16);
                mark.setFillColor(Color::Red);
                mark.setPosition(offsetX + i * TILE_SIZE + TILE_SIZE / 2 - 5, offsetY + j * TILE_SIZE + TILE_SIZE / 2 - 10);
                window.draw(mark);
            }
        }
    }
}

int main() {
    // Crear la ventana
    RenderWindow window(VideoMode(800, 600), "Batalla Naval");

    // Cargar la fuente
    Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        // Mostrar un cuadro de dialogo si no se puede cargar la fuente
        MessageBoxA(NULL, "No se pudo cargar la fuente", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Inicializar los tableros
    vector<vector<bool>> board1(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    vector<vector<bool>> board2(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));

    // Crear los barcos (dos de cada tamaño)
    vector<Ship> ships;
    float shipStartX = 50;
    float shipStartY = BOARD_OFFSET_Y + BOARD_SIZE * TILE_SIZE + 50;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < NUM_SHIPS; ++j) {
            ships.push_back(Ship(SHIP_SIZES[j], shipStartX + i * 200, shipStartY + j * (TILE_SIZE + 10)));
        }
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                for (auto& ship : ships) {
                    if (ship.isDragging) {
                        ship.rotate();
                        ship.keepInBoard(BOARD_OFFSET_X, BOARD_OFFSET_Y);
                    }
                }
            }
            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    for (auto& ship : ships) {
                        if (ship.shape.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            ship.isDragging = true;
                            ship.offset = ship.shape.getPosition() - Vector2f(mousePos.x, mousePos.y);
                            break;
                        }
                    }
                }
            }
            if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == Mouse::Left) {
                    for (auto& ship : ships) {
                        if (ship.isDragging) {
                            ship.alignToGrid(BOARD_OFFSET_X, BOARD_OFFSET_Y);
                            if (!canPlaceShip(ship, ships)) {
                                ship.isDragging = true;
                            } else {
                                ship.isDragging = false;
                            }
                            ship.keepInBoard(BOARD_OFFSET_X, BOARD_OFFSET_Y);
                        }
                    }
                }
                if (event.mouseButton.button == Mouse::Right) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    if (mousePos.x >= BOARD2_OFFSET_X && mousePos.x <= BOARD2_OFFSET_X + BOARD_SIZE * TILE_SIZE &&
                        mousePos.y >= BOARD_OFFSET_Y && mousePos.y <= BOARD_OFFSET_Y + BOARD_SIZE * TILE_SIZE) {
                        int gridX = (mousePos.x - BOARD2_OFFSET_X) / TILE_SIZE;
                        int gridY = (mousePos.y - BOARD_OFFSET_Y) / TILE_SIZE;
                        board2[gridX][gridY] = true;
                    }
                }
            }
            if (event.type == Event::MouseMoved) {
                Vector2i mousePos = Mouse::getPosition(window);
                for (auto& ship : ships) {
                    if (ship.isDragging) {
                        ship.shape.setPosition(mousePos.x + ship.offset.x, mousePos.y + ship.offset.y);
                    }
                }
            }
        }

        window.clear(Color::White);

        // Dibujar los tableros
        drawBoard(window, board1, BOARD_OFFSET_X, BOARD_OFFSET_Y, font);
        drawBoard(window, board2, BOARD2_OFFSET_X, BOARD_OFFSET_Y, font);

        // Dibujar los barcos
        for (auto& ship : ships) {
            window.draw(ship.shape);
        }

        // Marcar las casillas seleccionadas en el tablero derecho
        markBoard(window, board2, BOARD2_OFFSET_X, BOARD_OFFSET_Y, font);

        window.display();
    }

    return 0;
}
