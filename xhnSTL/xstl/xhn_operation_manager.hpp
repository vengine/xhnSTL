//
//  xhn_operation_manager.h
//  x_duplicate_cleaner
//
//  Created by 徐海宁 on 14-1-26.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

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
