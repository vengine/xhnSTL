#ifndef XHN_STATE_MACHINE_HPP
#define XHN_STATE_MACHINE_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_map.hpp"
#include "xhn_static_string.hpp"
#include "xhn_smart_ptr.hpp"
#include "rtti.hpp"
namespace xhn
{
    /// \brief state_data
    ///
    ///
class state_data : public RefObject
{
    DeclareRootRTTI;
public:
    virtual ~state_data() {}
};
typedef SmartPtr<state_data> state_data_ptr;
    /// \brief state_argument
    ///
    ///
class state_argument : public RefObject
{
    DeclareRootRTTI;
public:
    virtual ~state_argument() {}
};
typedef SmartPtr<state_argument> state_argument_ptr;
    
class state_machine;
typedef SmartPtr<state_machine> state_machine_ptr;
    
    /// \brief state
    ///
    ///
class state : public RefObject
{
    DeclareRootRTTI;
protected:
    state_machine_ptr m_sub_state_machine;
    state_machine* m_owner;
public:
    state(state_machine* owner)
    : m_owner(owner)
    {}
    virtual ~state() {}
    void goto_state(const static_string state_name);
    virtual void init() = 0;
    virtual void update(double elapsedTime) = 0;
    virtual const static_string get_name() const = 0;
    virtual void leave_state(state* next_state) = 0;
    virtual void enter_state(state* prev_state, state_argument_ptr arg) = 0;
    virtual state_data_ptr get_data() = 0;
};
typedef SmartPtr<state> state_ptr;

    /// \brief state_machine
    ///
    ///
class state_machine : public RefObject
{
private:
    map<static_string, state_ptr> m_states;
    state* m_current_state;
protected:
    template <typename STATE_TYPE>
    STATE_TYPE* add_state() {
        STATE_TYPE* st = VNEW STATE_TYPE(this);
        st->init();
        m_states.insert(st->get_name(), st);
        return st;
    }
    template <typename STATE_TYPE>
    STATE_TYPE* add_state(state_argument_ptr argument) {
        STATE_TYPE* st = VNEW STATE_TYPE(this, argument);
        st->init();
        m_states.insert(st->get_name(), st);
        return st;
    }
    template <typename STATE_TYPE, typename ...ARGS>
    STATE_TYPE* add_state(ARGS&&... args) {
        STATE_TYPE* st = VNEW STATE_TYPE(this, args...);
        st->init();
        m_states.insert(st->get_name(), st);
        return st;
    }
    inline void set_current_state(const static_string state_name) {
        m_current_state = m_states[state_name].get();
    }
    inline state* get_current_state() {
        return m_current_state;
    }
    inline state* get_state(const static_string state_name) {
        return m_states[state_name].get();
    }
    inline const state* get_current_state() const {
        return m_current_state;
    }
    inline const state* get_state(const static_string state_name) const {
        map<static_string, state_ptr>::const_iterator iter = m_states.find(state_name);
        if (iter != m_states.end()) {
            return iter->second.get();
        }
        else {
            return NULL;
        }
    }
    inline euint number_of_states() const {
        return m_states.size();
    }
public:
    state_machine()
    : m_current_state(NULL)
    {}
    virtual ~state_machine() {}
    virtual void init() = 0;
    void update(double elapsedTime);
    void goto_state(const static_string state_name);
    virtual void previous_state_switch(const xhn::static_string next_state_name) {}
    virtual void post_state_switch(const xhn::static_string prev_state_name) {}
    virtual state_argument_ptr get_state_argument(const static_string state_name) {
        return NULL;
    }
    inline state_data_ptr get_current_data() {
        return m_current_state->get_data();
    }
};
}

#endif

#endif