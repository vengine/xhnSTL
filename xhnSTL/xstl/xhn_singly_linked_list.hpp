/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_SINGLY_LINKED_LIST_HPP
#define XHN_SINGLY_LINKED_LIST_HPP

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
        
        void force_clear() {
            m_head = nullptr;
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
