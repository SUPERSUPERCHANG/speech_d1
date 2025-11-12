//
// Created by chang on 11/12/25.
//

#ifndef D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP
#define D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP

#include <string>
#include <tinyfsm.hpp>


// ----------------------------------------------------------------------------
// Event declarations
//
struct GrabEvent       : tinyfsm::Event{ };

struct ReleaseEvent       : tinyfsm::Event { };

struct CancelEvent : tinyfsm::Event { };

class ArmTaskFSM
: public tinyfsm::Fsm<ArmTaskFSM>
{
    /* NOTE: react(), entry() and exit() functions need to be accessible
     * from tinyfsm::Fsm class. You might as well declare friendship to
     * tinyfsm::Fsm, and make these functions private:
     *
     * friend class Fsm;
     */
public:

    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &) { };

    virtual void react(GrabEvent        const &);
    virtual void react(ReleaseEvent        const &);

    virtual void entry(void) { };  /* entry actions in some states */
    void         exit(void)  { };  /* no exit actions at all */

protected:
    static std::string current_state;
    static std::string reaching_state_series;
};



#endif //D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP