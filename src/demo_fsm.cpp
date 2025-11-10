#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "tinyfsm.hpp"  // 确保能找到你本地的 tinyfsm.hpp

// ---------- 事件 ----------
struct TickEvent {};
struct ModeCmdEvent { enum class Mode { Idle, Running, Exit } target; };

// ---------- 基类状态机 ----------
struct Machine : tinyfsm::Fsm<Machine> {
  using fsmtype = Machine;
  inline static ModeCmdEvent::Mode currentMode = ModeCmdEvent::Mode::Idle;

  // 缺省事件处理
  void react(const TickEvent&) {}
  void react(const ModeCmdEvent& e) { currentMode = e.target; }

  // 生命周期钩子（可选）
  void entry() {}
  void exit() {}
};

// ---------- 具体状态 ----------
struct IdleState : Machine {
  void entry() override { std::cout << "[Idle] entry\n"; }
  void react(const TickEvent&) override {
    // 看到目标模式变了就切换
    if (currentMode == ModeCmdEvent::Mode::Running) transit<class RunningState>();
  }
};

struct RunningState : Machine {
  void entry() override { std::cout << "[Running] entry\n"; }
  void react(const TickEvent&) override {
    if (currentMode == ModeCmdEvent::Mode::Idle) transit<class IdleState>();
  }
};

// 注册接收 dispatch(...) 的状态列表
using fsmList = tinyfsm::FsmList<IdleState, RunningState>;

// ---------- 主程序 ----------
int main() {
  // 启动（会调用第一个状态的 entry；如果需要其它初始状态，也可以先 dispatch 一个 ModeCmdEvent）
  tinyfsm::start();

  std::atomic<ModeCmdEvent::Mode> nextMode{ModeCmdEvent::Mode::Idle};
  std::atomic<bool> quit{false};

  // 读输入线程（阻塞读），输入 idle / run / exit
  std::thread inputThread([&] {
    std::string line;
    std::cout << "Type command: idle | run | exit\n";
    while (std::getline(std::cin, line)) {
      if (line == "idle") {
        nextMode = ModeCmdEvent::Mode::Idle;
      } else if (line == "run") {
        nextMode = ModeCmdEvent::Mode::Running;
      } else if (line == "exit") {
        nextMode = ModeCmdEvent::Mode::Exit;
        break;
      } else {
        std::cout << "Unknown cmd. Use: idle | run | exit\n";
      }
    }
  });

  // 定时轮询循环
  using namespace std::chrono_literals;
  while (!quit.load()) {
    // 派发心跳
    tinyfsm::dispatch(TickEvent{});

    // 处理可能的模式变更
    auto m = nextMode.load();
    if (m == ModeCmdEvent::Mode::Exit) {
      quit = true;
    } else {
      tinyfsm::dispatch(ModeCmdEvent{m});
    }

    std::this_thread::sleep_for(200ms);
  }

  if (inputThread.joinable()) inputThread.join();
  std::cout << "Bye.\n";
  return 0;
}
