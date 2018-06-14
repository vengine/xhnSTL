/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_REALTIME_THREAD_HPP
#define XHN_REALTIME_THREAD_HPP

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
    
class realtime_thread : public RefObject
{
public:
    class realtime_task : public RefObject
    {
    public:
        virtual const static_string type() const = 0;
        virtual ~realtime_task() {}
        virtual void run_once(double elapsedTime) = 0;
    };
    typedef SmartPtr<realtime_task> realtime_task_ptr;
    class realtime_task_pool : public RefObject
    {
    private:
        SpinObject<vector<realtime_task_ptr>> m_tasks;
    public:
        SpinObject<vector<realtime_task_ptr>>& GetTasks() {
            return m_tasks;
        }
    };
    typedef SmartPtr<realtime_task_pool> realtime_task_pool_ptr;
private:
    static void* realtime_proc(void* thread);
private:
    realtime_task_pool_ptr m_task_pool;
    double m_time_interval;
    pthread_t m_pt;
    TimeCheckpoint m_last_time_checkpoint;
    volatile esint32 m_is_running;
    volatile esint32 m_is_completed;
public:
    realtime_thread(realtime_task_pool_ptr task_pool,
                    double time_interval)
    : m_task_pool(task_pool)
    , m_time_interval(time_interval)
    , m_is_running(0)
    , m_is_completed(0)
    {}
    virtual ~realtime_thread();
    void run();
};
typedef SmartPtr<realtime_thread> realtime_thread_ptr;
    
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

