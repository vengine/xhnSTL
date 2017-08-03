/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef xhn_cache_hpp
#define xhn_cache_hpp

#include "xhn_dictionary.hpp"
#include "xhn_vector.hpp"

namespace xhn
{
    template <typename K, typename V, unsigned CACHE_SIZE>
    class FCacheHashListNodeAllocator
    {
    public:
        vector< hash_node<K, V>, FGetElementRealSizeProc< hash_node<K, V> > > m_node_buffer;
        euint m_top_pointer;
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef hash_node<K, V>* pointer;
        typedef const hash_node<K, V>* const_pointer;
        typedef hash_node<K, V> value_type;
        typedef hash_node<K, V>& reference;
        typedef const hash_node<K, V>& const_reference;
        
        pointer address(reference v) const                                         { return &v; }
        const_pointer address(const_reference v) const                             { return &v; }
        FCacheHashListNodeAllocator()                                              { m_node_buffer.resize(CACHE_SIZE); m_top_pointer = 0; }
        FCacheHashListNodeAllocator& operator=(const FCacheHashListNodeAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                                    { }
        pointer allocate(size_type count)                                          { return &m_node_buffer[m_top_pointer++]; }
        pointer allocate(size_type count, const void*)                             { return &m_node_buffer[m_top_pointer++]; }
        void construct(pointer ptr, const hash_node<K, V>& v)                      { new ( ptr ) value_type( v ); }
        void construct(const pointer ptr)                                          { new ( ptr ) value_type (); }
        void construct(pointer ptr, const K& k, const V& v)                        { new ( ptr ) value_type (k, v); }
        void construct(pointer ptr, K&& k, V&& v)                                  { new ( ptr ) value_type (k, v); }
        void destroy(pointer ptr)                                                  { ptr->~value_type(); }
        
        size_type max_size() const                                                 { return static_cast<size_type>(-1) / sizeof(value_type); }
    };

    template <typename K, typename V, unsigned CACHE_SIZE>
    class cache : public dictionary<K, V,
    2,
    FHashProc<K>,
    FEqualProc<K>,
    FHashBucketAllocator<K, V>,
    FCacheHashListNodeAllocator<K, V, CACHE_SIZE> >
    {
    public:
        typedef typename vector< hash_node<K, V>, FGetElementRealSizeProc< hash_node<K, V> > >::iterator iterator;
        typedef typename vector< hash_node<K, V>, FGetElementRealSizeProc< hash_node<K, V> > >::const_iterator const_iterator;
        
        typedef
        dictionary<K, V,
        2,
        FHashProc<K>,
        FEqualProc<K>,
        FHashBucketAllocator<K, V>,
        FCacheHashListNodeAllocator<K, V, CACHE_SIZE> > base_type;
    public:
        cache()
        {
            base_type::m_need_dealloc = false;
        }
        ~cache()
        {}
        void clear() {
            for (euint32 i = 0; i < base_type::m_num_buckets; i++) {
                base_type::m_buckets[i].force_clear();
            }
            base_type::m_node_allocator.m_top_pointer = 0;
        }
        iterator begin() {
            return base_type::m_node_allocator.m_node_buffer.begin();
        }
        iterator end() {
            return base_type::m_node_allocator.m_node_buffer.begin() + base_type::m_node_allocator.m_top_pointer;
        }
        const_iterator begin() const {
            return base_type::m_node_allocator.m_node_buffer.begin();
        }
        const_iterator end() const {
            return base_type::m_node_allocator.m_node_buffer.begin() + base_type::m_node_allocator.m_top_pointer;
        }
    };
}

#endif /* xhn_cache_hpp */

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
