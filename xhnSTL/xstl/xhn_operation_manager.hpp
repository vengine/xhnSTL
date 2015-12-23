/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef __xhn_operation_manager__
#define __xhn_operation_manager__

#include "xhn_thread.hpp"
#include "xhn_vector.hpp"
#include "rtti.hpp"
#include "xhn_smart_ptr.hpp"
namespace xhn
{
class operation_data : public RefObject
{
    DeclareRootRTTI;
};
typedef SmartPtr<operation_data> operation_data_ptr;
    
class operation_handle;
class operation : public thread::task
{
public:
    operation_handle* m_handle;
public:
    operation()
    : m_handle(nullptr)
    {}
    virtual ~operation();
    void complete();
    inline void set_handle(operation_handle* handle) {
        m_handle = handle;
    }
    virtual float get_progress_rate() = 0;
    virtual void send_data(operation_handle* handle) = 0;
    virtual void cancel() = 0;
};
    
class operation_handle : public RefObject
{
private:
    thread::task_ptr m_operation;
    operation_data_ptr m_data;
protected:
    virtual void process_impl(operation_data_ptr data) = 0;
public:
    operation_handle(operation* op)
    : m_operation(op) {
        op->set_handle(this);
    }
    virtual ~operation_handle() {}
    inline void process() {
        process_impl(m_data);
        m_data = nullptr;
    }
    inline void receive(operation_data_ptr data) {
        m_data = data;
    }
    inline bool is_completed() {
        return m_data;
    }
    float get_progress_rate();
    void cancel();
};
typedef SmartPtr<operation_handle> operation_handle_ptr;
    
class operation_manager : public RefObject
{
private:
    static operation_manager* s_operation_manager;
private:
    vector<thread_ptr> m_thread_pool;
public:
    operation_manager();
    ~operation_manager();
    static operation_manager* get();
    void add_operatoion(operation* op);
    template <typename OP, typename H>
    H* add_operatoion() {
        OP* op = VNEW OP;
        H* h = VNEW H(op);
        add_operatoion(op);
        return h;
    }
    template <typename OP, typename H, typename P>
    H* add_operatoion(P p) {
        OP* op = VNEW OP(p);
        H* h = VNEW H(op);
        add_operatoion(op);
        return h;
    }
    template <typename OP, typename H, typename P0, typename P1>
    H* add_operatoion(P0 p0, P1 p1) {
        OP* op = VNEW OP(p0);
        H* h = VNEW H(op, p1);
        add_operatoion(op);
        return h;
    }
    template <typename OP, typename H, typename P0, typename P1>
    H* add_operatoion2(P0 p0, P1 p1) {
        OP* op = VNEW OP(p0, p1);
        H* h = VNEW H(op);
        add_operatoion(op);
        return h;
    }
};
}

#endif /* defined(__xhn_operation_manager__) */

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
