/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef xhn_parallel_hpp
#define xhn_parallel_hpp

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_thread.hpp"
#include "xhn_vector.hpp"
#include "cpu.h"

namespace xhn
{

class parallel : public MemObject
{
public:
    mutable vector<thread_ptr> m_threads;
public:
    parallel()
    {
        thread_ptr t = VNEW thread();
        while (!t->is_running()) {}
        m_threads.push_back(t);
    }
    parallel(euint num_threads)
    {
        for (euint i = 0; i < num_threads; i++) {
            thread_ptr t = VNEW thread();
            while (!t->is_running()) {}
            m_threads.push_back(t);
        }
    }
    template <typename FUNC>
    void parallel_for(euint start, euint end, FUNC func) const
    {
        if (start >= end)
            return;
        euint works = end - start;
        volatile esint64 worked_thread_count = m_threads.size();
        euint num_threads = m_threads.size() + 1;
        euint works_per_thread = works / num_threads;
        euint remainder_works = works % num_threads;
        if (works_per_thread > 0) {
            euint a = start;
            euint b = start + works_per_thread;
            for (auto& t : m_threads) {
                Lambda<thread::TaskStatus ()>
                proc([a, b, &func, &worked_thread_count]() -> thread::TaskStatus {
                    func(a, b);
                    AtomicDecrement(&worked_thread_count);
                    return thread::Completed;
                });
                t->add_lambda_task(proc);
                
                a += works_per_thread;
                b = a + works_per_thread;
            }
            func(a, b);
            if (remainder_works) {
                a += works_per_thread;
                b = a + remainder_works;
                func(a, b);
            }
            while (AtomicLoad64(&worked_thread_count)) {
                nanopause(1000);
            }
        }
        else {
            func(start, end);
        }
    }
    template <typename FUNC>
    void parallel_for_async(euint start, euint end, FUNC func) const
    {
        if (start >= end)
            return;
        euint works = end - start;
        euint num_threads = m_threads.size() + 1;
        euint works_per_thread = works / num_threads;
        euint remainder_works = works % num_threads;
        if (works_per_thread > 0) {
            euint a = start;
            euint b = start + works_per_thread;
            for (auto& t : m_threads) {
                Lambda<thread::TaskStatus ()>
                proc([a, b, &func]() -> thread::TaskStatus {
                    func(a, b);
                    return thread::Completed;
                });
                t->add_lambda_task(proc);
                
                a += works_per_thread;
                b = a + works_per_thread;
            }
            func(a, b);
            if (remainder_works) {
                a += works_per_thread;
                b = a + remainder_works;
                func(a, b);
            }
        }
        else {
            func(start, end);
        }
    }
    bool is_completed() const {
        for (auto& t : m_threads) {
            if (!t->is_completed()) {
                return false;
            }
        }
        return true;
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
