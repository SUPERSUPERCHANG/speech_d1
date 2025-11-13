// src/main.cpp
#include "../include/fsm/arm_fsm.hpp"
#include "../include/fsm/grab_task.hpp"
#include <../include/arm_control/arm_speech_control.hpp>
#include "../include/fsm/fsmlist.hpp"
#include "../include/tcp/tcp_socket.hpp"
#include <../3rdparty/include/tinyfsm.hpp>


#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

// set a lock for sendevent
static std::mutex gFsmMutex;


// ---- helpers: trim & lower (camelCase)
static inline void lstrip(std::string& s)
{
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    if (i) s.erase(0, i);
}

static inline void rstrip(std::string& s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.pop_back();
}

static inline std::string trim(std::string s)
{
    lstrip(s);
    rstrip(s);
    return s;
}

static inline std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

static inline char parseCommand(const std::string& raw)
{
    std::string s = toLower(trim(raw));
    if (s.empty()) return '\0';
    // 支持单字母和完整单词
    if (s == "z" || s == "zero" || s == "movezero" || s == "move_zero") return 'z';
    if (s == "h" || s == "hold" || s == "movehold" || s == "move_hold") return 'h';
    if (s == "n" || s == "handle" || s == "movehandle" || s == "move_handle") return 'n';
    if (s == "o" || s == "open" || s == "moveopen" || s == "move_open") return 'o';
    if (s == "c" || s == "close" || s == "moveclose" || s == "move_close") return 'c';
    if (s == "w" || s == "state" || s == "where" || s == "whoami") return 'w';
    if (s == "g" || s == "grab" || s == "grasp") return 'g';
    if (s=="r"||s=="release"||s=="release_gripper") return 'r';
    if (s == "q" || s == "quit" || s == "exit") return 'q';
    return '\0';
}

int main()
{
    // === 1) Initialize and attach the controller ===
    ArmSpeechControl controller;
    ArmFSM::attachController(controller);  // Inject controller into FSM

    // === 2) Start all FSMs ===
    fsm_list::start();  // Enter initial state and trigger entry()

    // === 3) Start periodic Tick thread ===
    std::atomic<bool> running{true};
    const std::chrono::milliseconds tickPeriod(50); // Trigger a Tick every 100 ms
    std::thread tickThread([&]()
    {
        while (running.load(std::memory_order_relaxed))
        {
            {
                std::lock_guard<std::mutex> lk(gFsmMutex);  // Protect send_event from concurrent access
                send_event(Tick{});                         // Periodic Tick signal
            }
            std::this_thread::sleep_for(tickPeriod);
        }
    });

    // === 4) Start TCP server ===
    std::cout << "===== Arm FSM + TCP =====\n"
              << "TCP port: 9000\n"
              << "Available commands:\n"
              << " z / zero       - MoveZero\n"
              << " h / hold       - MoveHold\n"
              << " n / handle     - MoveHandle\n"
              << " o / open       - MoveOpen\n"
              << " c / close      - MoveClose\n"
              << " g / grab       - Execute grab task sequence\n"
              << " r / release    - Execute release task sequence\n"
              << " w / state      - Query current state\n"
              << " q / quit       - Quit program\n\n";

    TcpSocket server;
    const uint16_t port = 9000;
    if (!server.bindAndListen(port))
    {
        std::cerr << "[TCP] Failed to bind or listen on port " << port << "\n";
        return 1;
    }
    std::cout << "[TCP] Listening on " << port << " ...\n";

    bool quitAll = false;

    // === 5) Accept client connections ===
    while (!quitAll)
    {
        std::string clientIp;
        uint16_t clientPort = 0;

        TcpSocket client = server.accept(&clientIp, &clientPort);
        if (!client.valid())
        {
            std::cerr << "[TCP] accept() failed, waiting for new connections...\n";
            continue;
        }

        std::cout << "[TCP] Accepted " << clientIp << ":" << clientPort << "\n";
        client.sendAll("Welcome to the Arm FSM Server!\n"
                       "Commands: z h n o c g r w q\n");

        // === 6) Per-client command loop ===
        char buffer[1024];
        while (true)
        {
            long long bytes = client.recvSome(buffer, sizeof(buffer));
            if (bytes <= 0)
            {
                std::cout << "[TCP] Client disconnected: "
                          << clientIp << ":" << clientPort << "\n";
                break;
            }

            std::string msg(buffer, buffer + bytes);
            msg = trim(msg);
            if (msg.empty()) continue;

            char cmd = parseCommand(msg);
            if (cmd == '\0')
            {
                client.sendAll(std::string("Unknown command: ") + msg + "\n");
                continue;
            }

            // === 7) Dispatch command as FSM event ===
            switch (cmd)
            {
                case 'z':
                    std::cout << "[FSM] → MoveZero\n";
                    send_event(MoveZero{});
                    client.sendAll("ACK: MoveZero\n");
                    break;
                case 'h':
                    std::cout << "[FSM] → MoveHold\n";
                    send_event(MoveHold{});
                    client.sendAll("ACK: MoveHold\n");
                    break;
                case 'n':
                    std::cout << "[FSM] → MoveHandle\n";
                    send_event(MoveHandle{});
                    client.sendAll("ACK: MoveHandle\n");
                    break;
                case 'o':
                    std::cout << "[FSM] → MoveOpen\n";
                    send_event(MoveOpen{});
                    client.sendAll("ACK: MoveOpen\n");
                    break;
                case 'c':
                    std::cout << "[FSM] → MoveClose\n";
                    send_event(MoveClose{});
                    client.sendAll("ACK: MoveClose\n");
                    break;
                case 'g':
                    std::cout << "[FSM] → Grab (task FSM)\n";
                    send_event(GrabEvent{});
                    client.sendAll("ACK: Grab\n");
                    break;
                case 'r':
                    std::cout << "[FSM] → Release (task FSM)\n";
                    send_event(ReleaseEvent{});
                    client.sendAll("ACK: Release\n");
                    break;
                case 'w': {
                    std::string stateName = ArmFSM::getCurrentStateName();
                    std::cout << "[FSM] Current state: " << stateName << "\n";
                    client.sendAll(std::string("STATE: ") + stateName + "\n");
                    break;
                }
                case 'q':
                    std::cout << "[FSM] Quit by client.\n";
                    client.sendAll("ACK: Quit\n");
                    quitAll = true;
                    break;
            }

            if (quitAll) break;
        } // end per-client loop
    } // end accept loop

    // === 8) Stop Tick thread ===
    running = false;
    if (tickThread.joinable())
        tickThread.join();

    std::cout << "Exit main.\n";
    return 0;
}

