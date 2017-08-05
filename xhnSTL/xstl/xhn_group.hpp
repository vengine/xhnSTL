/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef xhn_group_hpp
#define xhn_group_hpp

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include "xhn_iterator.hpp"

namespace xhn
{

template <typename T>
struct group_node : public MemObject
{
    T m_elements[16];
    group_node* m_prev;
    group_node* m_next;
    euint m_num_elements;
    group_node()
    : m_prev(nullptr)
    , m_next(nullptr)
    , m_num_elements(0)
    {}
    ~group_node()
    {
        for (euint i = 0; i < m_num_elements; i++) {
            (&m_elements[i])->~T();
        }
    }
};
    
template <typename T>
class FGroupNodeAllocator
{
public:
    typedef euint size_type;
    typedef euint difference_type;
    typedef group_node<T>* pointer;
    typedef const group_node<T>* const_pointer;
    typedef group_node<T> value_type;
    typedef group_node<T>& reference;
    typedef const group_node<T>& const_reference;
    
    pointer address(reference v) const                                         { return &v; }
    const_pointer address(const_reference v) const                             { return &v; }
    FGroupNodeAllocator()                                                      {}
    FGroupNodeAllocator& operator=(const FGroupNodeAllocator&)                 { return (*this); }
    void deallocate(pointer ptr, size_type)                                    { Mfree(ptr); }
    pointer allocate(size_type count)                                          { return (pointer)NMalloc(sizeof(value_type)); }
    pointer allocate(size_type count, const void*)                             { return (pointer)NMalloc(sizeof(value_type)); }
    void construct(pointer ptr, const value_type& v)                           { new ( ptr ) value_type( v ); }
    void construct(const pointer ptr)                                          { new ( ptr ) value_type (); }
    void destroy(pointer ptr)                                                  { ptr->~value_type(); }
    
    size_type max_size() const                                                 { return static_cast<size_type>(-1) / sizeof(value_type); }
};
    
template <typename T,
          typename NODE_ALLOCATOR = FGroupNodeAllocator<T>>
class group
{
public:
    group_node<T>* m_start_node;
    group_node<T>* m_end_node;
    euint m_count;
    NODE_ALLOCATOR m_node_allocator;
public:
    struct iterator_data
    {
        group_node<T>* m_group_node;
        euint m_element_index;
    };
    
    typedef iterator_data node_type;
    
    template<typename N>
    struct FReadProc {
        void operator() ( N *from, N &to ) {
            to = *from;
        }
    };
    template<typename N>
    struct FWriteProc {
        void operator() ( N *to, N &from ) {
            *to = from;
        }
    };
    template<typename N>
    struct FNextProc {
        void operator() ( N *from, N *&to, euint ele_real_size ) {
            if (from->m_element_index < from->m_group_node->m_num_elements - 1) {
                from->m_element_index++;
                to = from;
            }
            else {
                from->m_group_node = from->m_group_node->m_next;
                from->m_element_index = 0;
                if (!from->m_group_node) {
                    to = nullptr;
                }
                else {
                    to = from;
                }
            }
        }
    };

    class iterator : public forward_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FNextProc<node_type> >
    {
    public:
        iterator_data m_data;
    public:
        typedef forward_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type> > base_type;
        iterator ( node_type *a, euint ele_real_size )
        : base_type (
                     a,
                     ele_real_size,
                     FReadProc<node_type>(),
                     FWriteProc<node_type>(),
                     FNextProc<node_type>() )
        {}
        iterator (  )
        : base_type (
                     NULL,
                     0,
                     FReadProc<node_type>(),
                     FWriteProc<node_type>(),
                     FNextProc<node_type>() )
        {}
        inline iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline iterator operator++ ( int ) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline iterator operator-- ( int ) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline T &operator*() {
            return base_type::m_ptr->m_group_node->m_elements[base_type::m_ptr->m_element_index];
        }
        inline T *operator->() {
            return &base_type::m_ptr->m_group_node->m_elements[base_type::m_ptr->m_element_index];
        }
    };
    
    class const_iterator : public const_forward_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FNextProc<node_type> >
    {
    public:
        iterator_data m_data;
    public:
        typedef forward_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type> > base_type;
        const_iterator ( node_type *a, euint ele_real_size )
        : base_type (
                     a,
                     ele_real_size,
                     FReadProc<node_type>(),
                     FWriteProc<node_type>(),
                     FNextProc<node_type>() )
        {}
        const_iterator (  )
        : base_type (
                     NULL,
                     0,
                     FReadProc<node_type>(),
                     FWriteProc<node_type>(),
                     FNextProc<node_type>() )
        {}
        inline const_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline const_iterator operator++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline const_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline const_iterator operator-- ( int ) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline const T &operator*() {
            return base_type::m_ptr->m_group_node->m_elements[base_type::m_ptr->m_element_index];
        }
        inline const T *operator->() {
            return &base_type::m_ptr->m_group_node->m_elements[base_type::m_ptr->m_element_index];
        }
    };
public:
    group()
    : m_count(0)
    {
        m_start_node = m_end_node = m_node_allocator.allocate( 1 );
        m_node_allocator.construct(m_start_node);
    }
    ~group()
    {
        group_node<T>* iter = m_start_node;
        while (iter != m_end_node) {
            group_node<T>* node = iter;
            iter = iter->m_next;
            m_node_allocator.destroy( node );
            m_node_allocator.deallocate( node, 1 );
        }
        m_node_allocator.destroy( m_end_node );
        m_node_allocator.deallocate( m_end_node , 1 );
    }
    iterator add(const T& value) {
        iterator ret;
        if (m_end_node->m_num_elements < 16) {
            new ( &m_end_node->m_elements[m_end_node->m_num_elements] ) T ( value );
            ret.m_data.m_group_node = m_end_node;
            ret.m_data.m_element_index = m_end_node->m_num_elements;
            m_end_node->m_num_elements++;
        }
        else {
            group_node<T>* node = m_node_allocator.allocate( 1 );
            m_node_allocator.construct( node );
            m_end_node->m_next = node;
            node->m_prev = m_end_node;
            new ( &node->m_elements[node->m_num_elements] ) T ( value );
            node->m_num_elements++;
            ret.m_data.m_group_node = node;
            ret.m_data.m_element_index = 0;
            m_end_node = node;
        }
        m_count++;
        return ret;
    }
    void remove(iterator iter) {
        T* value = &iter.m_data.m_group_node->m_elements[iter.m_data.m_element_index];
        value->~T();
        memcpy(value, &m_end_node->m_elements[m_end_node->m_num_elements - 1], sizeof(T));
        m_end_node->m_num_elements--;
        if (!m_end_node->m_num_elements && m_start_node != m_end_node) {
            group_node<T>* node = m_end_node;
            m_end_node = m_end_node->m_prev;
            m_node_allocator.destroy(node);
            m_node_allocator.deallocate(node, 1);
        }
        m_count--;
    }
    iterator begin() {
        iterator ret;
        if (m_count) {
            ret.m_data.m_group_node = m_start_node;
            ret.m_data.m_element_index = 0;
        }
        ret.m_ptr = &ret.m_data;
        return ret;
    }
    iterator end() {
        iterator ret;
        return ret;
    }
    const_iterator begin() const {
        const_iterator ret;
        if (m_count) {
            ret.m_data.m_group_node = m_start_node;
            ret.m_data.m_element_index = 0;
        }
        ret.m_ptr = &ret.m_data;
        return ret;
    }
    const_iterator end() const {
        iterator ret;
        return ret;
    }
    euint size() const {
        return m_count;
    }
};
    
}

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
