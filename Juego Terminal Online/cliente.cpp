
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <conio.h>
#include <ctime>
// Enlaza la biblioteca Winsock 2.2 para permitir el uso de funciones de red
#pragma comment(lib, "ws2_32.lib")
// Define el puerto en el que el cliente se conectara y el tamanio del buffer de recepcion
#define PORT 1234
#define BUFFER_SIZE 1024
using namespace std;
bool turno = false;
bool first = false;
string valido="";
// Colores
string red = "\033[1;31m";
string blue = "\033[1;34m";
string reset = "\033[0m";
string green = "\033[1;32m";
string orange = "\033[38;5;208m";
string yellow = "\033[38;5;226m";
string f_yellow = "\033[48;5;226m";
string barco_imagen = R"(                                                                                         
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                =                                         
                                               :+ :                                       
                                               :+.:                                       
                                               :=..                                       
                                          :-    =.    -                -                  
                                           =   .+==-  =                *-                 
                                          .+  .+-*+-::=               .=:                 
                                       . .:+==+***++**+                -=                 
                                     .:-   +.  =-*+::=*.   :          .-=                 
                                      :-:. =+-+*=*+-::*=:::+...--.    .-=                 
                                     ---==-=========**+=====--===++.  .-+                 
                                    ==++=+*=##*#***=*+#@#**+%%*%****-  +=                 
                                  :=+=+#*+*=+*+*+++++++++++#***#*##*..:+*+-               
                                 :..===:.-=::*-::+::+-:--:---==+*+=+*##%%#+---:::.        
                             .  .=+:.....:--:-::-+::+-:--:--++*##***********#%***+-       
                            :#+++. . ....::=-:::=*:-+===++****+++++++++++***#**++-        
                           -=*#%+.   ...::=:-====+===+%+==+++++++++++++*******++:         
                           =#%%%+  . ::.:-----=**=++======+++++++++++++******+=.          
                         :.  =*#=  ....::::----=========+++++++++++++++*****+-            
                   :::-=====+*=*+ ....:::----=========+#####+++++++++++****+:             
                 -#=###+++*++=-:..:.:::::----=========+*@@@@*+++++++++****=               
                 %.-%%++***=... ....::::----=========+++#%%#**+++++++****+-               
                 % .+%+**##+  .....::::----========++++++*#+++++++++***+ .+               
                :%---=+==-:.......:::-=---=====+++++++++*#%**+++++++**: :-#=              
                -#   =:... .....::::----===++++++++**+***%%%*+**++++*.  .#@*.             
            ..:.-==-+= ........:::---=-=+++++++++++++++++***+======-=   .::.              
           +:::::............::::-----=============================-+                     
           :-------------===++++****####%%%%%%%@@@@@@@@@@@@%%%%%%%%@@=-==-                
                -*=+%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%@@@%%%%+               
                :+   =@%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%@@@@@*               
                .:    =%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:               
                        :-=++***##%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                
                                          .:::----====++**##%%@@@@@@@@%+.                 
                                                                 .:::.                                                                                                            
)";
string moriste = R"(                                                                                          
             .:::.                                                     ::----:            
          .--::::---.                                                :--:::::--           
         :--::...::---                                             .---:....:-=.          
        .=--:.....::--=.                                          .=--::....:-:::..       
     ..:---::::....::--=           ..::-------------::..          ==--:::::::...:::::.    
  .--::::...:::::::::--=:     .:---------------------------:.    .==--:::::.........::-   
 .--::.........::::::---= .:------:::::::::::::::::::::--------: :=---::::....  ...::--:  
 .=-::....   ....:::----------::::......................::::----------:::.........::--=:  
  :=--::.........::-------::::.......             .........::::------:::........::--==-   
    -==---------::------::::.....                       ......::::-----::::::::--====.    
      .-========------::::....                             .....::::----::---===+=:.      
          .:::::----::::....                                 ....:::::---:---::.          
               :---::::....                                   ....::::----:               
              :---:::::....                                    ....::::----:              
              ----:::::....                                     ...::::-----:             
             .----::::.....                                     ....:::------             
             :----:::.......                                   ......:::-----             
             :---::::.......                                   .......::-----             
             .---:::.............:::..              .-====-:..........::---=-             
              ---:::...... :=*%@@@@@@@%*:        .+%@@@@@@@@@%*- .....:::--=:             
              :--:::.... =%@@@@@@@@@@@@@@-      .#@@@@@@@@@@@@@@%-....:::---              
               --::::...#@@@@@%%%%%%%%%@@#      -@@@%%%%%%%%%%@@@@-.:::::-=.              
               .-::::::-@@@%%%%%%%%%%%%%@#      -@%%%%%%%%%%%%%%@@%.:::::--               
                :-:::::=@%%%%%%######%%%@+.......%%%%########%%%%%@.:::---                
                 ---::::%%%%%#########%%%........=%%%###########%%*:::---.                
                  --:::.=%###****####%%#:.........-#%##***+++***##::::--.                 
                  --::::.-****+****##+-...      ....-+***++++++*+:::::--                  
                  --::.....--====--:.....  .-=:   .....:-======:.:::::-=                  
                  :--::.......::::::.....:#@@@@%= ....:::::::::::::::--=                  
                   -=--:::::::::::::....:%%%%#%@@=...::::::::::::::--==.                  
                  .-====--------:::::::::-==-:-==:::::::::-------====-                    
              ..:---------=======--:::::::::::::::::::::---===----:::--:.                 
        .::-==-----------===+=-===--::::::::::::::::::-------==---:::::---====--:.        
     .:--::::-----------====:  -===---::::::::::::::-------::==---:::::::::::::::---:     
    :-::..:::::::::::--===:    :====-----------------===---.  -=---:::::::::.......::-:   
   -::.....::::::::::-===       ========--=======--=====--:    .---:::...::::........::-  
  :-::....::::::...::-==-       -----:.:-:..:-:.:--::--:--:      =-::.......::::.....::-: 
  .=-:::::::::.....:--==:       :=---:..:....:...::::-----       :--::....  ...::::::--=  
   .==-----::......:--==.         ..::::::..:-::::-:-::.          --:::....   ..:-=---:   
     .:-==-:......::--=-                                          :--::........:-:        
          :-:....::--=-                                            :--:::....::--         
           -::::::--=:                                              .---:::::---.         
           .-------:                                                  .:---==-:           
             .....                                                                        
)";

string ganaste = R"(
                              %%##########################%%                              
                              @=                          =@                              
                         #####@=            :-            -@###%#                         
                         @+  .@=           :@@+           -@:  +%                         
                         #%   @+         .-@+-@+..        =@.  %*                         
                         .@=  #%     -%@#**-  :**#@@#     *%  =@.                         
                          =@+ -@:     :*#-      :#%-     .@= =@:                          
                           :%#:##       -@:     @*       *%:*%.                           
                             =%@@+      =%:=##+-*%      -@%#-                             
                               :+@+     #@*=. :+%@.    =@=.                               
                                 :@*.                 +%:                                 
                                   *@=.             =%+                                   
                                    .+@#=.      .-*%+.                                    
                                       :+*@=  -%*=:                                       
                                          @%  *%                                          
                                          @%  *%                                          
                                          @%  *%                                          
                                          @%  *%                                          
                                     #%###@%%%%@%%%%#                                     
                                     @#            *%                                     
                                .++++@%++++++++++++%@++++                                 
                                :@*::::::::::::::::::::=@.                                
                                :@+                    -@.                                
                                :@+                    -@.                                
                       +*##*##*##@%**#**#*##*##*##*##*#%@##*##*##**                       

)";

// Variables globales
vector<string> letras = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
vector<int> boats_size = {2, 3, 3, 4, 5}; //Tamanio de los barcos en numeros

string jugador2_vida = "17";
string jugador2_user = "";

struct Jugador {
    int vidas = 17;
    vector<int> position = {4, 4};
    vector<vector<string>> board = vector<vector<string>>(9, vector<string>(9, " "));
    vector<vector<string>> shots_board = vector<vector<string>>(9, vector<string>(9, " "));
};

void delay(int secs) {
  for(int i = (time(NULL) + secs); time(NULL) != i; time(NULL));
}
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
            // Imprimir el barco que esta seleccionando el jugador
            else if (a == i && b == j && cont > 0) {
                cout << f_yellow << " " << reset << " | ";
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
    // Verificar si el barco cabe en la posicion actual
    for (int i = 0; i < boats_size[n]; i++) {
        if (r >= 9 || c >= 9 || jugador.board[r][c] == "B") return false; // Hubo un problema
        if (vertical) r++;
        else c++;
    }
    // Colocar el barco en la posicion actual
    r = jugador.position[0];
    c = jugador.position[1];
    for (int i = 0; i < boats_size[n]; i++) {
        jugador.board[r][c] = "B";
        if (vertical) r++;
        else c++;
    }
    return true; // Salio todo bien
}

void print_APB() {
	char option;
    
    cout << yellow << " _________________________________________" << endl
                    << "|                  APB                    |" << endl
                    << "|_________________________________________|" << endl
                    << "|    Movimientos:                         |" << endl
                    << "|        -w: moverse hacia arriba         |" << endl
                    << "|        -a: moverse hacia la izquierda   |" << endl
                    << "|        -s: moverse hacia abajo          |" << endl
                    << "|        -d: moverse hacia la derecha     |" << endl
                    << "|        -e: seleccionar la casilla       |" << endl
                    << "|        -r: rotar el barco 90?           |" << endl
                    << "|_________________________________________|" << endl
                    << "|   -Presione una tecla si quiere salir   |" << endl
                    << "|_________________________________________|" << reset << endl;
	option = _getch();

    system("cls");
}

void reglas() {
	char option;

    cout << yellow << " ________________________________________________ " << endl
                    << "|                    Reglas                      |" << endl
                    << "|________________________________________________|" << endl
                    << "|                                                |" << endl
                    << "|   -Comienzo del juego:                         |" << endl
                    << "|        1. Cada jugador debera colocar sus      |" << endl
                    << "|       barcos en las casillas que deseen (de    |" << endl
                    << "|       forma vertical u horizontal).            |" << endl
                    << "|        2. Al terminar de colocar los barcos se |" << endl
                    << "|       esperara a que el otro jugador tambien   |" << endl
                    << "|       los coloque en las casillas deseadas.    |" << endl
                    << "|                                                |" << endl
                    << "|   -Combate:                                    |" << endl
                    << "|        1. al comienzo del combate cada jugador |" << endl
                    << "|       seleccionara la casilla que desee atacar |" << endl
                    << "|       y esta sera marcada con una X.           |" << endl
                    << "|        2. Si la casilla seleccionada contiene  |" << endl
                    << "|       un barco se le restara una vida al       |" << endl
                    << "|       jugador contrario.                       |" << endl
                    << "|        3. Si un barco es hundido, o una de sus |" << endl
                    << "|       casillas es golpada, se le informara a   |" << endl
                    << "|       ambos jugadores de esta accion.          |" << endl
                    << "|                                                |" << endl
                    << "|   -Fin del juego:                              |" << endl
                    << "|        1. El juego sera finalizado una vez uno |" << endl
                    << "|       de los dos jugadores se haya quedado sin |" << endl
                    << "|       vidas, es decir, sin barcos.             |" << endl
                    << "|________________________________________________|" << endl
                    << "|   -Presione una tecla si quiere salir          |" << endl
                    << "|________________________________________________|" << reset << endl;
	option = _getch();
}

bool menu() {
    while (true) {
        char opcion;
	//imprimir barco en ascii
	cout<<barco_imagen<<endl;
        cout << green << "---------Estas jugando battalla naval---------" << reset << endl;
        cout << orange << "-Seleccione una opcion:" << endl
                       << "\t 1. Jugar" << endl
                       << "\t 2. Reglas" << endl
                       << "\t 3. APB" << endl 
                       << "\t 4. Cerrar el programa" <<reset << endl;
        opcion = _getch();
        system("cls");
        if (opcion == '1') return true;
        else if (opcion == '2') reglas();
        else if (opcion == '3') print_APB();
        else if (opcion == '4') return false;
        else                  cout << red << "[!] Ingrese una opcion correcta (1, 2, 3 o 4)" << reset << endl;
    }
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
                cout << color << " " << board[i][j] << reset << " |";
            }
            else if(board[i][j] == "B") cout << f_yellow << "   " << reset << "|";
            else if(board[i][j] == "X") cout << orange << " " << board[i][j] << reset << " |";
            else                        cout << " " << board[i][j] << " |";

        }
        cout << endl;
        cout << "-----------------------------------" << endl;
    }
}

void selection_stage(Jugador& jugador) {
    bool vertical = false;
    bool selecting;
    if (menu()) {
        for (int i = 0; i < boats_size.size(); i++) {
            selecting = true;
            while (selecting) {
                print_selection_board( vertical, i, jugador);
                char key = _getch();
                selecting = move_selection_player(key, jugador, vertical, i);
                system("cls");
            }
        }
    }
    else exit(0);
    
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
            // Imprimir el barco que esta seleccionando el jugador
            else if(jugador.position[0] == i && jugador.position[1] == j){
                if (jugador.shots_board[i][j] == "X") cout << red << jugador.shots_board[i][j] << reset << " | ";
                else                                  cout << red << "." << reset << " | ";
            }
            else {
                cout << jugador.shots_board[i][j] << " | ";
            }
        }
        cout << endl;
        cout << "-----------------------------------" << endl;
    }
}

bool move_shot(Jugador& jugador, char key, string& shot){
    int new_row = jugador.position[0];
    int new_col = jugador.position[1];
    if (key == 'w' && jugador.position[0] > 1) new_row--;
    else if (key == 'a' && jugador.position[1] > 1) new_col--;
    else if (key == 's' && jugador.position[0] < 8) new_row++;
    else if (key == 'd' && jugador.position[1] < 8) new_col++;
    else if (key == 'e'){
    	if(jugador.shots_board[new_row][new_col] != "X"){
    		jugador.shots_board[new_row][new_col] = "X";
        	shot = to_string(new_row) + to_string(new_col);
        	return false;
		}
    }
    jugador.position[0] = new_row;
    jugador.position[1] = new_col;
    return true;
}

string shoot_boats(Jugador& jugador){
    bool shooting = true;
    string shot;
    while(shooting){
      system("cls");
      cout << endl << "[...] Jugando contra: " << jugador2_user << endl << endl;
        cout << "[+] Tu vida: " << jugador.vidas << endl;
        print_board(jugador.board, blue);
        cout << endl << endl << endl << endl;
        cout << "[+] Vida de " << jugador2_user << ": " << jugador2_vida << endl;
        print_shots_board(jugador);
        char key = _getch();
        shooting = move_shot(jugador, key, shot);
    }
    return shot;
}

bool send_message(SOCKET& client_socket, WSADATA& wsaData, string message){
      int send_result = send(client_socket, message.c_str(), message.size(), 0);
      if (send_result == SOCKET_ERROR) {
          cerr << "Send failed: " << WSAGetLastError() << endl;
          return false;
      }
      return true;
}

// Funcion para recibir un mensaje del servidor y asignar los valores al vector player2_shot
bool receive_shot(SOCKET& client_socket, WSADATA& wsaData, vector<string>& player2_shot) {
    // Prepara un bufer para recibir la respuesta del servidor
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0); // Recibe datos del servidor
    if (bytes_received <= 0) {
        cerr << "Recv failed or connection closed: " << WSAGetLastError() << endl;
        return false;
    }
    // Asegurate de que el buffer esta terminado en null para usarlo como string
    buffer[bytes_received] = '\0';
    // Convierte el buffer a string
    string received(buffer);
    
    // Asegurate de que player2_shot tenga el tamanio adecuado
    if (received.size() >= 2) {
        player2_shot.resize(2);
        player2_shot[0] = received.substr(0, 1); // Primer caracter
        player2_shot[1] = received.substr(1, 1); // Segundo caracter
    }
	
    return true;
}

bool receive_life(SOCKET& client_socket, WSADATA& wsaData, string& jugador2_vida) {
    // Prepara un buffer para recibir la respuesta del servidor
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0); // Recibe datos del servidor
    if (bytes_received <= 0) {
        cerr << "Recv failed or connection closed: " << WSAGetLastError() << endl;
        return false;
    }
    buffer[bytes_received] = '\0';
    string vida(buffer);
    jugador2_vida = vida;
    
    return true;
}

// Funci?n para obtener el mensaje despu?s de "user1\n"
string get_user(const string& received, string prefix) {
    size_t pos = received.find(prefix);

    if (pos != string::npos) {
        return received.substr(pos + prefix.length());
    } else {
        return ""; // Retornar cadena vac?a si no se encuentra al usuario
    }
}

// Funcion para recibir un mensaje del servidor y asignar los valores al vector player2_shot
bool receive_msg(SOCKET& client_socket, WSADATA& wsaData) {
    // Prepara un buffer para recibir la respuesta del servidor
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0); // Recibe datos del servidor
    if (bytes_received <= 0) {
        cerr << "Recv failed or connection closed: " << WSAGetLastError() << endl;
        return false;
    }
    // Asegurate de que el buffer esta terminado en null para usarlo como string
    buffer[bytes_received] = '\0';
    // Convierte el buffer a string
    string received(buffer);
    
    // Para first y para second recibimos el usuario del otro jugador
    if(received == "first\n") {
    	turno = true;
        first = true;
	}
    else if(received == "valido") valido = received;
    // El usuario del primer turno va a recibir user2 mientras que el del segundo turno, user1
    else if(received.find("user1\n") != string::npos){
    	jugador2_user = get_user(received, "user1\n");
	}
	else if(received.find("user2\n") != string::npos){
		jugador2_user = get_user(received, "user2\n");
	}
}

void loggin(SOCKET& client_socket, WSADATA& wsaData) {
    string nombre, contrasenia, creds;
    bool success;
    while(valido != "valido"){
        // logearse/registrarse
        system("cls");
        cout << "[i] Ingrese su nombre de usuario: "; cin >> nombre;
        cout << "[i] Ingrese su contrasenia: "; cin >> contrasenia;
        creds = "creds\n" + nombre + "\n" + contrasenia;
        success = send_message(client_socket, wsaData, creds);
        success = receive_msg(client_socket, wsaData);
    }
}   

// Funcion principal del juego
void playing(Jugador& jugador, SOCKET& client_socket, WSADATA& wsaData) {
    bool success;
    vector<string> player2_shot = {"0", "0"};
    // In game
    while (jugador.vidas != 0 && jugador2_vida != "0") {
        
        // Turno del jugador
        if (turno) {
            string shot = shoot_boats(jugador); // Disparo
            // Envia el disparo al servidor
            success = send_message(client_socket, wsaData, shot);
            if (!success) break; // Sale del bucle si hay algun error        
            // Envia la vida al servidor
            string vida = to_string(jugador.vidas);
            success = send_message(client_socket, wsaData, vida);
            if (!success) break; // Sale del bucle si hay algun error      
            turno = false;
        }
        system("cls");
        cout << endl << "[...] Esperando turno" << endl << endl;
        print_board(jugador.board, blue);
        cout << endl << endl << endl << endl;
        print_board(jugador.shots_board, red);
        
        // Recibe el disparo del otro jugador 
        success = receive_shot(client_socket, wsaData, player2_shot);
        if (!success) break;
        // Chequeamos si el disparo fue acertado
        int a = stoi(player2_shot[0]);
        int b = stoi(player2_shot[1]);
        
        if(jugador.board[a][b] == "B"){
            jugador.vidas -= 1;
        }
        
        // Recibe la vida del otro jugador
        success = receive_life(client_socket, wsaData, jugador2_vida);
        if (!success) break;
        cout << jugador2_vida;
        
        // Verifica si el juego ha terminado despu?s de recibir la vida del otro jugador
        if (jugador.vidas == 0 || jugador2_vida == "0") {
         string shot = {"00"};
        	 success = send_message(client_socket, wsaData, shot);
            // Envia la vida al servidor para notificar que el jugador perdi? 
            string vida = to_string(jugador.vidas);
            send_message(client_socket, wsaData, vida);
            break; // Sale del bucle mientras
        }
        
        turno = true;
    }
    
    char option;
    system("cls");
    if (jugador.vidas == 0) {
        cout << "Perdiste la partida" << endl;
        cout << moriste << endl;
        option = _getch();
        system("cls"); 
    } else {
        cout << "Ganaste la partida" << endl;
        cout << ganaste;
        option = _getch();
        system("cls");
    }
    menu();
}

void start_client(Jugador& jugador) {
    WSADATA wsaData;
    // Inicializa la biblioteca Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return;
    }
    // Crea un socket para el cliente
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }
    // Configura la direccion del servidor al que se conectara el cliente
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // Familia de direcciones IPv4
    server_addr.sin_addr.s_addr = inet_addr("34.176.129.197"); // Direccion IP del servidor (local)
    server_addr.sin_port = htons(PORT); // Puerto del servidor
    // Conecta el socket del cliente con el servidor
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Connect failed: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return;
    }
    

	loggin(client_socket, wsaData);
	bool success = receive_msg(client_socket, wsaData);
	if(first) {
	    cout << "[...] Esperando al otro jugador" << endl;
	    success = receive_msg(client_socket, wsaData);
	}
	
	// Main loop - Playing
	playing(jugador, client_socket, wsaData);

    // Cierra el socket del cliente y limpia la biblioteca Winsock
    closesocket(client_socket);
    WSACleanup();
    cout << "[INFO] Conexion cerrada con el servidor." << endl;
}

int main() {
    // Inicializamos el struct del jugador
    Jugador jugador;
    
    // Carga el tablero
    load_board(jugador); // Cargo el tablero con las filas y columnas principales
    selection_stage(jugador);
    
    // Inicia el cliente con el jugador
    start_client(jugador); 
}
