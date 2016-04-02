//
//  xhn_singly_linked_list.hpp
//  xhnSTL
//
//  Created by 徐海宁 on 16/4/2.
//  Copyright © 2016年 徐 海宁. All rights reserved.
//

#ifndef xhn_singly_linked_list_hpp
#define xhn_singly_linked_list_hpp

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"

namespace xhn
{
    template <typename T>
    class singly_linked_list
    {
    private:
        T *m_head;
    public:
        singly_linked_list()
        : m_head(nullptr)
        {}
        
        T* begin() {
            return m_head;
        }
        
        T* add ( T *node ) {
            if ( m_head ) {
                node->m_iter_next = m_head;
                node->m_iter_prev = nullptr;
                m_head->m_iter_prev = node;
                m_head = node;
            } else {
                m_head = node;
                m_head->m_iter_next = nullptr;
                m_head->m_iter_prev = nullptr;
            }
            return m_head;
        }
        
        T* remove ( T *node ) {
            if ( !m_head ) {
                return nullptr;
            } else {
                if ( node->m_iter_next && node->m_iter_prev ) {
                    T *ret = node->m_iter_next;
                    node->m_iter_prev->m_iter_next = node->m_iter_next;
                    node->m_iter_next->m_iter_prev = node->m_iter_prev;
                    return ret;
                } else if ( node->m_iter_next == nullptr && node->m_iter_prev ) {
                    node->m_iter_prev->m_iter_next = nullptr;
                    return nullptr;
                } else if ( node->m_iter_next && node->m_iter_prev == nullptr ) {
                    T *ret = node->m_iter_next;
                    node->m_iter_next->m_iter_prev = nullptr;
                    m_head = node->m_iter_next;
                    return ret;
                } else {
                    m_head = nullptr;
                    return nullptr;
                }
            }
        }
    };
}

#endif

#endif /* xhn_singly_linked_list_hpp */
