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
        // std::cout << "Tick" << std::endl;
        if (arm().is_move_success(arm().armTargets.at("zero"),3)) {
            isPreviousStateReached_=true;
            // std::cout << "arm is zero" << std::endl;
        }
        else {
            isPreviousStateReached_=false;
            // std::cout << "arm is not zero, resend the cmd" << std::endl;
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
        // std::cout << "Tick" << std::endl;
        if (arm().is_move_success(arm().armTargets.at("hold"),3)) {
            isPreviousStateReached_=true;
            // std::cout << "arm is zero" << std::endl;
        }
        else {
            isPreviousStateReached_=false;
            arm().hold_joint();
        }
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
        // std::cout << "Tick" << std::endl;
        if (arm().is_move_success(arm().armTargets.at("handle"),5)) {
            isPreviousStateReached_=true;
            // std::cout << "arm is zero" << std::endl;
        }
        else {
            isPreviousStateReached_=false;
            arm().handle_joint();
        }
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
        // std::cout << "Tick" << std::endl;
        if (arm().is_gripper_success(arm().armTargets.at("open"),2)) {
            isPreviousStateReached_=true;
            // std::cout << "arm is zero" << std::endl;
        }
        else {
            isPreviousStateReached_=false;
            arm().open_gripper();
        }
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
        // std::cout << "Tick" << std::endl;
        // if (arm().is_gripper_success(arm().armTargets.at("close"),2)) {
        //     isPreviousStateReached_=true;
        //     // std::cout << "arm is zero" << std::endl;
        // }
        // else {
        //     isPreviousStateReached_=false;
            arm().close_gripper();
            isPreviousStateReached_=true;
        // }
     }
};

void ArmFSM::react(MoveZero const &)
{
    if (isPreviousStateReached_)
    {
        std::cout << "Move2Zero" << std::endl;
        transit<Zero>();
    }
    else
    {
        std::cout << "Move2Zero failed" << std::endl;
    }
}

void ArmFSM::react(MoveHandle const &)
{
    if (isPreviousStateReached_)
    {
        std::cout << "Move2Handle" << std::endl;
        transit<Handle>();
    }
    else
    {
        std::cout << "Move2Handle failed" << std::endl;
    }
}

void ArmFSM::react(MoveHold const &)
{
    if (isPreviousStateReached_)
    {
        std::cout << "Move2Hold" << std::endl;
        transit<Hold>();
    }
    else
    {
        std::cout << "Move2Hold failed" << std::endl;
    }
}

void ArmFSM::react(MoveOpen const &)
{
    if (isPreviousStateReached_)
    {
        std::cout << "Move2Open" << std::endl;
        transit<Open>();
    }
    else
    {
        std::cout << "Move2Open failed" << std::endl;
    }
}

void ArmFSM::react(MoveClose const &)
{
    if (isPreviousStateReached_)
    {
        std::cout << "Move2Close" << std::endl;
        transit<Close>();
    }
    else
    {
        std::cout << "Move2Close failed" << std::endl;
    }
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




