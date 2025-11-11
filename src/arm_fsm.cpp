//
// Created by chang on 11/10/25.
//
// src/main.cpp

#include "arm_fsm.hpp"



// ----------------------------------------------------------------------------
// State: Zero
//
class Zero
: public Arm
{
    void entry() override {
        std::cout << "Move to zero" << std::endl;
    };
};
// ----------------------------------------------------------------------------
// State: Hold
//
class Hold
: public Arm
{
    void entry() override {
        std::cout << "Hold" << std::endl;
    };
};
// ----------------------------------------------------------------------------
// State: Handle
//
class Handle
: public Arm
{
    void entry() override {
        std::cout << "Handle" << std::endl;
    };
};
// ----------------------------------------------------------------------------
// State: Open
//
class Open
: public Arm
{
    void entry() override {
        std::cout << "Open" << std::endl;
    };
};
// ----------------------------------------------------------------------------
// State: Close
//
class Close
: public Arm
{
    void entry() override {
        std::cout << "Close" << std::endl;
    };
};





void Arm::react(MoveZero   const &)
{
    std::cout << "MoveZero" << std::endl;
}

void Arm::react(MoveHandle const &)
{
    std::cout << "MoveHandle" << std::endl;
}

void Arm::react(MoveHold const &)
{
    std::cout << "MoveHold" << std::endl;
}

void Arm::react(MoveOpen const &)
{
    std::cout << "MoveOpen" << std::endl;
}

void Arm::react(MoveClose const &)
{
    std::cout << "MoveZero" << std::endl;
}


std::string Arm::getCurrentStateName() {
    if (Arm::template is_in_state<Zero>())   return "Zero";
    if (Arm::template is_in_state<Hold>())   return "Hold";
    if (Arm::template is_in_state<Handle>()) return "Handle";
    if (Arm::template is_in_state<Open>())   return "Open";
    if (Arm::template is_in_state<Close>())  return "Close";
    return "Unknown";
}

FSM_INITIAL_STATE(Arm, Zero)




