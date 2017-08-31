/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LIST2_H
#define XHN_LIST2_H

#ifdef __cplusplus

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
                m_head = m_tail = nullptr;
                return nullptr;
            }
        }
    }
    
    euint size() const {
        return m_count;
    }
    
    bool empty() const {
        return !m_count;
    }
};
}

#endif

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
