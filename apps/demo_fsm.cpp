// src/main.cpp
#include "../include/fsm/arm_fsm.hpp"
#include <../include/arm_control/arm_speech_control.hpp>
#include "../include/fsm/fsmlist.hpp"
#include "../include/tcp/tcp_socket.hpp"

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
    if (s == "q" || s == "quit" || s == "exit") return 'q';
    return '\0';
}

int main()
{
    // 1) 控制器注入到 FSM
    ArmSpeechControl controller;
    ArmFSM::attachController(controller); // ★ 注入控制器

    // 2) 启动 FSM（进入初始状态并执行 entry()）
    fsm_list::start();

    std::atomic<bool> running{true};
    const std::chrono::milliseconds tickPeriod(100); // 每 50 ms 触发一次
    std::thread tickThread([&]()
    {
        while (running.load(std::memory_order_relaxed))
        {
            {
                std::lock_guard<std::mutex> lk(gFsmMutex);
                send_event(Tick{});
            }
            std::this_thread::sleep_for(tickPeriod);
        }
    });

    std::cout << "===== Arm FSM + TCP =====\n"
        << "TCP端口: 9000\n"
        << "命令: z/zero, h/hold, n/handle, o/open, c/close, w/state, q/quit\n";

    // 3) 在主函数里直接开 TCP server（单线程）
    TcpSocket server;
    const uint16_t port = 9000;
    if (!server.bindAndListen(port))
    {
        std::cerr << "[TCP] Failed to bind or listen on port " << port << "\n";
        return 1;
    }
    std::cout << "[TCP] Listening on " << port << " ...\n";

    bool quitAll = false;

    while (!quitAll)
    {
        std::string clientIp;
        uint16_t clientPort = 0;

        TcpSocket client = server.accept(&clientIp, &clientPort);
        if (!client.valid())
        {
            std::cerr << "[TCP] accept() failed, continue...\n";
            continue;
        }

        std::cout << "[TCP] Accepted " << clientIp << ":" << clientPort << "\n";
        client.sendAll("Welcome to the Arm FSM Server!\n"
            "Commands: z h n o c w q\n");

        // 简单收发循环：收到就转成事件
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

            switch (cmd)
            {
            case 'z':
                //may need lock here
                std::cout << "[FSM] sending MoveZero\n";
                send_event(MoveZero{});
                client.sendAll("ACK: MoveZero\n");
                break;
            case 'h':
                std::cout << "[FSM] sending MoveHold\n";
                send_event(MoveHold{});
                client.sendAll("ACK: MoveHold\n");
                break;
            case 'n':
                std::cout << "[FSM] sending MoveHandle\n";
                send_event(MoveHandle{});
                client.sendAll("ACK: MoveHandle\n");
                break;
            case 'o':
                std::cout << "[FSM] sending MoveOpen\n";
                send_event(MoveOpen{});
                client.sendAll("ACK: MoveOpen\n");
                break;
            case 'c':
                std::cout << "[FSM] sending MoveClose\n";
                send_event(MoveClose{});
                client.sendAll("ACK: MoveClose\n");
                break;
            case 'w':
                {
                    std::string stateName = ArmFSM::getCurrentStateName();
                    std::cout << "[FSM] Current state: " << stateName << "\n";
                    client.sendAll(std::string("STATE: ") + stateName + "\n");
                    break;
                }
            case 'q':
                std::cout << "[FSM] Quit by client.\n";
                client.sendAll("ACK: Quit\n");
                quitAll = true; // 退出整个程序
                break;
            }

            if (quitAll) break;
        } // per-client loop
    } // accept loop

    // === exit tick thread ===
    running = false;
    if (tickThread.joinable())
        tickThread.join();
    // === finished ===

    std::cout << "Exit main.\n";
    return 0;
}
