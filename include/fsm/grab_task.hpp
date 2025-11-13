//
// Created by chang on 11/12/25.
//

#ifndef D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP
#define D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP

#include <string>
#include <vector>
#include <tinyfsm.hpp>
#include <fsm/arm_fsm.hpp>

// ----------------------------------------------------------------------------
// Event declarations
//

struct CancelEvent : tinyfsm::Event { };
struct GrabEvent : tinyfsm::Event { };
struct ReleaseEvent : tinyfsm::Event { };

// ----------------------------------------------------------------------------
// State declarations
//
class Reaching;
class Releasing;
class Idle;


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

    virtual void react(GrabEvent        const &) {};
    virtual void react(ReleaseEvent        const &){};
    virtual void react(CancelEvent        const &){};
    virtual void react(Tick const &) {  } ;

    virtual void entry(void) { };  /* entry actions in some states */
    virtual void exit(void)  { };  /* no exit actions at all */

protected:
    inline static int current_state = 0;
    inline static bool isReadyToNext = false;
    inline static bool lastPrevReached = false;

};



#endif //D1_ARM_SPEECH_CONTROL_GRAB_TASK_HPP