//
//  xhn_gc_container_base.cpp
//  xhnSTL
//
//  Created by 徐海宁 on 15/1/5.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#include "xhn_gc_container_base.h"

GCMemory* xhn::GCAllocatable::Alloc(euint64 size)
{
    GCMemoryInfo info = {size, gc};
    return new (&info) GCMemory;
}