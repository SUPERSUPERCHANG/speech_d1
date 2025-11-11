//
// Created by chang on 11/10/25.
//

#ifndef D1_ARM_SPEECH_CONTROL_ARM_FSM_HPP
#define D1_ARM_SPEECH_CONTROL_ARM_FSM_HPP

#include <tinyfsm.hpp>
#include <iostream>

// ---------- Event ---------- //
struct MoveZero   : tinyfsm::Event { };
struct MoveHandle : tinyfsm::Event { };
struct MoveHold : tinyfsm::Event { };
struct MoveOpen : tinyfsm::Event {};
struct MoveClose  : tinyfsm::Event {};

// ---------- States ---------- //
class Zero;
class Hold;
class Handle;
class Open;
class Close;


class Arm
: public tinyfsm::Fsm<Arm>
{
    /* NOTE: react(), entry() and exit() functions need to be accessible
     * from tinyfsm::Fsm class. You might as well declare friendship to
     * tinyfsm::Fsm, and make these functions private:
     *
     * friend class Fsm;
     */
public:

    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &) { }; //do nothing for empty event

    /* non-virtual declaration: reactions are the same for all states */
    void react(MoveZero   const &);
    void react(MoveHandle const &);
    void react(MoveHold const &);
    void react(MoveOpen const &);
    void react(MoveClose const &);

    virtual void entry(void) = 0;  /* pure virtual: enforce implementation in all states */
    void exit(void)  { };          /* no exit actions at all */

    static std::string getCurrentStateName();

protected:



};



#endif //D1_ARM_SPEECH_CONTROL_ARM_FSM_HPP