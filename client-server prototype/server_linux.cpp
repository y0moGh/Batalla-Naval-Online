#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1234
#define BUFFER_SIZE 1024

using namespace std;

class ClientThread {
public:
    ClientThread(int client_socket, sockaddr_in client_address)
        : client_socket(client_socket), client_address(client_address) {}

    void operator()() {
        cout << "\n[+] Client " << ntohs(client_address.sin_port) << " has connected" << endl;

        while (true) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytesReceived <= 0) {
                break;
            }

            string message(buffer);
            message = message.substr(0, message.find('\n'));

            if (message == "bye") {
                break;
            }

            cout << "\n[*] Message received from client " << ntohs(client_address.sin_port) << ": " << message << endl;
            send(client_socket, buffer, bytesReceived, 0);
        }
        cout << "\n[!] Client " << ntohs(client_address.sin_port) << " disconnected" << endl;
        close(client_socket);
    }

private:
    int client_socket;
    sockaddr_in client_address;
};

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        return 1;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind failed: " << strerror(errno) << endl;
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        cerr << "Listen failed: " << strerror(errno) << endl;
        close(server_socket);
        return 1;
    }

    cout << "Server listening on port " << PORT << endl;

    vector<thread> client_threads;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket == -1) {
            cerr << "Accept failed: " << strerror(errno) << endl;
            continue;
        }

        client_threads.emplace_back(ClientThread(client_socket, client_addr));
    }

    for (auto& t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    close(server_socket);
    return 0;
}

