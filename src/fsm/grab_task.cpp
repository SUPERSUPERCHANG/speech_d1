//
// Created by chang on 11/12/25.
//

#include "fsm/grab_task.hpp"
#include "fsm/grab_task.hpp"

#include <iostream>
#include <ostream>

class Idle; // forward declaration

// ----------------------------------------------------------------------------
// State: Reaching
//

class Reaching:public ArmTaskFSM
{

    void entry() override
    {
        std::cout << "entry to Reaching" << std::endl;
    }
    void react(GrabEvent const & e) override
    {

    }
    void react(ReleaseEvent const & e) override
    {

    }
    void react(CancelEvent const & e) override
    {

    }
};

class Releasing:public ArmTaskFSM
{
    void entry() override
    {

    }
    void react(GrabEvent const & e) override
    {

    }
    void react(ReleaseEvent const & e) override
    {

    }
    void react(CancelEvent const & e) override
    {

    }
};
