//
//  xhn_context.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_context__
#define ___zOpenCL__xhn_context__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"

#include <OpenCL/cl.h>
#include "xhn_lock.hpp"
#include "xhn_kernel.h"
namespace xhn
{
    class device;
    class platform;
    class kernel;
    class context : public RefObject
    {
    private:
        static void callback(const char *a, const void *b, size_t c, void *d);
    private:
        SpinLock m_lock;
        cl_context m_context;
        cl_command_queue m_command_queue;
        cl_int m_errcode_ret;
        device* m_device;
        cl_device_id m_device_id;
        vector<kernel_ptr> m_kernels;
    public:
        void initialize();
        void destroy();
        context(device* device)
        : m_context(NULL)
        , m_command_queue(NULL)
        , m_errcode_ret(0)
        , m_device(device)
        , m_device_id(0)
        {}
        ~context();
        inline cl_context get_cl_context() {
            return m_context;
        }
        inline cl_command_queue get_command_queue() {
            return m_command_queue;
        }
        inline device* get_device() {
            return m_device;
        }
        template <typename KERNEL_TYPE>
        kernel_ptr create_kernel() {
            kernel_ptr ret = VNEW KERNEL_TYPE(this);
            ret->initialize();
            SpinLock::Instance inst = m_lock.Lock();
            m_kernels.push_back(ret);
            return ret;
        }
        template <typename KERNEL_TYPE, typename PARAMETER>
        kernel_ptr create_kernel(PARAMETER arg) {
            kernel_ptr ret = VNEW KERNEL_TYPE(this, arg);
            ret->initialize();
            SpinLock::Instance inst = m_lock.Lock();
            m_kernels.push_back(ret);
            return ret;
        }
        template <typename KERNEL_TYPE, typename PARAMETER0, typename PARAMETER1>
        kernel_ptr create_kernel(PARAMETER0 arg0, PARAMETER1 arg1) {
            kernel_ptr ret = VNEW KERNEL_TYPE(this, arg0, arg1);
            ret->initialize();
            SpinLock::Instance inst = m_lock.Lock();
            m_kernels.push_back(ret);
            return ret;
        }
        double launch_kernel(kernel_ptr kernel);
    };
    typedef SmartPtr<context> context_ptr;
}

#endif /* defined(___zOpenCL__xhn_context__) */
