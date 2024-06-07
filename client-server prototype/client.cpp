#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Enlaza la biblioteca Winsock 2.2 para permitir el uso de funciones de red
#pragma comment(lib, "ws2_32.lib")

// Define el puerto en el que el cliente se conectará y el tamaño del búfer de recepción
#define PORT 1234
#define BUFFER_SIZE 1024

using namespace std;

void start_client() {
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

    // Configura la dirección del servidor al que se conectará el cliente
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // Familia de direcciones IPv4
    server_addr.sin_addr.s_addr = inet_addr("34.176.243.105"); // Dirección IP del servidor (local)
    server_addr.sin_port = htons(PORT); // Puerto del servidor

    // Conecta el socket del cliente con el servidor
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Connect failed: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    while (true) {
        // Solicita al usuario que ingrese un mensaje
        cout << "[+] Send a message: ";
        string message;
        getline(cin, message); // Lee el mensaje de la entrada estándar

        // Envía el mensaje al servidor
        int send_result = send(client_socket, message.c_str(), message.size(), 0);
        if (send_result == SOCKET_ERROR) {
            cerr << "Send failed: " << WSAGetLastError() << endl;
            break;
        }

        // Prepara un búfer para recibir la respuesta del servidor
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0); // Recibe datos del servidor
        if (bytes_received <= 0) {
            cerr << "Recv failed or connection closed: " << WSAGetLastError() << endl;
            break;
        }

        // Muestra el mensaje recibido del servidor
        cout << "[+] Message received from server: " << string(buffer, bytes_received) << endl;
    }

    // Cierra el socket del cliente y limpia la biblioteca Winsock
    closesocket(client_socket);
    WSACleanup();
}

int main() {
    start_client(); // Inicia el cliente
    return 0;
}
