//
//  xhn_filesystem_node.h
//  Xgc
//
//  Created by 徐海宁 on 15/1/4.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#ifndef __Xgc__xhn_filesystem_node__
#define __Xgc__xhn_filesystem_node__

#include "xhn_garbage_collector.hpp"
#include "xhn_gc_container_base.h"
#include "xhn_gc_array.h"
#include "xhn_gc_string.h"

namespace xhn
{
    class filesystem_node : public GCObject
    {
    public:
        enum node_type
        {
            file_node,
            folder_node,
        };
    public:
        GCArray* m_children;
        GCUtf8String* m_path;
        node_type m_type;
    public:
        filesystem_node(const Utf8& path, node_type type);
        void add_child_node(filesystem_node* node);
        void print();
        static filesystem_node* collect_filesystem_tree(GCRootHandle& root, Utf8& path);
    };
}

#endif /* defined(__Xgc__xhn_filesystem_node__) */
