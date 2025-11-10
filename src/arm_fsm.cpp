//
// Created by chang on 11/10/25.
//
// src/main.cpp

#include "arm_fsm.hpp"
#include <tinyfsm.hpp>
#include <iostream>

class Zero
: public Arm
{
    void entry() override {
        std::cout << "Move to zero" << std::endl;
        currentMode_ = 0;
    };
};

class Hold
: public Arm
{
    void entry() override {
        std::cout << "Hold" << std::endl;
        currentMode_ = 1;
    };
};

class Handle
: public Arm
{
    void entry() override {
        std::cout << "Handle" << std::endl;
        currentMode_ = 2;
    };
};

class Open
: public Arm
{
    void entry() override {
        std::cout << "Open" << std::endl;
        currentMode_ = 3;
    };
};

class Close
: public Arm
{
    void entry() override {
        std::cout << "Close" << std::endl;
        currentMode_ = 4;
    };
};




