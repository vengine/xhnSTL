//
//  xhn_list2.h
//  xhnSTL
//
//  Created by 徐海宁 on 1/24/15.
//  Copyright (c) 2015 徐 海宁. All rights reserved.
//

#ifndef __xhnSTL__xhn_list2__
#define __xhnSTL__xhn_list2__

#include "common.h"
#include "etypes.h"

namespace xhn
{
template <typename T>
class list2
{
private:
    T *m_head;
    T *m_tail;
    euint m_count;
public:
    list2()
    : m_head(nullptr)
    , m_tail(nullptr)
    , m_count(0)
    {}
    
    T* begin() {
        return m_head;
    }
    
    T* push_back ( T *node ) {
        
        if ( m_tail ) {
            m_tail->m_iter_next = node;
            node->m_iter_prev = m_tail;
            node->m_iter_next = nullptr;
            m_tail = node;
        } else {
            m_head = m_tail = node;
            m_tail->m_iter_prev = nullptr;
            m_tail->m_iter_next = nullptr;
        }
        
        m_count++;
        return m_tail;
    }
    
    T* push_front ( T *node ) {
        
        if ( m_head ) {
            m_head->m_iter_prev = node;
            node->m_iter_next = m_head;
            node->m_iter_prev = NULL;
            m_head = node;
        } else {
            m_tail = m_head = node;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = NULL;
        }
        
        m_count++;
        return m_head;
    }
    T* insert_after ( T *where, T *n ) {
        
        if ( where->m_iter_next ) {
            n->m_iter_next = where->m_iter_next;
            where->m_iter_next->m_iter_prev = n;
            n->m_iter_prev = where;
            where->m_iter_next = n;
            m_count++;
            return n;
        } else {
            where->m_iter_next = n;
            m_tail = where->m_iter_next;
            m_tail->m_iter_prev = where;
            m_tail->m_iter_next = nullptr;
            m_count++;
            return m_tail;
        }
    }
    
    T* insert_before ( T *where, T *n ) {
        
        if ( where->m_iter_prev ) {
            n->m_iter_prev = where->m_iter_prev;
            where->m_iter_prev->m_iter_next = n;
            n->m_iter_next = where;
            where->m_iter_prev = n;
            m_count++;
            return n;
        } else {
            where->m_iter_prev = n;
            m_head = where->m_iter_prev;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = where;
            m_count++;
            return m_head;
        }
    }
    
    T* remove ( T* node ) {
        if ( !m_count ) {
            return nullptr;
        } else {
            m_count--;
            
            if ( node != m_head && node != m_tail ) {
                T *ret = node->m_iter_next;
                node->m_iter_prev->m_iter_next = node->m_iter_next;
                node->m_iter_next->m_iter_prev = node->m_iter_prev;
                return ret;
            } else if ( node != m_head && node == m_tail ) {
                node->m_iter_prev->m_iter_next = nullptr;
                m_tail = node->m_iter_prev;
                return nullptr;
            } else if ( node == m_head && node != m_tail ) {
                T *ret = node->m_iter_next;
                node->m_iter_next->m_iter_prev = nullptr;
                m_head = node->m_iter_next;
                return ret;
            } else {
                T *tmp = m_head;
                m_head = m_tail = nullptr;
                return nullptr;
            }
        }
    }
};
}
#endif /* defined(__xhnSTL__xhn_list2__) */
