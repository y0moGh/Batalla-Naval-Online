#include <iostream>
#include <string>
#include <vector>
#include <conio.h>

using namespace std;

// Colores
string red = "\033[1;31m";
string blue = "\033[1;34m";
string reset = "\033[0m";
string green = "\033[1;32m";
string orange = "\033[38;5;208m";
string yellow = "\033[38;5;226m";

// Variables globales
vector<string> letras = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
vector<int> boats_size = {2, 3, 3, 4, 5}; //Tamaño de los barcos en numeros

struct Jugador {
    int vidas = 3;
    vector<int> position = {4, 4};
    vector<vector<string>> board = vector<vector<string>>(9, vector<string>(9, " "));
    vector<vector<string>> shots_board = vector<vector<string>>(9, vector<string>(9, " "));
};

void load_board(Jugador& jugador) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Cargamos la primera fila de la matriz con numeros del 1 al 8
            if (i == 0 && j != 0) {
                jugador.board[i][j] = to_string(j);
                jugador.shots_board[i][j] = to_string(j);
            }
            // Cargamos la primera columna con las letras A-I
            else if (j == 0 && i != 0) {
                jugador.board[i][j] = letras[i - 1];
                jugador.shots_board[i][j] = letras[i - 1];
            }
        }
    }
}

void print_selection_board(bool vertical, int n, Jugador& jugador) {
    int a = jugador.position[0];
    int b = jugador.position[1];
    int cont = boats_size[n];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Colores en la primera fila y columna
            if ((i == 0 && j != 0) || (j == 0 && i != 0)) {
                cout << blue << jugador.board[i][j] << reset << " | ";
            }
            // Imprimir el barco que está seleccionando el jugador
            else if (a == i && b == j && cont > 0) {
                cout << yellow << "B" << reset << " | ";
                if (!vertical) b++;
                else a++;
                cont--;
            } else {
                cout << jugador.board[i][j] << " | ";
            }
        }
        cout << endl;
        cout << "-----------------------------------" << endl;
    }
}

bool put_boat(bool vertical, int n, Jugador& jugador) {
    int r = jugador.position[0];
    int c = jugador.position[1];

    // Verificar si el barco cabe en la posición actual
    for (int i = 0; i < boats_size[n]; i++) {
        if (r >= 9 || c >= 9 || jugador.board[r][c] == "B") return false; // Hubo un problema
        if (vertical) r++;
        else c++;
    }

    // Colocar el barco en la posición actual
    r = jugador.position[0];
    c = jugador.position[1];
    for (int i = 0; i < boats_size[n]; i++) {
        jugador.board[r][c] = "B";
        if (vertical) r++;
        else c++;
    }
    return true; // Salió todo bien
}

bool move_selection_player(char key, Jugador& jugador, bool& vertical, int n) {
    int new_row = jugador.position[0];
    int new_col = jugador.position[1];
    bool check;

    if (key == 'w' && jugador.position[0] > 1) new_row--;
    else if (key == 'a' && jugador.position[1] > 1) new_col--;
    else if (key == 's') {
        if (!vertical && jugador.position[0] < 8) new_row++;
        else if (vertical && jugador.position[0] < 8 - boats_size[n] + 1) new_row++;
    } else if (key == 'd') {
        if (!vertical && jugador.position[1] < 8 - boats_size[n] + 1) new_col++;
        else if (vertical && jugador.position[1] < 8) new_col++;
    } else if (key == 'e') {
        check = put_boat(vertical, n, jugador);
        return !check;
    } else if (key == 'r') vertical = !vertical;

    jugador.position[0] = new_row;
    jugador.position[1] = new_col;
    return true;
}

void print_board(vector<vector<string>> board, string color){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            // Colores en la primera fila y columna
            if ((i == 0 && j != 0) || (j == 0 && i != 0)) {
                cout << color << board[i][j] << reset << " | ";
            }
            else if(board[i][j] == "B") cout << yellow << board[i][j] << reset << " | ";
            else {
                cout << board[i][j] << " | ";
            }
        }
        cout << endl;
        cout << "-----------------------------------" << endl;
    }
}

void selection_stage(Jugador& jugador) {
    bool vertical = false;
    bool selecting;

    for (int i = 0; i < boats_size.size(); i++) {
        selecting = true;
        while (selecting) {
            print_selection_board( vertical, i, jugador);
            char key = _getch();
            selecting = move_selection_player(key, jugador, vertical, i);
            system("cls");
        }
    }
        jugador.position = {4, 4};
}

void print_shots_board(Jugador& jugador){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            // Colores en la primera fila y columna
            if ((i == 0 && j != 0) || (j == 0 && i != 0)) {
                cout << red << jugador.shots_board[i][j] << reset << " | ";
            }
            else if(jugador.shots_board[i][j] == "X") cout << orange << jugador.shots_board[i][j] << reset << " | ";
            // Imprimir el barco que está seleccionando el jugador
            else if(jugador.position[0] == i && jugador.position[1] == j){
                cout << yellow << "." << reset << " | ";
            }
            else {
                cout << jugador.shots_board[i][j] << " | ";
            }
        }
        cout << endl;
        cout << "-----------------------------------" << endl;
    }
}

bool move_shot(Jugador& jugador, char key){
    int new_row = jugador.position[0];
    int new_col = jugador.position[1];

    if (key == 'w' && jugador.position[0] > 1) new_row--;
    else if (key == 'a' && jugador.position[1] > 1) new_col--;
    else if (key == 's' && jugador.position[0] < 8) new_row++;
    
    else if (key == 'd' && jugador.position[1] < 8) new_col++;
    else if (key == 'e'){
        jugador.shots_board[new_row][new_col] = "X";
        return false;
    }

    jugador.position[0] = new_row;
    jugador.position[1] = new_col;
    return true;
}

void shoot_boats(Jugador& jugador){
	bool shooting = true;
	
    while(shooting){
        print_board(jugador.board, blue);
        cout << endl << endl << endl << endl;
        print_shots_board(jugador);
        char key = _getch();
        shooting = move_shot(jugador, key);
        system("cls");
    }
}

int main() {
    Jugador jugador;
    load_board(jugador); // Cargo el tablero con las filas y columnas principales
    selection_stage(jugador);
    shoot_boats(jugador);
}
