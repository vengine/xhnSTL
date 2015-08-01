//
//  xhn_kernel_memory.cpp
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_kernel_memory.h"
#include "xhn_kernel.h"
#include "xhn_context.h"

namespace
{
    const char* get_error_information(cl_int error)
    {
        switch (error)
        {
            case CL_SUCCESS:
                return "CL_SUCCESS";
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
            case CL_INVALID_IMAGE_DESCRIPTOR:
                return "CL_INVALID_IMAGE_DESCRIPTOR";
            case CL_INVALID_MEM_OBJECT:
                return "CL_INVALID_MEM_OBJECT";
            case CL_INVALID_VALUE:
            default:
                return "CL_INVALID_VALUE";
        }
    }
}

void xhn::kernel_memory::initialize(euint size, access access)
{
    cl_mem_flags flags = 0;
    switch (access)
    {
        case readonly:
            flags = CL_MEM_READ_ONLY;
            break;
        case writeonly:
            flags = CL_MEM_WRITE_ONLY;
            break;
        case readwrite:
        default:
            flags = CL_MEM_READ_WRITE;
            break;
    }
    m_host_memory.convert<void_vector::FCharProc>(1);
    m_host_memory.resize(size);
    if (!m_use_as_image) {
        m_device_memory = clCreateBuffer(m_kernel->get_context()->get_cl_context(),
                                         flags | CL_MEM_USE_HOST_PTR,
                                         size,
                                         m_host_memory.get(),
                                         NULL);
    }
    else {
        cl_image_format format = {CL_R, CL_UNSIGNED_INT8};
        cl_image_desc desc =
        {
            CL_MEM_OBJECT_IMAGE1D,
            size,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL
        };
        cl_int error = 0;
        m_device_memory =
        clCreateImage(m_kernel->get_context()->get_cl_context(),
                      flags | CL_MEM_USE_HOST_PTR,
                      &format,
                      &desc,
                      m_host_memory.get(),
                      &error);

        printf("%s\n", get_error_information(error));
    }
    m_access = access;
}

void* xhn::kernel_memory::lock(void* address, euint size)
{
    euint offset = (ref_ptr)address - (ref_ptr)m_host_memory.get();
    cl_map_flags flags = 0;
    switch (m_access)
    {
        case readonly:
            flags = CL_MAP_READ;
            break;
        case writeonly:
            flags = CL_MAP_WRITE;
            break;
        case readwrite:
        default:
            flags = CL_MAP_WRITE_INVALIDATE_REGION;
            break;
    };
    if (!m_use_as_image) {
        cl_int error = 0;
        void* ret = clEnqueueMapBuffer(m_kernel->get_context()->get_command_queue(),
                                       m_device_memory,
                                       CL_TRUE,
                                       flags,
                                       offset,
                                       size,
                                       0,
                                       NULL, NULL, &error);
        if (error != CL_SUCCESS) {
            printf("%s\n", get_error_information(error));
        }
        return ret;
    }
    else {
        size_t origin[3] = {offset, 0, 0};
        size_t region[3] = {size, 1, 1};
        size_t image_row_pitch = 0;
        size_t image_slice_pitch = 0;
        cl_int error = 0;
        void* ret = clEnqueueMapImage(m_kernel->get_context()->get_command_queue(),
                                      m_device_memory,
                                      CL_TRUE,
                                      flags,
                                      origin,
                                      region,
                                      &image_row_pitch,
                                      &image_slice_pitch,
                                      0, NULL, NULL, &error);
        if (error != CL_SUCCESS) {
            printf("%s\n", get_error_information(error));
        }
        return ret;
    }
}
void xhn::kernel_memory::unlock(void* address)
{
    clEnqueueUnmapMemObject(m_kernel->get_context()->get_command_queue(),
                            m_device_memory,
                            address,
                            0,
                            NULL, NULL);
}

xhn::kernel_memory::~kernel_memory()
{
    if (m_device_memory)
        clReleaseMemObject(m_device_memory);
}

void xhn::kernel_memory::read_to_host()
{
    void* address = clEnqueueMapBuffer(m_kernel->get_context()->get_command_queue(),
                                       m_device_memory,
                                       CL_TRUE,
                                       CL_MAP_READ,
                                       0,
                                       m_host_memory.size(),
                                       0,
                                       NULL, NULL, NULL);
    
    clEnqueueUnmapMemObject(m_kernel->get_context()->get_command_queue(),
                            m_device_memory,
                            address,
                            0,
                            NULL, NULL);
}

void xhn::kernel_memory::fill_as_image(euint8 color)
{
    if (m_use_as_image) {
        cl_int error = 0;
        size_t origin[3] = {0, 0, 0};
        size_t region[3] = {m_host_memory.size(), 1, 1};
        error =
        clEnqueueFillImage(m_kernel->get_context()->get_command_queue(),
                           m_device_memory,
                           &color,
                           origin,
                           region,
                           0,
                           NULL,
                           NULL);
        if (error != CL_SUCCESS) {
            printf("%s\n", get_error_information(error));
        }
    }
}

void xhn::kernel_memory::write_to_device(const void* data, euint size)
{
    if (!m_use_as_image) {
        size = (size < m_host_memory.size()) ? size : m_host_memory.size();
        vptr buffer = m_host_memory.get();
        vptr address = lock(buffer, size);
        memcpy(address, data, size);
        unlock(address);
    }
    else {
        cl_int error = 0;
        size = (size < m_host_memory.size()) ? size : m_host_memory.size();
        size_t origin[3] = {0, 0, 0};
        size_t region[3] = {size, 1, 1};
        error =
        clEnqueueWriteImage(m_kernel->get_context()->get_command_queue(),
                            m_device_memory,
                            CL_TRUE,
                            origin,
                            region,
                            m_host_memory.size(),
                            0,
                            data,
                            0,
                            NULL,
                            NULL);
        if (error != CL_SUCCESS) {
            printf("%s\n", get_error_information(error));
        }
    }
}

euint xhn::kernel_memory::size()
{
    return m_host_memory.size();
}

