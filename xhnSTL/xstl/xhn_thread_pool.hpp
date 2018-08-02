/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_THREAD_POOL_HPP
#define XHN_THREAD_POOL_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_thread.hpp"
#include "xhn_vector.hpp"
#include "xhn_set.hpp"
#include "xhn_lambda.hpp"
namespace xhn
{
    /// \brief concurrent_thread_pool
    ///
    ///
class concurrent_thread_pool : public RefObjectBase, public MemObject
{
public:
    vector<thread_ptr> m_threads;
public:
    concurrent_thread_pool(euint num_threads);
    ~concurrent_thread_pool();
    thread_ptr add_task(thread::task_ptr t);
    void clear();
    thread_ptr search_current_thread();
};
typedef SmartPtr<concurrent_thread_pool> concurrent_thread_pool_ptr;
    /**
    
class transaction_pool;
typedef WeakPtr<transaction_pool> transaction_pool_weak_ptr;
typedef SmartPtr<transaction_pool> transaction_pool_ptr;

class transaction : public RefObjectBase, public MemObject
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
    
    
class transaction_pool : public RefObjectBase, public MemObject
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
