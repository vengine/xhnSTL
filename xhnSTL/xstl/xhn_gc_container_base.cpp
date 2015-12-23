#include "xhn_gc_container_base.h"

GCMemory* xhn::GCAllocatable::Alloc(euint64 size)
{
    GCMemoryInfo info = {size, gc};
    return new (&info) GCMemory;
}