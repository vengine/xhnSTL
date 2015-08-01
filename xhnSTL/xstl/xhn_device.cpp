//
//  xhn_device.cpp
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_device.h"

void xhn::device::throw_output_overflow_exception()
{
    ///
}

void xhn::device::collect_device_informations()
{
    get_device_information(CL_DEVICE_VENDOR_ID, m_vendor_id);
    get_device_information(CL_DEVICE_MAX_COMPUTE_UNITS, m_max_compute_units);
    get_device_information(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, m_max_work_item_dimensions);
    get_device_information(CL_DEVICE_MAX_WORK_GROUP_SIZE, m_max_work_group_size);
    get_device_information(CL_DEVICE_MAX_WORK_ITEM_SIZES, m_max_work_item_sizes);
    get_device_information(CL_DEVICE_MAX_CLOCK_FREQUENCY, m_max_clock_frequency);
    get_device_information(CL_DEVICE_MAX_MEM_ALLOC_SIZE , m_max_mem_alloc_size);
    get_device_information(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, m_global_mem_cache_type);
    get_device_information(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, m_global_mem_cacheline_size);
    get_device_information(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, m_global_mem_cache_size);
    get_device_information(CL_DEVICE_GLOBAL_MEM_SIZE, m_global_mem_size);
    get_device_information(CL_DEVICE_LOCAL_MEM_TYPE, m_local_mem_type);
    get_device_information(CL_DEVICE_LOCAL_MEM_SIZE, m_local_mem_size);
    get_device_information(CL_DEVICE_IMAGE2D_MAX_WIDTH, m_image2d_max_width);
    get_device_information(CL_DEVICE_IMAGE2D_MAX_HEIGHT, m_image2d_max_height);
    get_device_information(CL_DEVICE_IMAGE_SUPPORT, m_image_support);
    get_device_information(CL_DEVICE_OPENCL_C_VERSION, m_opencl_c_version);
}

void xhn::device::initialize()
{
    collect_device_informations();
}

xhn::context_ptr
xhn::device::create_context(const string& context_name)
{
    context_ptr ret;
    context_ptr context = VNEW class context(this);
    context->initialize();
    map<string, context_ptr>::iterator iter = m_contexts.insert(make_pair(context_name, context));
    if (iter != m_contexts.end()) {
        ret = iter->second;
    }
    return ret;
}
