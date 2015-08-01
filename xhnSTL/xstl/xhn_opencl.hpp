//
//  xhn_opencl.hpp
//  xhn
//
//  Created by 徐海宁 on 13-12-17.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef ___z__xhn_opencl__
#define ___z__xhn_opencl__
#include "xhn_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_void_vector.hpp"
#include "xhn_map.hpp"
#include "xhn_btree.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_lock.hpp"
#include "timer.h"
#include <OpenCL/cl.h>
#include "xhn_kernel_memory.h"
#include "xhn_kernel.h"
#include "xhn_library.h"
#include "xhn_context.h"
#include "xhn_device.h"
#include "xhn_platform.h"
/// 各种不同的id关系如下
/// get_global_id(dim) == get_group_id(dim) * get_local_size(dim) + get_local_id(dim);
namespace xhn
{
#define KERNEL(...)#__VA_ARGS__
class opencl
{
private:
    vector<platform> m_platforms;
private:
    void throw_get_platform_id_failure_exception(cl_int error_code);
    void collect_platforms();
    void initialize_platforms();
protected:
    void initialize();
    void destroy();
public:
    opencl() {
        initialize();
    }
    ~opencl() {}
    platform* get_platform(euint index);
};
}
#endif /* defined(___z__xhn_opencl__) */
