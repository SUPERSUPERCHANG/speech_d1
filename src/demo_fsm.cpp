// src/main.cpp
#include <iostream>
#include "arm_fsm.hpp"
#include "fsmlist.hpp"
// 这里假设：
// - 你已经在 arm_fsm.hpp(or .cpp) 里定义了事件类型：MoveZero, MoveHold, MoveHandle, MoveOpen, MoveClose（均继承 tinyfsm::Event）
// - 以及 fsm_list = tinyfsm::FsmList<Zero, Hold, Handle, Open, Close>（或等价写法）
// - 各状态类的 entry() 会打印如 "Move to zero" / "Hold" / "Handle" / "Open" / "Close"

int main() {
    // 启动 TinyFSM（会触发初始状态的 entry()）
    fsm_list::start();

    std::cout << "===== Arm FSM Tester =====\n"
              << "z: MoveZero\n"
              << "h: MoveHold\n"
              << "n: MoveHandle\n"
              << "o: MoveOpen\n"
              << "c: MoveClose\n"
              << "w: Current State\n"
              << "q: Quit\n";

    while (true) {
        std::cout << "\nCommand ? [z/h/n/o/c/q]: ";
        char cmd;
        if (!(std::cin >> cmd)) break;

        switch (cmd) {
            case 'z': {
                send_event(MoveZero{});
                std::cout << "[sent] MoveZero\n";

                break;
            }
            case 'h': {
                send_event(MoveHold{});
                std::cout << "[sent] MoveHold\n";
                break;
            }
            case 'n': {
                // n ＝ handle（避免和 h=Hold 冲突）
                send_event(MoveHandle{});
                std::cout << "[sent] MoveHandle\n";
                break;
            }
            case 'o': {
                send_event(MoveOpen{});
                std::cout << "[sent] MoveOpen\n";
                break;
            }
            case 'c': {
                send_event(MoveClose{});
                std::cout << "[sent] MoveClose\n";
                break;
            }
            case 'q': {
                std::cout << "Bye!\n";
                return 0;
            }
            case 'w': {
                std::cout << Arm::getCurrentStateName() <<"\n";
                break;
            }
            default:
                std::cout << "Invalid input.\n";
                break;
        }
    }
    return 0;
}