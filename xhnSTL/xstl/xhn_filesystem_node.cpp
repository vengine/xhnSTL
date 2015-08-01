//
//  xhn_filesystem_node.cpp
//  Xgc
//
//  Created by 徐海宁 on 15/1/4.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

#include "xhn_filesystem_node.h"



xhn::filesystem_node::filesystem_node(const Utf8& path, node_type type)
: m_type(type)
{
    m_path = new (gc) GCUtf8String(path);
    m_children = new(gc) GCArray;
    gc->Attach(gcNode, m_path->gcNode);
    gc->Attach(gcNode, m_children->gcNode);
}

void xhn::filesystem_node::add_child_node(filesystem_node* node)
{
    m_children->AddObject(node);
}

void xhn::filesystem_node::print()
{
    if (m_type == file_node) {
        printf("%s\n", m_path->c_str());
    }
    else {
        printf("%s\n", m_path->c_str());
        for (euint i = 0; i < m_children->Size(); i++) {
            filesystem_node* child = (filesystem_node*)m_children->GetObject(i);
            child->print();
        }
    }
}