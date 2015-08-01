//
//  xhn_device.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_device__
#define ___zOpenCL__xhn_device__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"

#include "xhn_map.hpp"
#include "xhn_string.hpp"

#include "xhn_context.h"

namespace xhn
{
    class platform;
    class context;
    class kernel;
    class device : public RefObject
    {
    public:
        enum device_type
        {
            cpu,
            gpu,
            accelerator,
            custom,
            all,
        };
    private:
        cl_device_id m_id;
        device_type m_type;
        platform* m_platform;
        map<string, context_ptr> m_contexts;
        cl_uint m_vendor_id;
        cl_uint m_max_compute_units;
        cl_uint m_max_work_item_dimensions;
        cl_ulong m_max_work_group_size;
        cl_uint8 m_max_work_item_sizes;
        cl_uint m_max_clock_frequency;
        cl_ulong m_max_mem_alloc_size;
        cl_uint m_global_mem_cache_type;
        cl_uint m_global_mem_cacheline_size;
        cl_ulong m_global_mem_cache_size;
        cl_ulong m_global_mem_size;
        cl_uint m_local_mem_type;
        cl_ulong m_local_mem_size;
        cl_ulong m_image2d_max_width;
        cl_ulong m_image2d_max_height;
        cl_uint m_image_support;
        char m_opencl_c_version[14];
    private:
        void throw_output_overflow_exception();
        template <typename T>
        void get_device_information(cl_device_info information, T& output)
        {
            size_t size;
            clGetDeviceInfo(m_id, information, NULL, NULL, &size);
            if (size <= sizeof(T)) {
                memset(&output, 0, sizeof(T));
                clGetDeviceInfo(m_id, information, size, &output, NULL);
            }
            else {
                throw_output_overflow_exception();
            }
        }
        void collect_device_informations();
    public:
        void initialize();
        device(cl_device_id id, device_type type, platform* platform)
        : m_id(id)
        , m_type(type)
        , m_platform(platform)
        {}
        inline platform* get_platform() {
            return m_platform;
        }
        inline cl_device_id get_id() {
            return m_id;
        }
        context_ptr create_context(const string& context_name);
        inline cl_ulong get_max_work_item_size() {
            return *((cl_ulong*)&m_max_work_item_sizes.s[0]) / 2;
        }
        inline cl_ulong get_max_work_group_size() {
            return m_max_work_group_size;
        }
        inline cl_uint get_max_compute_units() {
            return m_max_compute_units;
        }
    };
}

#endif /* defined(___zOpenCL__xhn_device__) */
