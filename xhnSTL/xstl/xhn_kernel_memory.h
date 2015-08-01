//
//  xhn_kernel_memory.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_kernel_memory__
#define ___zOpenCL__xhn_kernel_memory__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"

#include <OpenCL/cl.h>
#include "xhn_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_void_vector.hpp"
#include "xhn_smart_ptr.hpp"
namespace xhn
{
    class device;
    class platform;
    class context;
    class kernel;
    class kernel_memory : public RefObject
    {
    public:
        enum access
        {
            readonly,
            writeonly,
            readwrite,
        };
    private:
        void_vector m_host_memory;
        cl_mem m_device_memory;
        access m_access;
        kernel* m_kernel;
        bool m_use_as_image;
    public:
        void initialize(euint size, access access);
        void* lock(void* address, euint size);
        void unlock(void* address);
        kernel_memory(kernel* kernel, bool use_as_image)
        : m_device_memory(NULL)
        , m_access(readonly)
        , m_kernel(kernel)
        , m_use_as_image(use_as_image)
        {}
        ~kernel_memory();
        template <typename T>
        void write(const T& from, euint& offset) {
            vptr buffer = m_host_memory.get();
            ref_ptr end_address = (ref_ptr)buffer + offset + sizeof(T);
            if (end_address > (ref_ptr)m_host_memory.get_barrier()) {
                vptr address = (vptr)((ref_ptr)m_host_memory.get_barrier() - sizeof(T));
                address = lock(address, sizeof(T));
                memcpy(address, &from, sizeof(T));
                unlock(address);
            }
            else {
                vptr address = (vptr)((ref_ptr)buffer + offset);
                address = lock(address, sizeof(T));
                memcpy(address, &from, sizeof(T));
                unlock(address);
            }
            offset += sizeof(T);
        }
        template <typename T>
        void read(T& to, euint& offset) {
            vptr buffer = m_host_memory.get();
            ref_ptr end_address = (ref_ptr)buffer + offset + sizeof(T);
            if (end_address > (ref_ptr)m_host_memory.get_barrier()) {
                vptr address = (vptr)((ref_ptr)m_host_memory.get_barrier() - sizeof(T));
                memcpy(&to, address, sizeof(T));
            }
            else {
                vptr address = (vptr)((ref_ptr)buffer + offset);
                memcpy(&to, address, sizeof(T));
            }
            offset += sizeof(T);
        }
        void read_to_host();
        void fill_as_image(euint8 color);
        void write_to_device(const void* data, euint size);
        const cl_mem& get_cl_memory() {
            return m_device_memory;
        }
        vptr get_host_memory() {
            return m_host_memory.get();
        }
        void_vector* get_void_vector() {
            return &m_host_memory;
        }
        euint size();
    };
    typedef SmartPtr<kernel_memory> kernel_memory_ptr;
}

#endif /* defined(___zOpenCL__xhn_kernel_memory__) */
