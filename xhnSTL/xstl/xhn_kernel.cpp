//
//  xhn_kernel.cpp
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_kernel.h"
#include "xhn_context.h"
#include "xhn_device.h"
#include "timer.h"

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
            case CL_INVALID_WORK_GROUP_SIZE:
                return "CL_INVALID_WORK_GROUP_SIZE";
            case CL_INVALID_VALUE:
            default:
                return "CL_INVALID_VALUE";
        }
    }
}

void xhn::kernel::callback(cl_program program , void * user_data)
{
}

void xhn::kernel::initialize()
{
    make_host_interface();
    cl_int errcode_ret = 0;
    string libs;
    vector<library_ptr>::iterator libIter = m_dependent_librarys.begin();
    vector<library_ptr>::iterator libEnd = m_dependent_librarys.end();
    for (; libIter != libEnd; libIter++) {
        libs += (*libIter)->make_source_code();
    }
    string opencl_source_code = make_opencl_source_code();
    opencl_source_code = libs + opencl_source_code;
    string kernel_name = get_kernel_name();
    string preprocessor_options = get_preprocessor_options();
    const char* str = opencl_source_code.c_str();
    size_t size = opencl_source_code.size();
    m_program = clCreateProgramWithSource(m_context->get_cl_context(),
                                          1,
                                          &str,
                                          &size,
                                          &errcode_ret);
    cl_device_id device_id = m_context->get_device()->get_id();
    cl_int status = clBuildProgram(m_program, 1, &device_id, preprocessor_options.c_str(), callback, NULL);
    if(status != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(m_program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    m_kernel = clCreateKernel(m_program, kernel_name.c_str(), &errcode_ret);
    
    vector<argument_entry>::iterator iter = m_arguments.begin();
    vector<argument_entry>::iterator end = m_arguments.end();
    for (cl_uint i = 0; iter != end; iter++, i++) {
        argument_entry& entry = *iter;
        if (entry.type == value) {
            range<euint>& range = m_main_argument_map[entry.name];
            clSetKernelArg(m_kernel,
                           i,
                           range.end_addr - range.begin_addr,
                           m_main_argument_buffer[range.begin_addr]);
        }
        else {
            kernel_memory_ptr memory = m_host_memory_map[entry.name];
            clSetKernelArg(m_kernel,
                           i,
                           sizeof(cl_mem),
                           &memory->get_cl_memory());
        }
    }
}

xhn::kernel::~kernel()
{
    if (m_kernel) {
        clReleaseKernel(m_kernel);
        m_kernel = NULL;
    }
    if (m_program) {
        clReleaseProgram(m_program);
        m_program = NULL;
    }
}

xhn::kernel::memory_handle
xhn::kernel::add_memory_parameter(const string& name,
                                  euint memory_size,
                                  kernel_memory::access access)
{
    xhn::kernel::memory_handle ret(NULL);
    kernel_memory_ptr kernel_memory = VNEW class kernel_memory(this, false);
    kernel_memory->initialize(memory_size, access);
    map<string, kernel_memory_ptr>::iterator iter =
    m_host_memory_map.insert(make_pair(name, kernel_memory));
    if (iter != m_host_memory_map.end()) {
        ret.m_kernel_memory = iter->second;
    }
    argument_entry entry = {memory, name};
    m_arguments.push_back(entry);
    return ret;
}

xhn::kernel::memory_handle
xhn::kernel::add_memory_parameter(const string& name,
                                  memory_handle& handle)
{
    xhn::kernel::memory_handle ret(NULL);
    map<string, kernel_memory_ptr>::iterator iter =
    m_host_memory_map.insert(make_pair(name, handle.m_kernel_memory));
    if (iter != m_host_memory_map.end()) {
        ret.m_kernel_memory = iter->second;
    }
    argument_entry entry = {memory, name};
    m_arguments.push_back(entry);
    return ret;
}

xhn::kernel::memory_handle
xhn::kernel::add_image_memory_parameter(const string& name,
                                        euint memory_size,
                                        kernel_memory::access access)
{
    xhn::kernel::memory_handle ret(NULL);
    kernel_memory_ptr kernel_memory = VNEW class kernel_memory(this, true);
    kernel_memory->initialize(memory_size, access);
    map<string, kernel_memory_ptr>::iterator iter =
    m_host_memory_map.insert(make_pair(name, kernel_memory));
    if (iter != m_host_memory_map.end()) {
        ret.m_kernel_memory = iter->second;
    }
    argument_entry entry = {memory, name};
    m_arguments.push_back(entry);
    return ret;
}

void xhn::kernel::add_dependent_library(library_ptr lib)
{
    m_dependent_librarys.push_back(lib);
}

double xhn::kernel::launch(execute_event* event)
{
    size_t global_work_offset = m_global_work_offset;
    size_t global_work_size = m_global_work_size;
    size_t local_work_size = m_local_work_size;
    TimeCheckpoint t0 = TimeCheckpoint::Tick();
    cl_event* evt = NULL;
    if (event) {
        evt = &event->event;
    }
    cl_int error =
    clEnqueueNDRangeKernel(m_context->get_command_queue(),
                           m_kernel,
                           1,
                           &global_work_offset,
                           &global_work_size,
                           &local_work_size,
                           0,
                           NULL,
                           evt);
    if (error != CL_SUCCESS) {
        printf("%s\n", get_error_information(error));
    }
    TimeCheckpoint t1 = TimeCheckpoint::Tick();
    return TimeCheckpoint::CaleElapsedTime(t0, t1);
}
