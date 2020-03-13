/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_THREAD_HPP
#define XHN_THREAD_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_smart_ptr.hpp"
#include "xhn_list.hpp"
#include "xhn_string.hpp"
#include "xhn_static_string.hpp"
#include "xhn_lock.hpp"
#include "xhn_btree.hpp"
#include "xhn_lambda.hpp"
#include "timer.h"
namespace xhn
{
class thread;
typedef SmartPtr<thread> thread_ptr;
class XHN_EXPORT thread : public RefObjectBase, public MemObject
{
public:
    enum TaskStatus
    {
        Continue,
        Completed,
    };
public:
    static void milli_sleep(euint32 millisecond);
    static void micro_sleep(euint32 microsecond);
    static void nano_sleep(euint32 nanosecond);
public:
    class stack_range
    {
    public:
        vptr m_begin_addr;
        euint m_size;
        stack_range()
        : m_begin_addr(NULL)
        , m_size(0)
        {}
        inline bool is_in_range(vptr addr) {
            return (ref_ptr)addr >= (ref_ptr)m_begin_addr &&
            (ref_ptr)addr <= (ref_ptr)m_begin_addr + (ref_ptr)m_size;
        }
    };
	class task : public RefObjectBase, public MemObject
	{
	public:
        virtual const static_string type() const = 0;
		virtual ~task() {}
        /// 返回真表示执行结束
		virtual bool run() = 0;
	};
	typedef SmartPtr<task> task_ptr;
    class lambda_task : public task
    {
    public:
        STATIC_CONST static_string StrLambdaTask;
    public:
        Lambda<TaskStatus ()> m_lambda;
    public:
        virtual const static_string type() const;
        virtual bool run();
        void set_lambda(Lambda<TaskStatus ()>& lambda);
    };
    class ThreadAllocator
    {
    public:
        Unlocked_mem_allocator* m_base_allocator;
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef list_node<task_ptr>* pointer;
        typedef const list_node<task_ptr>* const_pointer;
        typedef list_node<task_ptr> value_type;
        typedef list_node<task_ptr>& reference;
        typedef const list_node<task_ptr>& const_reference;
    public:
        ThreadAllocator()
        {
            m_base_allocator = UnlockedMemAllocator_new(&g_DefaultMemoryAllcator);
        }
        ~ThreadAllocator()
        {
            UnlockedMemAllocator_delete(m_base_allocator);
        }
        void deallocate(pointer ptr, size_type)
        {
            UnlockedMemAllocator_free(m_base_allocator, ptr);
        }
        
        pointer allocate(size_type count)
        {
            return (pointer)UnlockedMemAllocator_alloc(m_base_allocator, count * sizeof(value_type), false);
        }
        
        pointer allocate(size_type count, const void*)
        {
            return (pointer)UnlockedMemAllocator_alloc(m_base_allocator, count * sizeof(value_type), false);
        }
        
        void construct(pointer ptr, const_reference v)
        {
            new (ptr) list_node<task_ptr>(v);
        }
        
        void construct(pointer ptr, const task_ptr& v)
        {
            new ( ptr ) list_node<task_ptr> (v);
        }
        
        void destroy(pointer ptr)
        {
            ptr->~list_node<task_ptr>();
        }
    };
public:
	///SpinLock m_taskLock;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    pthread_t m_pt;
    stack_range m_stack_range;
    /// 这是一组的
	list<task_ptr, ThreadAllocator> m_tasks;
    volatile esint32 m_is_completed;
    
    volatile esint32 m_is_finished;
    volatile esint32 m_is_running;
	
	volatile esint32 m_is_stopped;
	
    volatile esint32 m_is_errored;
    xhn::string m_error_message;
private:
	static void* thread_proc(void* self);
public:
    thread();
	~thread();
    void move_to_realtime_scheduling_class();
	void run();
	void add_task(task_ptr t);
    void add_lambda_task(Lambda<TaskStatus ()>& lambda);
	bool is_completed() {
		return AtomicCompareExchange(1, 1, &m_is_completed);
	}
    bool is_errored() {
        return AtomicCompareExchange(1, 1, &m_is_errored);
    }
    void stop();
    void reset();
    bool is_stopped() {
        return AtomicCompareExchange(1, 1, &m_is_stopped);
    }
    bool is_running() {
        return AtomicCompareExchange(1, 1, &m_is_running);
    }
    euint num_tasks();
    void force_stop();
    void force_restart();
    void wait_completed();
    void join();
    void clear();
    bool I_am_this_thread();
    bool has_tasks(const set<static_string>& task_types);
    const xhn::string& get_error_message() {
        return m_error_message;
    }
    static void stop_thread_and_join(SmartPtr<thread>& t);
    static void assign_to_local_thread(thread_ptr& local_thread, thread_ptr& global_thread, SpinLock& lock);
};

}

#endif

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
