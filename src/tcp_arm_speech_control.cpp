//
// Created by chang on 11/8/25.
//

#include "tcp_arm_speech_control.hpp"

// --- helpers: trim \r \n
static inline void lstrip(std::string &s) {
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    if (i) s.erase(0, i);
}
static inline void rstrip(std::string &s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.pop_back();
}
static inline std::string trim(std::string s) { lstrip(s); rstrip(s); return s; }

int main()
{
    TcpSocket server;
    // Bind to port 9000 and start listening
    if (!server.bindAndListen(9000)) {
        std::cerr << "Failed to bind or listen on port 9000\n";
        return 1;
    }

    std::cout << "Server is listening on port 9000...\n";

    ArmSpeechControl armSpeechControl;
    armSpeechControl.init();

    while (true) {
        std::string clientIp;
        uint16_t clientPort = 0;

        // Wait for a new connection
        TcpSocket client = server.accept(&clientIp, &clientPort);
        if (!client.valid()) {
            std::cerr << "Waiting for connection...\n";
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

            msg = trim(msg);
            armSpeechControl.process_command(msg);
            std::cout << "Received: " << msg;
            // Echo the same message back
            client.sendAll(msg);
        }
    }








return 0;
}