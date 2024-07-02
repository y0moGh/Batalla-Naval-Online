#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <algorithm>
#include <sqlite3.h>
#include <map>

#define PORT 1234
#define BUFFER_SIZE 1024

using namespace std;

bool first = true;
mutex client_list_mutex;
vector<int> client_sockets;
map<int, string> client_users; // Mapa para asociar sockets con usuarios

sqlite3 *db;

class ClientThread {
public:
    ClientThread(int client_socket, sockaddr_in client_address)
        : client_socket(client_socket), client_address(client_address) {}

    void operator()() {
        cout << "\n[+] Client " << ntohs(client_address.sin_port) << " has connected" << endl;

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        while (true) {
            int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytesReceived <= 0) {
                break;
            }

            string message(buffer);

            if (message.find("creds") != string::npos) {
                size_t pos1 = message.find('\n');
                size_t pos2 = message.find('\n', pos1 + 1);

                if (pos1 != string::npos && pos2 != string::npos) {
                    string user = message.substr(pos1 + 1, pos2 - pos1 - 1);
                    string pass = message.substr(pos2 + 1);

                    user.erase(remove(user.begin(), user.end(), '\n'), user.end());
                    pass.erase(remove(pass.begin(), pass.end(), '\n'), pass.end());

                    cout << "\n[*] Received credentials from client " << ntohs(client_address.sin_port) << " - User: " << user << ", Pass: " << pass << endl;

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
                                    client_sockets.push_back(client_socket);
                                    client_users[client_socket] = user; // Asocia el usuario con el socket
                                }

                                {
                                    lock_guard<mutex> lock(client_list_mutex);
                                    if (client_sockets.size() == 1) {
                                        send(client_socket, "first\n", 6, 0);
                                    } else {
                                        string user1 = "user1\n" + client_users[client_sockets[0]];
                                        string user2 = "user2\n" + client_users[client_sockets[1]];
                                        send(client_socket, user1.c_str(), user1.length(), 0);
                                        send(client_sockets[0], user2.c_str(), user2.length(), 0);
                                    }
                                }
                            } else {
                                send(client_socket, "incorrecto", 10, 0);
                            }
                        } else {
                            sqlite3_finalize(stmt);
                            sql = "INSERT INTO users (name, password, points) VALUES (?, ?, 0)";
                            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                                sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
                                sqlite3_bind_text(stmt, 2, pass.c_str(), -1, SQLITE_STATIC);
                                if (sqlite3_step(stmt) == SQLITE_DONE) {
                                    send(client_socket, "valido", 6, 0);

                                    {
                                        lock_guard<mutex> lock(client_list_mutex);
                                        client_sockets.push_back(client_socket);
                                        client_users[client_socket] = user;
                                    }

                                    {
                                        lock_guard<mutex> lock(client_list_mutex);
                                        if (client_sockets.size() == 1) {
                                            send(client_socket, "first\n", 6, 0);
                                        } else {
                                            string user1 = "user1\n" + client_users[client_sockets[0]];
                                            string user2 = "user2\n" + client_users[client_sockets[1]];
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
            } else if (message == "podio") {
                string podio_result;
                string sql = "SELECT name, points FROM users ORDER BY points DESC";
                sqlite3_stmt *stmt;
                if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                        int points = sqlite3_column_int(stmt, 1);
                        podio_result += name + ": " + to_string(points) + "\n";
                    }
                    sqlite3_finalize(stmt);
                } else {
                    cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
                }
                send(client_socket, ("podio:" + podio_result).c_str(), podio_result.length() + 6, 0);
            } else if (message.find("win") != string::npos) {
                cout << "\n[+] Win received from " << ntohs(client_address.sin_port) << ": " << message << endl;
                {
                    lock_guard<mutex> lock(client_list_mutex);
                    string user = client_users[client_socket];
                    string sql = "UPDATE users SET points = points + 10 WHERE name = ?";
                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
                        if (sqlite3_step(stmt) != SQLITE_DONE) {
                            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
                        }
                        sqlite3_finalize(stmt);
                    }
                }

            } else {
                message = message.substr(0, message.find('\n'));

                cout << "\n[*] Message received from client " << ntohs(client_address.sin_port) << ": " << message << endl;

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
            client_sockets.erase(remove(client_sockets.begin(), client_sockets.end(), client_socket), client_sockets.end());
            client_users.erase(client_socket); // Elimina la asociación del usuario con el socket
            if (client_sockets.empty()) {
                first = true;
            }
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

    if (sqlite3_open("example.db", &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

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
    sqlite3_close(db);
    return 0;
}
