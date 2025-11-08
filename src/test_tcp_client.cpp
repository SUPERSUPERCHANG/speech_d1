#include "tcp_socket.hpp"
#include <iostream>
#include <string>

int main() {
    TcpSocket client;

    // Connect to localhost:9000 (you can change IP or port)
    if (!client.connectTo("127.0.0.1", 9000, 3000)) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    std::cout << "Connected to server!\n";

    // Receive welcome message
    char buffer[1024];
    long long bytes = client.recvSome(buffer, sizeof(buffer));
    if (bytes > 0) {
        std::string msg(buffer, buffer + bytes);
        std::cout << "Server says: " << msg;
    }

    // Send messages from stdin
    std::string input;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, input)) break;
        if (input == "exit") break;

        client.sendAll(input + "\n");

        // Receive echo reply
        bytes = client.recvSome(buffer, sizeof(buffer));
        if (bytes <= 0) break;
        std::cout << "Echo: " << std::string(buffer, buffer + bytes);
    }

    std::cout << "Disconnected.\n";
    return 0;
}
