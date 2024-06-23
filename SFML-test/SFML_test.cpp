#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <windows.h>

using namespace sf;
using namespace std;

const int BOARD_SIZE = 10; // Tamaño del tablero 10x10
const int TILE_SIZE = 30; // Tamaño de cada casilla
const int BOARD_OFFSET_X = 50; // Desplazamiento del primer tablero
const int BOARD_OFFSET_Y = 50;
const int BOARD2_OFFSET_X = BOARD_OFFSET_X + BOARD_SIZE * TILE_SIZE + 50; // Desplazamiento del segundo tablero

struct Ship {
    RectangleShape shape;
    bool dragging = false;
    Vector2f offset;
    bool horizontal = true;
};

void drawBoard(RenderWindow& window, const std::vector<std::vector<bool>>& board, int offsetX, int offsetY, const Font& font, bool isRightBoard) {
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

            // Dibujar una 'X' roja en el tablero derecho si la casilla está seleccionada
            if (isRightBoard && board[i][j]) {
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

    // Dibujar números y letras
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

bool isWithinBounds(const Vector2f& position, const Vector2f& size, int offsetX, int offsetY) {
    return position.x >= offsetX && position.y >= offsetY &&
           position.x + size.x <= offsetX + BOARD_SIZE * TILE_SIZE &&
           position.y + size.y <= offsetY + BOARD_SIZE * TILE_SIZE;
}

bool checkOverlap(const vector<vector<bool>>& board, const Ship& ship, int gridX, int gridY) {
    int length = static_cast<int>(ship.horizontal ? ship.shape.getSize().x / TILE_SIZE : ship.shape.getSize().y / TILE_SIZE);
    for (int i = 0; i < length; ++i) {
        if (ship.horizontal) {
            if (gridX + i >= BOARD_SIZE || board[gridX + i][gridY]) return true;
        } else {
            if (gridY + i >= BOARD_SIZE || board[gridX][gridY + i]) return true;
        }
    }
    return false;
}

void clearShip(vector<vector<bool>>& board, const Ship& ship) {
    int length = static_cast<int>(ship.horizontal ? ship.shape.getSize().x / TILE_SIZE : ship.shape.getSize().y / TILE_SIZE);
    for (int i = 0; i < length; ++i) {
        if (ship.horizontal) {
            int gridX = (ship.shape.getPosition().x - BOARD_OFFSET_X) / TILE_SIZE;
            int gridY = (ship.shape.getPosition().y - BOARD_OFFSET_Y) / TILE_SIZE;
            if (gridX + i < BOARD_SIZE && gridY < BOARD_SIZE) {
                board[gridX + i][gridY] = false;
            }
        } else {
            int gridX = (ship.shape.getPosition().x - BOARD_OFFSET_X) / TILE_SIZE;
            int gridY = (ship.shape.getPosition().y - BOARD_OFFSET_Y) / TILE_SIZE;
            if (gridX < BOARD_SIZE && gridY + i < BOARD_SIZE) {
                board[gridX][gridY + i] = false;
            }
        }
    }
}

void placeShip(vector<vector<bool>>& board, const Ship& ship, int gridX, int gridY) {
    int length = static_cast<int>(ship.horizontal ? ship.shape.getSize().x / TILE_SIZE : ship.shape.getSize().y / TILE_SIZE);
    for (int i = 0; i < length; ++i) {
        if (ship.horizontal) {
            board[gridX + i][gridY] = true;
        } else {
            board[gridX][gridY + i] = true;
        }
    }
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Batalla Naval");

    Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        MessageBoxA(NULL, "No se pudo cargar la fuente", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    vector<vector<bool>> board1(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    vector<vector<bool>> board2(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));

    vector<Ship> ships;

    vector<int> shipSizes = {2, 3, 3, 4, 5}; // Tamaños de los barcos
    int yOffset = BOARD_OFFSET_Y + BOARD_SIZE * TILE_SIZE + 20;

    for (int size : shipSizes) {
        for (int i = 0; i < 2; ++i) {
            Ship ship;
            ship.shape.setSize(Vector2f(size * TILE_SIZE, TILE_SIZE));
            ship.shape.setFillColor(Color::Blue);
            ship.shape.setOutlineColor(Color::Black);
            ship.shape.setOutlineThickness(1);
            ship.shape.setPosition(BOARD_OFFSET_X + (ships.size() * (size * TILE_SIZE + 10)), yOffset);
            ships.push_back(ship);
        }
    }

    Ship* selectedShip = nullptr;
    bool placementLocked = false;

    // Botón para finalizar la colocación
    RectangleShape finalizeButton(Vector2f(100, 30));
    finalizeButton.setFillColor(Color::Green);
    finalizeButton.setPosition(BOARD_OFFSET_X, yOffset + 50);

    Text buttonText("Finalize", font, 16);
    buttonText.setFillColor(Color::Black);
    buttonText.setPosition(BOARD_OFFSET_X + 10, yOffset + 55);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (!placementLocked) {
                if (event.type == Event::MouseButtonPressed) {
                    if (event.mouseButton.button == Mouse::Left) {
                        Vector2i mousePos = Mouse::getPosition(window);
                        for (auto& ship : ships) {
                            if (ship.shape.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                ship.dragging = true;
                                ship.offset = ship.shape.getPosition() - Vector2f(mousePos.x, mousePos.y);
                                selectedShip = &ship;
                                clearShip(board1, ship); // Clear previous position
                                break;
                            }
                        }

                        // Verificar si se hizo clic en el botón de finalizar
                        if (finalizeButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            placementLocked = true;
                        }
                    }
                }

                if (event.type == Event::MouseButtonReleased) {
                    if (event.mouseButton.button == Mouse::Left) {
                        for (auto& ship : ships) {
                            if (ship.dragging) {
                                ship.dragging = false;

                                // Asegurarse de que el barco no salga del tablero izquierdo
                                Vector2f pos = ship.shape.getPosition();
                                if (!isWithinBounds(pos, ship.shape.getSize(), BOARD_OFFSET_X, BOARD_OFFSET_Y)) {
                                    ship.shape.setPosition(BOARD_OFFSET_X, ship.shape.getPosition().y); // Reset position if out of bounds
                                } else {
                                    int gridX = (pos.x - BOARD_OFFSET_X) / TILE_SIZE;
                                    int gridY = (pos.y - BOARD_OFFSET_Y) / TILE_SIZE;
                                    if (!checkOverlap(board1, ship, gridX, gridY)) {
                                        ship.shape.setPosition(BOARD_OFFSET_X + gridX * TILE_SIZE,
                                                               BOARD_OFFSET_Y + gridY * TILE_SIZE);
                                        placeShip(board1, ship, gridX, gridY);
                                    } else {
                                        ship.shape.setPosition(BOARD_OFFSET_X, ship.shape.getPosition().y); // Reset position if overlapping
                                    }
                                }
                            }
                        }
                        selectedShip = nullptr;
                    }
                }

                if (event.type == Event::MouseMoved) {
                    if (selectedShip && selectedShip->dragging) {
                        Vector2i mousePos = Mouse::getPosition(window);
                        selectedShip->shape.setPosition(mousePos.x + selectedShip->offset.x, mousePos.y + selectedShip->offset.y);
                    }
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::R) {
                        if (selectedShip) {
                            clearShip(board1, *selectedShip); // Clear previous position before rotation
                            selectedShip->horizontal = !selectedShip->horizontal;
                            if (selectedShip->horizontal) {
                                selectedShip->shape.setSize(Vector2f(selectedShip->shape.getSize().y, TILE_SIZE));
                            } else {
                                selectedShip->shape.setSize(Vector2f(TILE_SIZE, selectedShip->shape.getSize().x));
                            }
                        }
                    }
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);

                    // Comprobar si se hizo clic en el segundo tablero
                    if (mousePos.x >= BOARD2_OFFSET_X && mousePos.x < BOARD2_OFFSET_X + BOARD_SIZE * TILE_SIZE &&
                        mousePos.y >= BOARD_OFFSET_Y && mousePos.y < BOARD_OFFSET_Y + BOARD_SIZE * TILE_SIZE) {
                        int x = (mousePos.x - BOARD2_OFFSET_X) / TILE_SIZE;
                        int y = (mousePos.y - BOARD_OFFSET_Y) / TILE_SIZE;
                        board2[x][y] = !board2[x][y]; // Alternar casilla
                    }
                }
            }
        }

        window.clear(Color::White);

        drawBoard(window, board1, BOARD_OFFSET_X, BOARD_OFFSET_Y, font, false);
        drawBoard(window, board2, BOARD2_OFFSET_X, BOARD_OFFSET_Y, font, true);

        for (auto& ship : ships) {
            window.draw(ship.shape);
        }

        window.draw(finalizeButton);
        window.draw(buttonText);

        window.display();
    }

    return 0;
}
