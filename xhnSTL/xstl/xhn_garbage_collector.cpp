//
//  xhn_garbage_collector.cpp
//  Xgc
//
//  Created by 徐海宁 on 14-7-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

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