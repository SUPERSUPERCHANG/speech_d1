//
// Created by chang on 11/12/25.
//

#include "fsm/grab_task.hpp"

#include <iostream>
#include <ostream>

#include "fsm/arm_fsm.hpp"
#include "fsm/fsmlist.hpp"

//define steps for a series of movement
namespace steps {
    using StepFn = void(*)();

    const std::vector<StepFn> reachingSteps = {
        [](){ send_event(MoveOpen{});  },
        [](){ send_event(MoveHandle{});},
        [](){ send_event(MoveClose{}); },
        [](){ send_event(MoveHold{});  },
    };

    const std::vector<StepFn> releasingSteps = {
        [](){send_event(MoveRelease{});},
        [](){send_event(MoveOpen{});  },
        [](){send_event(MoveHold{});  },
    };
}

// ----------------------------------------------------------------------------
// State: Reaching
//

class Reaching:public ArmTaskFSM
{

    void entry() override
    {
        std::cout << "entry to Reaching" << std::endl;

        current_state = 0;
        lastPrevReached = ArmFSM::isPreviousStateReached_; // 记录初始值，供上升沿检测
        isReadyToNext = false;                              // 先不允许推进，等“到达”的上升沿

        // 发送第一个目标
        steps::reachingSteps[current_state]();
    }

    void react(CancelEvent const&) override
    {
        transit<Idle>();
    }

    void react(Tick const&) override
    {
        // 1) 上升沿检测：false -> true 的瞬间，才把 isReadyToNext 置为 true
        bool now = ArmFSM::isPreviousStateReached_;
        // std::cout<<"in tick of taskFSM"<<std::endl;
        if (now && !lastPrevReached) {
            isReadyToNext = true;   // 刚刚到达上一步
        }
        lastPrevReached = now;

        // 2) 只有在“刚到达过一次”且还有下一步时，推进一步；推进后立即去抖
        if (isReadyToNext) {
            if (current_state + 1 < steps::reachingSteps.size()) {
                ++current_state;
                steps::reachingSteps[current_state]();
                isReadyToNext = false;   // 立刻关闸，防止同一 Tick 或持续 true 下二次触发
            }
        }
    }

    void react(ReleaseEvent const&) override
    {
        transit<Releasing>();
    }

    void exit() override
    {
        current_state = 0;
        isReadyToNext = false;
        lastPrevReached = false;
    }

};

class Releasing:public ArmTaskFSM
{
    void entry() override
    {
        std::cout << "entry to Releasing" << std::endl;

        current_state = 0;
        lastPrevReached = ArmFSM::isPreviousStateReached_; // 记录初始值，供上升沿检测
        isReadyToNext = false;                              // 先不允许推进，等“到达”的上升沿

        // 发送第一个目标
        steps::releasingSteps[current_state]();
    }
    void react(GrabEvent const & e) override
    {
        transit<Reaching>();
    }
    void react(Tick const&) override
    {
        // 1) 上升沿检测：false -> true 的瞬间，才把 isReadyToNext 置为 true
        bool now = ArmFSM::isPreviousStateReached_;
        // std::cout<<"in tick of taskFSM"<<std::endl;
        if (now && !lastPrevReached) {
            isReadyToNext = true;   // 刚刚到达上一步
        }
        lastPrevReached = now;

        // 2) 只有在“刚到达过一次”且还有下一步时，推进一步；推进后立即去抖
        if (isReadyToNext) {
            if (current_state + 1 < steps::releasingSteps.size()) {
                ++current_state;
                steps::releasingSteps[current_state]();
                isReadyToNext = false;   // 立刻关闸，防止同一 Tick 或持续 true 下二次触发
            }
        }
    }
    void react(CancelEvent const & e) override
    {
        transit<Idle>();
    }
    void exit() override
    {
        current_state = 0;
        isReadyToNext = false;
        lastPrevReached = false;
    }
};

class Idle:public ArmTaskFSM
{
    void entry() override
    {
        current_state = 0;
        isReadyToNext = false;
        lastPrevReached = false;
    }
    void react(GrabEvent const&) override
    {
        transit<Reaching>();
    }
    void react(ReleaseEvent const&) override
    {
        transit<Releasing>();
    }
    void exit() override
    {
        current_state = 0;
        isReadyToNext = false;
        lastPrevReached = false;
    }
}; // forward declaration


FSM_INITIAL_STATE(ArmTaskFSM, Idle)
