//
// Created by chang on 11/10/25.
//

#ifndef D1_ARM_SPEECH_CONTROL_FSMLIST_HPP
#define D1_ARM_SPEECH_CONTROL_FSMLIST_HPP

#include <tinyfsm.hpp>
#include <fsm/arm_fsm.hpp>
#include <fsm/grab_task.hpp>

using fsm_list = tinyfsm::FsmList<ArmFSM,ArmTaskFSM>;

/** dispatch event to Arm */
template<typename E>
void send_event(E const & event)
{
    fsm_list::template dispatch<E>(event);
}





#endif //D1_ARM_SPEECH_CONTROL_FSMLIST_HPP