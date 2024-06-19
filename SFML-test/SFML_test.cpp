#include <SFML/Graphics.hpp>
#include <vector>
#include <windows.h> // Incluir el encabezado para usar MessageBoxA

using namespace sf;

const int SIZE = 10; // Tamaño del tablero 10x10 (más una fila y columna para etiquetas)
const int TILE_SIZE = 30; // Tamaño de cada casilla
const int BOARD_OFFSET_X = 50; // Desplazamiento del primer tablero
const int BOARD_OFFSET_Y = 50;
const int BOARD2_OFFSET_X = BOARD_OFFSET_X + (SIZE + 1) * TILE_SIZE + 50; // Desplazamiento del segundo tablero

// Función para dibujar un tablero
void drawBoard(RenderWindow& window, const std::vector<std::vector<bool>>& board, int offsetX, int offsetY, Font& font) {
    for (int i = 0; i <= SIZE; ++i) {
        for (int j = 0; j <= SIZE; ++j) {
            RectangleShape tile(Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setPosition(offsetX + i * TILE_SIZE, offsetY + j * TILE_SIZE);
            tile.setOutlineColor(Color::Black);
            tile.setOutlineThickness(1);

            if (i == 0 || j == 0) {
                tile.setFillColor(Color::White);
            } else if (board[i - 1][j - 1]) {
                tile.setFillColor(Color::Blue); // Casilla con barco
            } else {
                tile.setFillColor(Color::White); // Casilla vacía
            }

            window.draw(tile);
        }
    }

    // Dibujar números y letras
    for (int i = 1; i <= SIZE; ++i) {
        Text number;
        number.setFont(font);
        number.setString(std::to_string(i));
        number.setCharacterSize(16);
        number.setFillColor(Color::Black);
        number.setPosition(offsetX + i * TILE_SIZE + TILE_SIZE / 2 - 5, offsetY - TILE_SIZE / 2);

        Text letter;
        letter.setFont(font);
        letter.setString(static_cast<char>('A' + i - 1));
        letter.setCharacterSize(16);
        letter.setFillColor(Color::Black);
        letter.setPosition(offsetX - TILE_SIZE / 2 - 10, offsetY + i * TILE_SIZE + TILE_SIZE / 2 - 10);

        window.draw(number);
        window.draw(letter);
    }
}

int main() {
    // Crear la ventana
    RenderWindow window(VideoMode(800, 600), "Batalla Naval");

    // Cargar la fuente
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // Mostrar un cuadro de diálogo si no se puede cargar la fuente
        MessageBoxA(NULL, "No se pudo cargar la fuente", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Inicializar los tableros
    std::vector<std::vector<bool>> board1(SIZE, std::vector<bool>(SIZE, false));
    std::vector<std::vector<bool>> board
