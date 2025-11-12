//
// Created by chang on 11/10/25.
//
// src/main.cpp

#include "../../include/fsm/arm_fsm.hpp"
#include <../include/arm_control/arm_speech_control.hpp>

// ----------------------------------------------------------------------------
// State: Zero
//
class Zero
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to zero" << std::endl;
        arm().zero_joint();
    }
    void react(Tick const &) override {
        std::cout << "Tick" << std::endl;
        if (arm().is_move_success(arm().armTargets.at("zero"),3)) {
            isPreviousStateReached_=true;
            std::cout << "arm is zero" << std::endl;
        }
        else {
            isPreviousStateReached_=false;
            std::cout << "arm is not zero, resend the cmd" << std::endl;
            arm().zero_joint();
        }
    }
};
// ----------------------------------------------------------------------------
// State: Hold
//
class Hold
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to hold" << std::endl;
        arm().hold_joint();
    }
    void react(Tick const &) override {
        std::cout << "Tick" << std::endl;
    }
};
// ----------------------------------------------------------------------------
// State: Handle
//
class Handle
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to Handle" << std::endl;
        arm().handle_joint();
    }
    void react(Tick const &) override {
        std::cout << "Tick" << std::endl;
    }
};
// ----------------------------------------------------------------------------
// State: Open
//
class Open
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to Open" << std::endl;
        arm().open_gripper();
    }
    void react(Tick const &) override {
        std::cout << "Tick" << std::endl;
    }
};
// ----------------------------------------------------------------------------
// State: Close
//
class Close
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to Close" << std::endl;
        arm().close_gripper();
    }
    void react(Tick const &) override {
        std::cout << "Tick" << std::endl;
    }
};

void ArmFSM::react(MoveZero   const &)
{
    std::cout << "Move2Zero" << std::endl;
    if (isPreviousStateReached_) {
        transit<Zero>();
    }

}

void ArmFSM::react(MoveHandle const &)
{
    std::cout << "Move2Handle" << std::endl;
    transit<Handle>();
}

void ArmFSM::react(MoveHold const &)
{
    std::cout << "Move2Hold" << std::endl;
    transit<Hold>();
}

void ArmFSM::react(MoveOpen const &)
{
    std::cout << "Move2Open" << std::endl;
    transit<Open>();
}

void ArmFSM::react(MoveClose const &)
{
    std::cout << "Move2Close" << std::endl;
    transit<Close>();
}


std::string ArmFSM::getCurrentStateName() {
    if (ArmFSM::template is_in_state<Zero>())   return "Zero";
    if (ArmFSM::template is_in_state<Hold>())   return "Hold";
    if (ArmFSM::template is_in_state<Handle>()) return "Handle";
    if (ArmFSM::template is_in_state<Open>())   return "Open";
    if (ArmFSM::template is_in_state<Close>())  return "Close";
    return "Unknown";
}


FSM_INITIAL_STATE(ArmFSM, Zero)




