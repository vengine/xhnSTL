#ifndef XHN_THREAD_POOL_HPP
#define XHN_THREAD_POOL_HPP
#include "common.h"
#include "etypes.h"
#include "xhn_thread.hpp"
#include "xhn_vector.hpp"
#include "xhn_set.hpp"
#include "xhn_lambda.hpp"
namespace xhn
{
///==========================================================================///
///  concurrent_thread_pool                                                  ///
///==========================================================================///
class concurrent_thread_pool : public RefObject
{
public:
    vector<thread_ptr> m_threads;
public:
    concurrent_thread_pool(euint num_threads);
    ~concurrent_thread_pool();
    void add_task(thread::task_ptr t);
    void clear();
};
typedef SmartPtr<concurrent_thread_pool> concurrent_thread_pool_ptr;
    /**
///==========================================================================///
///  transaction                                                             ///
///==========================================================================///
    
class transaction_pool;
typedef WeakPtr<transaction_pool> transaction_pool_weak_ptr;
typedef SmartPtr<transaction_pool> transaction_pool_ptr;

class transaction : public RefObject
{
    friend class SmartPtr<transaction>;
    friend class transaction_pool;
private:
    transaction_pool_weak_ptr m_owner;
    vector<thread_ptr> m_threads;
private:
    transaction(transaction_pool_ptr owner);
    ~transaction();
public:
    void resize(euint num_threads);
    void clear();
    void for_each(Lambda<void(thread_ptr&, bool*)>& proc);
};
typedef SmartPtr<transaction> transaction_ptr;
    
///==========================================================================///
///  transaction_pool                                                        ///
///==========================================================================///
    
class transaction_pool : public RefObject
{
    friend class transaction;
public:
    list<thread_ptr> m_thread_pool;
private:
    thread_ptr alloc_thread();
    void free_thread(thread_ptr thread);
public:
    ~transaction_pool();
    transaction_ptr new_transaction();
};
**/
}
#endif