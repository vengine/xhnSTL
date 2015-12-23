#include "xhn_garbage_collector.hpp"

GarbageCollector* GarbageCollector::s_GarbageCollector = NULL;
void GarbageCollector::Init()
{
    s_GarbageCollector = VNEW GarbageCollector;
}
void GarbageCollector::Dest()
{
    delete s_GarbageCollector;
}
GarbageCollector* GarbageCollector::Get()
{
    return s_GarbageCollector;
}