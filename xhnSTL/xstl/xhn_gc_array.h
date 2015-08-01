//
//  xhn_gc_array.h
//  xhnSTL
//
//  Created by 徐海宁 on 15/1/5.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#ifndef __xhnSTL__xhn_gc_array__
#define __xhnSTL__xhn_gc_array__

#include "xhn_gc_container_base.h"

namespace xhn
{
    class GCArray : public GCAllocatable
    {
    private:
        GCMemory* m_memory;
        euint m_capacity;
        euint m_size;
    public:
        GCArray();
    private:
        void GrowUp(euint newCapacity);
    public:
        void AddObject(GCObject* object);
        euint Size() const;
        GCObject* GetObject(euint index);
        void RemoveObject(euint index);
        void InsertObject(euint index, GCObject* object);
    };
}

#endif /* defined(__xhnSTL__xhn_gc_array__) */
