#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Declaraciones de variables globales de turno
extern bool turno;
extern bool first;
extern string valido;
extern string podio;

// Declaraciones de variables globales de juego
extern vector<string> letras;
extern vector<int> boats_size;
extern string jugador2_vida;
extern string jugador2_user;

struct Jugador {
    int vidas;
    vector<int> position;
    vector<vector<string>> board;
    vector<vector<string>> shots_board;

    Jugador()
        : vidas(17), 
          position({4, 4}),
          board(vector<vector<string>>(9, vector<string>(9, " "))),
          shots_board(vector<vector<string>>(9, vector<string>(9, " ")))
    {}
};

// Declaraciones de colores
extern string red;
extern string blue;
extern string reset;
extern string green;
extern string orange;
extern string yellow;
extern string f_yellow;
extern string f_gray;
extern string f_red;

// Imagenes ascii
extern string barco_imagen;
extern string moriste;
extern string ganaste;

#endif // GLOBALS_H
