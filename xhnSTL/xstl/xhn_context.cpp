//
//  xhn_context.cpp
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_context.h"
#include "xhn_device.h"
#include "xhn_platform.h"

void xhn::context::callback(const char *a, const void *b, size_t c, void *d)
{
    ///printf("here\n");
}

void xhn::context::initialize()
{
    cl_context temp = NULL;
    cl_command_queue queue = NULL;
    {
        SpinLock::Instance inst = m_lock.Lock();
        if (m_context)
            return;
        temp = m_context;
    }
    if (!temp) {
        cl_context_properties cps[3] =
        {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)m_device->get_platform()->get_id(),
            0
        };
        m_device_id = m_device->get_id();
        temp = clCreateContext(cps, 1, &m_device_id, callback, this, &m_errcode_ret);
        queue = clCreateCommandQueue(temp, m_device_id, 0, &m_errcode_ret);
    }
    {
        SpinLock::Instance inst = m_lock.Lock();
        if (!m_context) {
            m_context = temp;
            m_command_queue = queue;
        }
        else {
            clReleaseCommandQueue(queue);
            clReleaseContext(temp);
        }
    }
}

void xhn::context::destroy()
{
    m_kernels.clear();
    SpinLock::Instance inst = m_lock.Lock();
    if (m_context) {
        clReleaseCommandQueue(m_command_queue);
        clReleaseContext(m_context);
        m_command_queue = NULL;
        m_context = NULL;
    }
}

xhn::context::~context()
{
    destroy();
}

double xhn::context::launch_kernel(kernel_ptr kernel)
{
    kernel->prelaunch(kernel->m_global_work_offset,
                      kernel->m_global_work_size,
                      kernel->m_local_work_size);
    double ret = kernel->launch(kernel->m_execute_event);
    kernel->postlaunch();
    return ret;
}
