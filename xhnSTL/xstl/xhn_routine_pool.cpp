#include "xhn_routine_pool.hpp"
#ifdef __APPLE__
#include <unistd.h>
#include <limits.h>
#endif

ImplementRootRTTI(xhn::project);
ImplementRootRTTI(xhn::receipt);

bool xhn::routine::run()
{
    begin_project(m_project);
    m_receipt->complete();
    return true;
}

xhn::routine_pool::routine_pool(euint num_threads)
: m_thread_pool(num_threads)
{
}
xhn::routine_pool::~routine_pool()
{
}
xhn::receipt* xhn::routine_pool::alloc_routine(project_ptr proj)
{
    SpinLock::Instance inst = m_lock.Lock();
    if (m_unused_routines.size()) {
        thread::task_ptr& t = m_unused_routines.front();
        routine* r = (routine*)t.get();
        r->set_project(proj);
        m_thread_pool.add_task(t);
        m_unused_routines.pop_front();
        m_used_routines.insert(make_pair(r->get_receipt(), t));
        return r->get_receipt();
    }
    else {
        routine* r = m_routine_agent->create_routine();
        r->set_project(proj);
        thread::task_ptr t = r;
        m_thread_pool.add_task(t);
        m_used_routines.insert(make_pair(r->get_receipt(), t));
        return r->get_receipt();
    }
}
/// 一个事务结束后必须释放
void xhn::routine_pool::free_routine(receipt* routine_id)
{
    SpinLock::Instance inst = m_lock.Lock();
    map<receipt*, thread::task_ptr>::iterator iter = m_used_routines.find(routine_id);
    m_unused_routines.push_back(iter->second);
    m_used_routines.erase(iter);
}
void xhn::routine_pool::clear_routines()
{
    SpinLock::Instance inst = m_lock.Lock();
    map<receipt*, thread::task_ptr>::iterator iter = m_used_routines.begin();
    map<receipt*, thread::task_ptr>::iterator end = m_used_routines.end();
    for (; iter != end; iter++) {
        m_unused_routines.push_back(iter->second);
    }
    m_used_routines.clear();
    
}