#include <iostream>
#include <string>
using namespace std;

struct Jugador  {  // Declarar struct  para usar en 2 jugadores 
    int vidas=0;
    string fila[8] = {"A", "B", "C", "D", "E", "F", "G", "H"}; //Las filas disponibles del tablero
    string columna[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};//Lo mismo pero columnas
    string matriz[9][9]; //Matriz es 9x9 porque fila y columna ocupan un espacio
    string disparos[9][9];//Matriz mascara para que el jugador no pueda ver el tablero del rival 

    Jugador() {
        vidas =0;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                matriz[i][j] = " ";
                disparos[i][j] = " ";
            }
        }
        for (int i = 1; i < 9; i++) {
            matriz[0][i] = columna[i-1];
        }
        for (int j = 1; j < 9; j++) {
            matriz[j][0] = fila[j-1];
            disparos[j][0] = fila[j-1];
        }
    }
    //Llama una funcion que pone los caracteres de fila y columna en sus lugares y el resto vacio cada vez que declaras una variable del struct jugador
};

void dibujar(string jugador[9][9]) { //dibuja matriz
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cout << jugador[i][j] << " ";
        }
        cout << endl;
    }
}

int checkear(int fila, int columna, string jugador[9][9], string disparos[9][9], int& vidas) {//Funcion que mira si le acertaste a una parte del barco
    if (jugador[fila][columna] == "B" && disparos[fila][columna] == "X" ){
        
        cout<<"Esta casilla ya fue disparada"<<endl;
        return vidas;
    }
    
    
    else if (jugador[fila][columna] == "B") {
        cout << "Le acertaste a un barco" << endl;
        disparos[fila][columna] = "X";//Si acertas pones una X a la matriz mascara y restas una vida del otro jugador
        vidas--;
        if (vidas == 0) {
            cout << "Te moriste" << endl;
            return 0;
        } else {
            return vidas;
        }
    } else {
        cout << "Fallaste!" << endl;
        disparos[fila][columna] = "O";//Sino pones una O en la matriz mascara 
        return vidas;
    }
}

void disparar(string columna[], string fila[], string jugador[9][9], string disparos[9][9], int& vidas, bool turno) {
    string disparo;//String de 2 espacios para la fila y columna
    cout << "Ingrese la fila y columna a la que va a disparar: ";
    cin>> disparo;

    if (disparo.length() != 2) {
        cout << "No ingresaste 1 caracter y letra, vuelva a intentarlo" << endl;//Si no se ingresan 2 Caracteres se vuelve a llamar la funcion
        disparar(columna, fila, jugador, disparos, vidas, turno);
        return;
    }

    char fil = disparo[0]; //Guarda la fila del disparo en un char
    char col = disparo[1];//Guarda la columna del disparo

    bool filaValida = false;
    bool colValida = false;
    int indiceF = -1, indiceC = -1;//Comprobacion para ver si que lo ingresaste esta dentro del rango de la matriz
    for (int i = 0; i < 8; i++) {
        if (fil == fila[i][0]) {
            filaValida = true;
            indiceF = i + 1;
        }
        if (col == columna[i][0]) {
            colValida = true;
            indiceC = i + 1;
        }
    }

    if (!filaValida || !colValida) {
        cout << "No se encontro la fila o columna" << endl;
        disparar(columna, fila, jugador, disparos, vidas, turno);
        return;
    }

    cout << "Disparo registrado en la fila " << fil << " y columna " << col << endl;
    checkear(indiceF, indiceC, jugador, disparos, vidas);//Revisar si el disparo
    cout << "Vidas restantes del jugador: "<<turno<<"  "<< vidas << endl;
}

void poner_barco(string jugador[9][9], string barco[], int barcoSize, string fila[], string columna[]) {
    
    //funcion para posicionar los barcos ingresando la primera posicion del primer caracter del string de barco
    cout << "El barco que vas a poner ocupa " << barcoSize << " casillas" << endl;
    bool rotar;
    int num;
    cout << "Quiere rotar el barco verticalmente? (1 si, 0 no): ";
    cin >> num;
    rotar = num == 1;

    string inicio;
    cout << "Ingrese la fila y columna en la que va a poner el inicio del barco: ";
    cin >> inicio;

    if (inicio.length() != 2) {
        cout << "No ingresaste 1 caracter y letra, vuelva a intentarlo" << endl;
        poner_barco(jugador, barco, barcoSize, fila, columna);
        return;
    }

    char row = inicio[0];
    char col = inicio[1];

    bool filaValida = false;
    bool colValida = false;//Comprobacion si todos los caracteres del barco pueden entrar en la matriz
    int indiceF = -1, indiceC = -1;
    for (int i = 0; i < 8; i++) {
        if (row == fila[i][0]) {
            filaValida = true;
            indiceF = i + 1;
        }
        if (col == columna[i][0]) {
            colValida = true;
            indiceC = i + 1;
        }
    }

    if (!filaValida || !colValida) { //Si esta fuera del rango se vuelve a llamar la funcion
        cout << "Fila o columna no válidas, vuelva a intentarlo" << endl;
        poner_barco(jugador, barco, barcoSize, fila, columna);
        return;
    }

    if ((indiceF + barcoSize > 9 && rotar) || (indiceC + barcoSize > 9 && !rotar)) {
        cout << "El barco no cabe en esa posicion, vuelva a intentarlo" << endl;
        poner_barco(jugador, barco, barcoSize, fila, columna);
        return;
    }

    for (int i = 0; i < barcoSize; i++) {//Posiciona los caracteres en la matriz dependiendo si se eligio rotar o no 
        if (rotar) {
            jugador[indiceF + i][indiceC] = barco[i];
        } else {
            jugador[indiceF][indiceC + i] = barco[i];
        }
    }

    cout << "Barco colocado en la fila " << row << " y columna " << col << endl;
    dibujar(jugador);
}



void vidas(string jugador[9][9],int& vidas){//int& vidas hace referencia a vidas que se paso como atributo
    //Funcion para añadir vidas dependiendo de la cantidad de partes de barco 
    cout<<vidas;
    for(int j=1;j<9;j++){ 
        for(int i=0;i<9;i++){
            if(jugador[i][j] == "B"){
             vidas ++;
                cout<<vidas;
            }
        } 
    }
    
}




void armar_campo(Jugador& jugador) {
    //Todos los barcos disponibles
    string destructor[] = {"B", "B"};
    string submarino[] = {"B", "B", "B"};
    string crucero[] = {"B", "B", "B"};
    string acorazado[] = {"B", "B", "B", "B"};
    string carrier[] = {"B", "B", "B", "B", "B"};

    int barcos[] = {2, 3, 3, 4, 5};//Tamaño de los barcos en numeros
    string* tipos[] = { destructor,submarino,crucero, acorazado, carrier};//String* se usa para agrupar varios strings de distintos tamaños en uno solo

    for (int i = 0; i < 1; i++) {
        poner_barco(jugador.matriz, tipos[i], barcos[i], jugador.fila, jugador.columna);//Posicionar los barcos de manera individual por cada uno
    }
}

void turno(Jugador& jugador1, Jugador& jugador2) {
    bool turno = true;//si turno es verdadero es el turno del jugador 1 si es falso es jugador 2
    int turnos = 0;//cantidad de turnos de la partida

    armar_campo(jugador1);
    vidas(jugador1.matriz,jugador1.vidas);
    armar_campo(jugador2);
    vidas(jugador2.matriz,jugador2.vidas);

    while (jugador1.vidas > 0 && jugador2.vidas > 0) {
        turnos++;
        cout << "Estamos en el turno: " << turnos << endl;

        if (turno) {
            cout << "Turno del Jugador 1" << endl;
            cout << "Tablero del Jugador 1:" << endl;
            dibujar(jugador1.matriz);
            cout << "Tablero de disparos del Jugador 1:" << endl;
            dibujar(jugador1.disparos);
            disparar(jugador2.columna, jugador2.fila, jugador2.matriz, jugador1.disparos, jugador2.vidas, turno);//Disparar al tablero del otro jugador
        } else {
            cout << "Turno del Jugador 2" << endl;
            cout << "Tablero del Jugador 2:" << endl;
            dibujar(jugador2.matriz);
            cout << "Tablero de disparos del Jugador 2:" << endl;
            dibujar(jugador2.disparos);
            disparar(jugador1.columna, jugador1.fila, jugador1.matriz, jugador2.disparos, jugador1.vidas, turno);//Disparar al tablero del otro jugador
        }

        turno = !turno;
    }

    if (jugador1.vidas == 0) {
        cout << "El jugador 2 gano" << endl;
    } else {
        cout << "El Jugador 1 gano!" << endl;
    }
}

int main() {
    Jugador jugador1;//declarar jugadores usando el struct
    Jugador jugador2;

    cout << "Hola, estas jugando a batalla naval" << endl;
    turno(jugador1, jugador2);

    return 0;
}
