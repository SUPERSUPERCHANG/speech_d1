#include "tcp_socket.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> [port]\n";
        return 1;
    }

    std::string ip = argv[1];
    int port = 9000;  // 默认端口
    if (argc >= 3) port = std::stoi(argv[2]);

    TcpSocket client;

    // Connect to localhost:9000 (you can change IP or port)
    if (!client.connectTo(ip, 9000, 3000)) {
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
