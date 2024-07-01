#include <iostream>         // Incluye la biblioteca de entrada/salida estándar
#include <string>           // Incluye la biblioteca de cadenas de caracteres
#include <thread>           // Incluye la biblioteca de manejo de hilos
#include <vector>           // Incluye la biblioteca de vectores
#include <cstring>          // Incluye la biblioteca de manipulación de cadenas de caracteres
#include <arpa/inet.h>      // Incluye la biblioteca de funciones de Internet
#include <sys/types.h>      // Incluye tipos de datos usados en sistemas de sockets
#include <sys/socket.h>     // Incluye la biblioteca de funciones de sockets
#include <unistd.h>         // Incluye la biblioteca de funciones de POSIX
#include <mutex>            // Incluye la biblioteca de manejo de exclusión mutua
#include <algorithm>        // Incluye la biblioteca de algoritmos
#include <sqlite3.h>        // Incluye la biblioteca de SQLite

#define PORT 1234           // Define el puerto del servidor
#define BUFFER_SIZE 1024    // Define el tamaño del buffer de recepción

using namespace std;        // Usa el espacio de nombres estándar

bool first = true;          // Variable global para identificar el primer cliente
mutex client_list_mutex;    // Mutex para proteger el acceso a la lista de clientes
vector<int> client_sockets; // Lista de sockets de los clientes
vector<string> client_users; // Lista de usuarios de los clientes

sqlite3 *db;                // Base de datos SQLite

// Clase que maneja los hilos de los clientes
class ClientThread {
public:
    // Constructor de la clase
    ClientThread(int client_socket, sockaddr_in client_address)
        : client_socket(client_socket), client_address(client_address) {}

    // Operador de función, será ejecutado cuando se inicie el hilo
    void operator()() {
        cout << "\n[+] Client " << ntohs(client_address.sin_port) << " has connected" << endl;

        char buffer[BUFFER_SIZE];             // Buffer para recibir datos
        memset(buffer, 0, sizeof(buffer));    // Inicializa el buffer con ceros

        {
            lock_guard<mutex> lock(client_list_mutex);
            //client_sockets.push_back(client_socket); // Elimina esta línea para agregar después de autenticar
        }

        while (true) {
            int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0); // Recibe datos del cliente

            if (bytesReceived <= 0) { // Si no se reciben datos o hay un error, se sale del bucle
                break;
            }

            string message(buffer);   // Convierte el buffer a una cadena de caracteres

            // Manejar mensaje con "creds"
            if (message.find("creds") != string::npos) {
                size_t pos1 = message.find('\n');
                size_t pos2 = message.find('\n', pos1 + 1);

                if (pos1 != string::npos && pos2 != string::npos) {
                    string user = message.substr(pos1 + 1, pos2 - pos1 - 1);
                    string pass = message.substr(pos2 + 1);

                    // Remover caracteres de nueva línea adicionales
                    user.erase(remove(user.begin(), user.end(), '\n'), user.end());
                    pass.erase(remove(pass.begin(), pass.end(), '\n'), pass.end());

                    cout << "\n[*] Received credentials from client " << ntohs(client_address.sin_port) << " - User: " << user << ", Pass: " << pass << endl;

                    // Verificar usuario y contraseña
                    string sql = "SELECT password FROM users WHERE name = ?";
                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
                        if (sqlite3_step(stmt) == SQLITE_ROW) {
                            string stored_pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                            if (stored_pass == pass) {
                                send(client_socket, "valido", 6, 0);

                                {
                                    lock_guard<mutex> lock(client_list_mutex);
                                    client_sockets.push_back(client_socket); // Agregar el socket del cliente a la lista
                                    client_users.push_back(user); // Agregar el usuario del cliente a la lista
                                }

                                // Verificar si el cliente es el unico en el vector
                                {
                                    lock_guard<mutex> lock(client_list_mutex);
                                    if (client_sockets.size() == 1) {
                                        send(client_socket, "first\n", 6, 0); // Enviar "first\n" al cliente
                                    }
                                    else{
                                           // Enviamos los usuarios del contrincante a cada cliente
                                           string user1 = "user1\n" + client_users[0];
                                           string user2 = "user2\n" + client_users[1];
                                           send(client_socket, user1.c_str(), user1.length(), 0);
                                           send(client_sockets[0], user2.c_str(), user2.length(), 0);
                                    }
                                }
                            } else {
                                send(client_socket, "incorrecto", 10, 0);
                            }
                        } else {
                            // Crear nuevo usuario
                            sqlite3_finalize(stmt);
                            sql = "INSERT INTO users (name, password, points) VALUES (?, ?, 0)";
                            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                                sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
                                sqlite3_bind_text(stmt, 2, pass.c_str(), -1, SQLITE_STATIC);
                                if (sqlite3_step(stmt) == SQLITE_DONE) {
                                    send(client_socket, "valido", 6, 0);

                                    // Almacenar el nombre de usuario autenticado
                                    // authenticated_user = user;

                                    {
                                        lock_guard<mutex> lock(client_list_mutex);
                                        client_sockets.push_back(client_socket); // Agregar el socket del cliente a la lista
                                        client_users.push_back(user); // Agregar el usuario del cliente a la lista
                                    }

                                    // Verificar si el cliente es el único en el vector
                                    {
                                        lock_guard<mutex> lock(client_list_mutex);
                                        if (client_sockets.size() == 1) {
                                            send(client_socket, "first\n", 6, 0); // Enviar "first\n" al cliente
                                        }
                                        else{
                                           // Enviamos los usuarios del contrincante a cada cliente
                                           string user1 = "user1\n" + client_users[0];
                                           string user2 = "user2\n" + client_users[1];
                                           send(client_socket, user1.c_str(), user1.length(), 0);
                                           send(client_sockets[0], user2.c_str(), user2.length(), 0);
                                        }
                                    }
                                }
                            }
                            sqlite3_finalize(stmt);
                        }
                    }
                }
            }else{
                message = message.substr(0, message.find('\n')); // Elimina caracteres de nueva línea

                cout << "\n[*] Message received from client " << ntohs(client_address.sin_port) << ": " << message << endl;

                // Reenviar el mensaje a todos los demás clientes
                {
                    lock_guard<mutex> lock(client_list_mutex);
                    for (int socket : client_sockets) {
                        if (socket != client_socket) {
                            send(socket, buffer, bytesReceived, 0);
                        }
                    }
                }

            }
        }

        {
            lock_guard<mutex> lock(client_list_mutex);
            client_sockets.erase(remove(client_sockets.begin(), client_sockets.end(), client_socket), client_sockets.end()); // Vaciamos el vector client_sockets cuando termina la partida
            client_users.clear(); // Vaciamos el vector client_users cuando termina la partida
            if (client_sockets.empty()) {
                first = true;  // Reinicia la variable first si no hay clientes conectados
            }
        }

        cout << "\n[!] Client " << ntohs(client_address.sin_port) << " disconnected" << endl;
        close(client_socket); // Cierra el socket del cliente
    }

private:
    int client_socket;          // Socket del cliente
    sockaddr_in client_address; // Dirección del cliente
};

int main() {
    // Crea un socket de servidor
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        return 1;
    }

    int opt = 1;
    // Configura el socket para reutilizar la dirección
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configura la dirección del servidor
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;                // Familia de direcciones (IPv4)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Dirección IP (cualquier interfaz)
    server_addr.sin_port = htons(PORT);              // Puerto del servidor

    // Asocia el socket con la dirección y el puerto especificados
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind failed: " << strerror(errno) << endl;
        close(server_socket);
        return 1;
    }

    // Configura el socket para escuchar conexiones entrantes
    if (listen(server_socket, SOMAXCONN) == -1) {
        cerr << "Listen failed: " << strerror(errno) << endl;
        close(server_socket);
        return 1;
    }

    // Abrir la base de datos SQLite
    if (sqlite3_open("example.db", &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Crear tabla de usuarios si no existe
    const char *create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL,
            points INTEGER NOT NULL DEFAULT 0
        );
    )";
    char *errMsg = nullptr;
    if (sqlite3_exec(db, create_table_sql, 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return 1;
    }

    cout << "Server listening on port " << PORT << endl;

    vector<thread> client_threads; // Vector para almacenar los hilos de clientes

    while (true) {
        sockaddr_in client_addr;            // Dirección del cliente
        socklen_t client_addr_len = sizeof(client_addr); // Tamaño de la dirección del cliente
        // Acepta una conexión entrante
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket == -1) {
            cerr << "Accept failed: " << strerror(errno) << endl;
            continue; // Continúa con la siguiente iteración si falla aceptar la conexión
        }

        // Crea un nuevo hilo para manejar el cliente
        client_threads.emplace_back(ClientThread(client_socket, client_addr));
    }

    // Une todos los hilos de clientes antes de cerrar el servidor
    for (auto& t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    close(server_socket); // Cierra el socket del servidor
    sqlite3_close(db); // Cierra la base de datos SQLite
    return 0;
}
