/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_ROUTINE_POOL_HPP
#define XHN_ROUTINE_POOL_HPP

#ifdef __cplusplus

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
    /// \brief project
    ///
    /// project 是数据，routine 是执行过程，receipt 是回执
class project : public RefObjectBase, public MemObject
{
    DeclareRootRTTI;
public:
    virtual ~project() {}
};
typedef SmartPtr<project> project_ptr;

class receipt;
typedef SmartPtr<receipt> receipt_ptr;
    /// \brief routine
    ///
    /// project 是数据，routine 是执行过程，receipt 是回执
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
    /// \brief receipt
    ///
    /// project 是数据，routine 是执行过程，receipt 是回执
class receipt : public RefObjectBase, public MemObject
{
    DeclareRootRTTI;
public:
    virtual ~receipt() {}
    virtual void complete() = 0;
};
typedef SmartPtr<receipt> receipt_ptr;
    /// \brief routine_agent
    ///
    ///
class routine_agent : public RefObjectBase, public MemObject
{
public:
    virtual ~routine_agent() {}
    virtual routine* create_routine() = 0;
};
typedef SmartPtr<routine_agent> routine_agent_ptr;
    /// \brief routine_pool
    ///
    ///
class routine_pool : public RefObjectBase, public MemObject
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
