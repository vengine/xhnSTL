//
//  xhn_library.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_library__
#define ___zOpenCL__xhn_library__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"

#include "xhn_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_smart_ptr.hpp"
namespace xhn
{
    class device;
    class platform;
    class context;
    class kernel;
    class struct_declaration : public RefObject
    {
    public:
        virtual ~struct_declaration() {}
        virtual string make_struct_declaration() = 0;
    };
    typedef SmartPtr<struct_declaration> struct_declaration_ptr;
    class library : public RefObject
    {
    public:
        virtual ~library() {}
        virtual string make_source_code() = 0;
        virtual vector<string> get_output_functions() = 0;
        virtual struct_declaration_ptr get_struct_declaration() = 0;
    };
    typedef SmartPtr<library> library_ptr;
}

#endif /* defined(___zOpenCL__xhn_library__) */
