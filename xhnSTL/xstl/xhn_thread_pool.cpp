#include "xhn_thread_pool.hpp"
#ifdef __APPLE__
#include <unistd.h>
#include <limits.h>
#endif
///==========================================================================///
///  concurrent_thread_pool                                                  ///
///==========================================================================///
xhn::concurrent_thread_pool::concurrent_thread_pool(euint num_threads)
{
    for (euint i = 0; i < num_threads; i++) {
        thread* t = VNEW thread;
        t->reset();
        while (!t->is_running()) {}
        m_threads.push_back(t);
    }
}
xhn::concurrent_thread_pool::~concurrent_thread_pool()
{
    vector<thread_ptr>::iterator iter = m_threads.begin();
    vector<thread_ptr>::iterator end = m_threads.end();
    for (; iter != end; iter++) {
        thread_ptr t = *iter;
        thread::stop_thread_and_join(t);
    }
}
void xhn::concurrent_thread_pool::add_task(thread::task_ptr t)
{
    euint least_tasks = 0;
    thread* least_tasks_thread = NULL;
    vector<thread_ptr>::iterator iter = m_threads.begin();
    vector<thread_ptr>::iterator end = m_threads.end();
    for (; iter != end; iter++) {
        thread_ptr thr = *iter;
        if (!least_tasks_thread) {
            least_tasks = thr->num_tasks();
            least_tasks_thread = thr.get();
        }
        else {
            if (thr->num_tasks() < least_tasks) {
                least_tasks = thr->num_tasks();
                least_tasks_thread = thr.get();
            }
        }
    }
    if (least_tasks_thread)
        least_tasks_thread->add_task(t);
}
void xhn::concurrent_thread_pool::clear()
{
    vector<thread_ptr>::iterator iter = m_threads.begin();
    vector<thread_ptr>::iterator end = m_threads.end();
    for (; iter != end; iter++) {
        thread_ptr t = *iter;
        t->clear();
    }
}
/**
///==========================================================================///
///  transaction                                                             ///
///==========================================================================///
xhn::transaction::transaction(transaction_pool_ptr owner)
{
    owner.ToWeakPtr(m_owner);
}
xhn::transaction::~transaction()
{
    clear();
}
void xhn::transaction::resize(euint num_threads)
{
    transaction_pool_ptr owner = m_owner.ToStrongPtr();
    if (owner) {
        if (num_threads > m_threads.size()) {
            euint num_added_threads = num_threads - m_threads.size();
            for (euint i = 0; i < num_added_threads; i++) {
                m_threads.push_back(owner->alloc_thread());
            }
        }
        else if (num_threads < m_threads.size()) {
            euint num_removed_threads = m_threads.size() - num_threads;
            vector<thread_ptr>::reverse_iterator iter = m_threads.rbegin();
            vector<thread_ptr>::reverse_iterator end = m_threads.rend();
            for (euint i = 0; i < num_removed_threads && iter != end;iter++, i++) {
                owner->free_thread(*iter);
            }
            m_threads.resize(num_threads);
        }
    }
}
void xhn::transaction::clear()
{
    transaction_pool_ptr owner = m_owner.ToStrongPtr();
    if (owner) {
        vector<thread_ptr>::iterator iter = m_threads.begin();
        vector<thread_ptr>::iterator end = m_threads.end();
        for (; iter != end; iter++) {
            owner->free_thread(*iter);
        }
    }
}
void xhn::transaction::for_each(Lambda<void(thread_ptr&, bool*)>& proc)
{
    vector<thread_ptr>::iterator iter = m_threads.begin();
    vector<thread_ptr>::iterator end = m_threads.end();
    for (; iter != end; iter++) {
        bool stop = false;
        proc(*iter, &stop);
        if (stop)
            break;
    }
}

///==========================================================================///
///  transaction_pool                                                        ///
///==========================================================================///
xhn::transaction_pool::~transaction_pool()
{
    list<thread_ptr>::iterator iter = m_thread_pool.begin();
    list<thread_ptr>::iterator end = m_thread_pool.end();
    for (; iter != end; iter++) {
        while (!(*iter)->is_stopped()) {
            (*iter)->stop();
        }
        (*iter)->join();
    }
    m_thread_pool.clear();
}

xhn::thread_ptr xhn::transaction_pool::alloc_thread()
{
    if (m_thread_pool.size()) {
        thread_ptr ret = m_thread_pool.front();
        m_thread_pool.pop_front();
        return ret;
    }
    else {
        thread_ptr ret = VNEW thread;
        ret->reset();
        while (!ret->is_running()) {}
        return ret;
    }
}
void xhn::transaction_pool::free_thread(xhn::thread_ptr thread)
{
    thread->force_restart();
    m_thread_pool.push_back(thread);
}

xhn::transaction_ptr xhn::transaction_pool::new_transaction()
{
    transaction_ptr ret = VNEW transaction(this);
    return ret;
}
**/