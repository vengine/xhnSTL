#ifndef XHN_ROUTINE_POOL_HPP
#define XHN_ROUTINE_POOL_HPP
#include "common.h"
#include "etypes.h"
#include "xhn_thread_pool.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_list.hpp"
#include "xhn_map.hpp"
#include "xhn_lock.hpp"
#include "rtti.hpp"
namespace xhn
{
/// project 是数据，routine 是执行过程，receipt 是回执
///==========================================================================///
///  project                                                                 ///
///==========================================================================///
class project : public RefObject
{
    DeclareRootRTTI;
public:
    virtual ~project() {}
};
typedef SmartPtr<project> project_ptr;
///==========================================================================///
///  routine                                                                 ///
///==========================================================================///
class receipt;
typedef SmartPtr<receipt> receipt_ptr;
class routine : public thread::task
{
protected:
    receipt_ptr m_receipt;
    project_ptr m_project;
public:
    routine(receipt_ptr rec)
    : m_receipt(rec)
    {}
    virtual ~routine() {}
    inline void set_project(project_ptr proj) {
        m_project = proj;
    }
    inline receipt* get_receipt() {
        return m_receipt.get();
    }
    virtual void begin_project(project_ptr proj) = 0;
    virtual float get_progress_rate() = 0;
    virtual bool run();
};
///==========================================================================///
///  receipt                                                                 ///
///==========================================================================///
class receipt : public RefObject
{
    DeclareRootRTTI;
public:
    virtual ~receipt() {}
    virtual void complete() = 0;
};
typedef SmartPtr<receipt> receipt_ptr;
///==========================================================================///
///  routine_agent                                                           ///
///==========================================================================///
class routine_agent : public RefObject
{
public:
    virtual ~routine_agent() {}
    virtual routine* create_routine() = 0;
};
typedef SmartPtr<routine_agent> routine_agent_ptr;
///==========================================================================///
///  routine_pool                                                            ///
///==========================================================================///
class routine_pool : public RefObject
{
public:
    SpinLock m_lock;
    concurrent_thread_pool m_thread_pool;
    routine_agent_ptr m_routine_agent;
    list<thread::task_ptr> m_unused_routines;
    map<receipt*, thread::task_ptr> m_used_routines;
public:
    routine_pool(euint num_threads);
    virtual ~routine_pool();
    inline void set_routine_agent(routine_agent_ptr agent) {
        m_routine_agent = agent;
    }
    receipt* alloc_routine(project_ptr proj);
    /// 一个事务结束后必须释放
    void free_routine(receipt* routine_id);
    void clear_routines();
};
}
#endif