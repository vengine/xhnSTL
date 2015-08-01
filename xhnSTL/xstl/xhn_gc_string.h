//
//  xhn_gc_string.h
//  xhnSTL
//
//  Created by 徐海宁 on 15/1/5.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#ifndef __xhnSTL__xhn_gc_string__
#define __xhnSTL__xhn_gc_string__

#include "xhn_gc_container_base.h"
#include "xhn_string.hpp"

namespace xhn
{
    class GCUtf8String : public GCAllocatable
    {
    private:
        GCMemory* m_memory;
        const char* m_string;
        euint m_size;
    public:
        GCUtf8String();
        GCUtf8String(const Utf8& str);
        inline const char* c_str() const {
            return m_string;
        }
    };
}

#endif /* defined(__xhnSTL__xhn_gc_string__) */
