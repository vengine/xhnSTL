//
//  xhn_platform.cpp
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_platform.h"

void xhn::platform::get_platform_information(cl_platform_info information, string& output_string)
{
    size_t size;
    clGetPlatformInfo(m_id, information, NULL, NULL, &size);
    output_string.resize(size);
    clGetPlatformInfo(m_id, information, size, &output_string[0], NULL);
}

void xhn::platform::collect_platform_informations()
{
    get_platform_information(CL_PLATFORM_PROFILE, m_profile);
    get_platform_information(CL_PLATFORM_VERSION, m_version);
    get_platform_information(CL_PLATFORM_NAME, m_name);
    get_platform_information(CL_PLATFORM_VENDOR, m_vendor);
    get_platform_information(CL_PLATFORM_EXTENSIONS, m_extensions);
}

void xhn::platform::throw_get_device_id_failure_exception(cl_int error_code)
{
    ///
}

void xhn::platform::collect_devices_in_type(cl_device_type type, vector<device>& output_devices)
{
    cl_uint count = 0;
    cl_int return_value = 0;
    vector< cl_device_id, FGetCharRealSizeProc<char> > ids;
    return_value = clGetDeviceIDs(m_id, type, 0, NULL, &count);
    if (return_value) {
        throw_get_device_id_failure_exception(return_value);
    }
    ids.resize(count);
    return_value = clGetDeviceIDs(m_id, type, count, &ids[0], &count);
    if (return_value) {
        throw_get_device_id_failure_exception(return_value);
    }
    for (cl_uint i = 0; i < count; i++) {
        switch(type)
        {
            case CL_DEVICE_TYPE_CPU:
                output_devices.push_back(device(ids[i], device::device_type::cpu, this));
                break;
            case CL_DEVICE_TYPE_GPU:
                output_devices.push_back(device(ids[i], device::device_type::gpu, this));
                break;
            case CL_DEVICE_TYPE_ACCELERATOR:
                output_devices.push_back(device(ids[i], device::device_type::accelerator, this));
                break;
            case CL_DEVICE_TYPE_CUSTOM:
                output_devices.push_back(device(ids[i], device::device_type::custom, this));
                break;
            default:
                break;
        }
    }
}

void xhn::platform::collect_all_devices()
{
    collect_devices_in_type(CL_DEVICE_TYPE_CPU, m_cpu_devices);
    collect_devices_in_type(CL_DEVICE_TYPE_GPU, m_gpu_devices);
    collect_devices_in_type(CL_DEVICE_TYPE_ACCELERATOR, m_accelerator_devices);
    collect_devices_in_type(CL_DEVICE_TYPE_CUSTOM, m_custom_devices);
}

void xhn::platform::initialize_all_devices()
{
    vector<device>::iterator iter = m_cpu_devices.begin();
    vector<device>::iterator end = m_cpu_devices.end();
    for (; iter != end; iter++) { iter->initialize(); }
    iter = m_gpu_devices.begin();
    end = m_gpu_devices.end();
    for (; iter != end; iter++) { iter->initialize(); }
    iter = m_accelerator_devices.begin();
    end = m_accelerator_devices.end();
    for (; iter != end; iter++) { iter->initialize(); }
    iter = m_custom_devices.begin();
    end = m_custom_devices.end();
    for (; iter != end; iter++) { iter->initialize(); }
}

void xhn::platform::initialize()
{
    collect_platform_informations();
    collect_all_devices();
    initialize_all_devices();
}

xhn::device*
xhn::platform::get_device(device::device_type type, euint index)
{
    switch (type)
    {
        case device::device_type::cpu:
            if (index < m_cpu_devices.size())
                return &m_cpu_devices[index];
            else
                return NULL;
        case device::device_type::gpu:
            if (index < m_gpu_devices.size())
                return &m_gpu_devices[index];
            else
                return NULL;
        case device::device_type::accelerator:
            if (index < m_accelerator_devices.size())
                return &m_accelerator_devices[index];
            else
                return NULL;
        case device::device_type::custom:
            if (index < m_custom_devices.size())
                return &m_custom_devices[index];
            else
                return NULL;
        default:
            return NULL;
    }
}
