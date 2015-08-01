#include "xhn_state_machine.hpp"
///==========================================================================///
///  state_data                                                              ///
///==========================================================================///
ImplementRootRTTI(xhn::state_data);
///==========================================================================///
///  state_argument                                                          ///
///==========================================================================///
ImplementRootRTTI(xhn::state_argument);
///==========================================================================///
///  state                                                                   ///
///==========================================================================///
void xhn::state::goto_state(const static_string state_name)
{
    m_owner->goto_state(state_name);
}

///==========================================================================///
///  state_machine                                                           ///
///==========================================================================///
void xhn::state_machine::update(double elapsedTime)
{
    m_current_state->update(elapsedTime);
}
void xhn::state_machine::goto_state(const static_string state_name)
{
    state* prev_state = m_current_state;
    state* next_state = m_states[state_name].get();
    state_argument_ptr arg = get_state_argument(state_name);
    previous_state_switch(state_name);
    prev_state->leave_state(next_state);
    next_state->enter_state(prev_state, arg);
    m_current_state = next_state;
    post_state_switch(prev_state->get_name());
}
