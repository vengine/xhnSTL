/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef xhn_concurrent_hpp
#define xhn_concurrent_hpp

#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include "xhn_static_string.hpp"
#include "xhn_dictionary.hpp"
#include "xhn_thread.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_lock.hpp"
#include "xhn_lambda.hpp"
#include "xhn_vector.hpp"
#include "rtti.hpp"
#include "cpu.h"

namespace xhn
{

class concurrent_variable : public RefObject
{
    DeclareRootRTTI;
public:
    virtual ~concurrent_variable() {}
};
    
typedef SmartPtr<concurrent_variable> concurrent_variable_ptr;
typedef dictionary<static_string, concurrent_variable_ptr> thread_local_variables;
    
class concurrent : public RefObject
{
private:
    struct concurrent_thread : public MemObject
    {
        thread_ptr thread;
        thread_local_variables thread_local_variables;
    };
public:
    SpinLock m_lock;
    vector<concurrent_thread*> m_unused_threads;
    vector<concurrent_thread*> m_used_threads;
public:
    concurrent()
    {
        for (euint i = 0; i < 2; i++) {
            thread_ptr t = VNEW thread();
            while (!t->is_running()) {}
            concurrent_thread* ct = VNEW concurrent_thread();
            ct->thread = t;
            m_unused_threads.push_back(ct);
        }
    }
    concurrent(euint num_threads)
    {
        if (num_threads < 2) num_threads = 2;
        for (euint i = 0; i < num_threads; i++) {
            thread_ptr t = VNEW thread();
            while (!t->is_running()) {}
            concurrent_thread* ct = VNEW concurrent_thread();
            ct->thread = t;
            m_unused_threads.push_back(ct);
        }
    }
    ~concurrent() {
        auto inst = m_lock.Lock();
        EAssert(m_used_threads.empty(), "when destructed concurrent, the used threads array must be empty!");
        for (auto ct : m_unused_threads) {
            VDELETE ct;
        }
    }
    bool execute(Lambda<void (thread_local_variables&)> proc)
    {
        auto inst = m_lock.Lock();
        if (!m_unused_threads.empty()) {
            concurrent_thread* ct = m_unused_threads.back();
            m_unused_threads.pop_back();
            Lambda<thread::TaskStatus ()> task([this, ct, proc]() -> thread::TaskStatus {
                proc(ct->thread_local_variables);
                {
                    auto inst = m_lock.Lock();
                    auto iter = m_used_threads.begin();
                    auto end = m_used_threads.end();
                    for (; iter != end; iter++) {
                        if (ct == *iter) {
                            m_used_threads.erase(iter);
                            break;
                        }
                    }
                    m_unused_threads.push_back(ct);
                }
                return thread::Completed;
            });
            m_used_threads.push_back(ct);
            ct->thread->add_lambda_task(task);
            return true;
        }
        else {
            return false;
        }
    }
    void waiting_for_completed() {
        while (true) {
            {
                auto inst = m_lock.Lock();
                if (m_used_threads.empty()) return;
            }
            nanopause(1000);
        }
    }
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
