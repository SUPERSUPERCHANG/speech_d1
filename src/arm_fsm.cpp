//
// Created by chang on 11/10/25.
//
// src/main.cpp

#include "arm_fsm.hpp"
#include <arm_speech_control.hpp>


// ----------------------------------------------------------------------------
// State: Zero
//
class Zero
: public ArmFSM
{
    void entry() override {
        std::cout << "entry to zero" << std::endl;
        arm().zero_joint();
    };
    // bool isStateReached() override
    // {
    //
    // }
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
    };
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
    };
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
    };
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
    };
};





void ArmFSM::react(MoveZero   const &)
{
    std::cout << "Move2Zero" << std::endl;
    transit<Zero>();
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




