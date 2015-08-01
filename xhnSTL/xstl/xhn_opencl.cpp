//
//  xhn_opencl.cpp
//  xhn
//
//  Created by 徐海宁 on 13-12-17.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#include "xhn_opencl.hpp"




void xhn::opencl::throw_get_platform_id_failure_exception(cl_int error_code)
{
    ///
}

void xhn::opencl::collect_platforms()
{
    cl_uint count = 0;
    cl_int return_value = clGetPlatformIDs(0, NULL, &count);
    if (return_value) {
        throw_get_platform_id_failure_exception(return_value);
    }
    if (!count)
        return;
    cl_platform_id* ids = (cl_platform_id*)malloc(count * sizeof(cl_platform_id));
    memset(ids, 0, count * sizeof(cl_platform_id));
    return_value = clGetPlatformIDs(count, ids, &count);
    if (return_value) {
        throw_get_platform_id_failure_exception(return_value);
    }
    for (cl_uint i = 0; i < count; i++) {
        m_platforms.push_back(platform(ids[i]));
    }
    free(ids);
}

void xhn::opencl::initialize_platforms()
{
    vector<platform>::iterator iter = m_platforms.begin();
    vector<platform>::iterator end = m_platforms.end();
    for (; iter != end; iter++)
    {
        platform& platform = *iter;
        platform.initialize();
    }
}

void xhn::opencl::initialize()
{
    collect_platforms();
    initialize_platforms();
}

xhn::platform* xhn::opencl::get_platform(euint index)
{
    if (index < m_platforms.size()) {
        return &m_platforms[index];
    }
    else{
        return NULL;
    }
}
