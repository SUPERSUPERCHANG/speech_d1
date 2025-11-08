#include "tcp_socket.hpp"
#include <iostream>
#include <thread>

int main() {
    TcpSocket server;

    // Bind to port 9000 and start listening
    if (!server.bindAndListen(9000)) {
        std::cerr << "Failed to bind or listen on port 9000\n";
        return 1;
    }

    std::cout << "Server is listening on port 9000...\n";

    while (true) {
        std::string clientIp;
        uint16_t clientPort = 0;

        // Wait for a new connection
        TcpSocket client = server.accept(&clientIp, &clientPort);
        if (!client.valid()) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Accepted connection from "
                  << clientIp << ":" << clientPort << "\n";

        // Send welcome message
        client.sendAll("Welcome to the Arm Control Server!\n");

        // Simple echo loop
        char buffer[1024];
        while (true) {
            long long bytes = client.recvSome(buffer, sizeof(buffer));
            if (bytes <= 0) {
                std::cout << "Client disconnected: "
                          << clientIp << ":" << clientPort << "\n";
                break;
            }

            std::string msg(buffer, buffer + bytes);
            std::cout << "Received: " << msg;

            // Echo the same message back
            client.sendAll(msg);
        }
    }

    return 0;
}
