//
//  xhn_gc_container_base.h
//  xhnSTL
//
//  Created by 徐海宁 on 15/1/5.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#ifndef __xhnSTL__xhn_gc_container_base__
#define __xhnSTL__xhn_gc_container_base__

#include "xhn_garbage_collector.hpp"

namespace xhn
{
    class GCAllocatable : public GCObject
    {
    protected:
        GCMemory* Alloc(euint64 size);
    };
}

#endif /* defined(__xhnSTL__xhn_gc_container_base__) */
