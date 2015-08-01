//
//  xhn_platform.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_platform__
#define ___zOpenCL__xhn_platform__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include <OpenCL/cl.h>
#include "xhn_string.hpp"
#include "xhn_device.h"
namespace xhn
{
    class device;
    class context;
    class kernel;
    class platform : public RefObject
    {
    public:
        cl_platform_id m_id;
        string m_profile;
        string m_version;
        string m_name;
        string m_vendor;
        string m_extensions;
        vector<device> m_cpu_devices;
        vector<device> m_gpu_devices;
        vector<device> m_accelerator_devices;
        vector<device> m_custom_devices;
    private:
        void get_platform_information(cl_platform_info information, string& output_string);
        void collect_platform_informations();
        void throw_get_device_id_failure_exception(cl_int error_code);
        void collect_devices_in_type(cl_device_type type, vector<device>& output_devices);
        void collect_all_devices();
        void initialize_all_devices();
    public:
        void initialize();
        platform(cl_platform_id id)
        : m_id(id)
        {}
        inline cl_platform_id get_id() {
            return m_id;
        }
        device* get_device(device::device_type type, euint index);
    };
}

#endif /* defined(___zOpenCL__xhn_platform__) */
