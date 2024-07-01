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

#define PORT 1234           // Define el puerto del servidor
#define BUFFER_SIZE 1024    // Define el tamaño del buffer de recepción

using namespace std;        // Usa el espacio de nombres estándar

bool first = true;          // Variable global para identificar el primer cliente
mutex client_list_mutex;    // Mutex para proteger el acceso a la lista de clientes
vector<int> client_sockets; // Lista de sockets de los clientes

// Clase que maneja los hilos de los clientes
class ClientThread {
public:
    // Constructor de la clase
    ClientThread(int client_socket, sockaddr_in client_address)
        : client_socket(client_socket), client_address(client_address) {}

    // Operador de función, será ejecutado cuando se inicie el hilo
    void operator()() {
        cout << "\n[+] Client " << ntohs(client_address.sin_port) << " has connected" << endl;
        
        {
            lock_guard<mutex> lock(client_list_mutex);
            if (first) {
                // Enviar "first" al primer cliente
                const char* msg = "first\n";
                send(client_socket, msg, strlen(msg), 0);
              
                first = false;  // Actualiza la variable para que no se envíe a los siguientes clientes
            } else {
                const char* msg2 = "second\n";
                send(client_socket, msg2, strlen(msg2), 0);
                send(client_sockets[0], msg2, strlen(msg2), 0); // Le avisamos al jugador1 que ya puede empezar a atacar

            }
            client_sockets.push_back(client_socket); // Añadir el socket del cliente a la lista
        }

        while (true) {
            char buffer[BUFFER_SIZE];             // Buffer para recibir datos
            memset(buffer, 0, sizeof(buffer));    // Inicializa el buffer con ceros
            int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0); // Recibe datos del cliente

            if (bytesReceived <= 0) { // Si no se reciben datos o hay un error, se sale del bucle
                break;
            }

            string message(buffer);   // Convierte el buffer a una cadena de caracteres
            message = message.substr(0, message.find('\n')); // Elimina caracteres de nueva línea

            if (message == "bye") {   // Si el mensaje es "bye", se sale del bucle
                break;
            }

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

        {
            lock_guard<mutex> lock(client_list_mutex);
            client_sockets.erase(remove(client_sockets.begin(), client_sockets.end(), client_socket), client_sockets.end());
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
    return 0;
}
