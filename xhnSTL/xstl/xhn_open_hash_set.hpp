/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */


#ifndef XHN_OPEN_HASH_SET_HPP
#define XHN_OPEN_HASH_SET_HPP

#ifdef __cplusplus

#include "xhn_dictionary.hpp"

namespace xhn
{
    template <typename K>
    class hash_set_node
    {
    public:
        typedef typename remove_const< K >::type key_type;
        typedef typename TKeyValue< key_type >::type hash_key_type;
    public:
        ALIGN_FLAG ( 16 ) hash_key_type first;
        hash_set_node* m_iter_next;
        hash_set_node* m_iter_prev;
        euint32 m_hash_value;
        euint32 m_count;
        vptr m_hash_table;
        euint32 m_bucket_index;
        hash_set_node( const K &key )
        : first((hash_key_type)key)
        , m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        hash_set_node( K &&key )
        : first(key)
        , m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        hash_set_node()
        : m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        inline void set_value(const K &value) {
        }
        inline void set_value(K &&value) {
        }
        inline K& get_value() {
            return first;
        }
    };
    
    template <typename K>
    class FHashSetBucketAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef singly_linked_list<hash_set_node<K>>* pointer;
        typedef const singly_linked_list<hash_set_node<K>>* const_pointer;
        typedef singly_linked_list<hash_set_node<K>> value_type;
        typedef singly_linked_list<hash_set_node<K>>& reference;
        typedef const singly_linked_list<hash_set_node<K>>& const_reference;
        
        pointer address(reference v) const                                 { return &v; }
        const_pointer address(const_reference v) const                     { return &v; }
        FHashSetBucketAllocator()                                          {}
        FHashSetBucketAllocator& operator=(const FHashSetBucketAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                            { Mfree(ptr); }
        pointer allocate(size_type count)                                  { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)                     { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_set_node<K>& v)             { new ( ptr ) value_type( v ); }
        void construct(const pointer ptr)                                  { new ( ptr ) value_type (); }
        void destroy(pointer ptr)                                          { ptr->~value_type(); }
        
        size_type max_size() const                                         { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template <typename K>
    class FHashSetListNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef hash_set_node<K>* pointer;
        typedef const hash_set_node<K>* const_pointer;
        typedef hash_set_node<K> value_type;
        typedef hash_set_node<K>& reference;
        typedef const hash_set_node<K>& const_reference;
        
        pointer address(reference v) const                                     { return &v; }
        const_pointer address(const_reference v) const                         { return &v; }
        FHashSetListNodeAllocator()                                            {}
        FHashSetListNodeAllocator& operator=(const FHashSetListNodeAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                                { Mfree(ptr); }
        pointer allocate(size_type count)                                      { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)                         { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_set_node<K>& v)                 { new ( ptr ) value_type( v ); }
        void construct(const pointer ptr)                                      { new ( ptr ) value_type (); }
        void construct(pointer ptr, const K& k, const K& v)                    { new ( ptr ) value_type (k); }
        void construct(pointer ptr, K&& k, K&& v)                              { new ( ptr ) value_type (k); }
        void destroy(pointer ptr)                                              { ptr->~value_type(); }
        
        size_type max_size() const                                             { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template < typename K,
               unsigned INITIAL_REBUILD_TOLERANCE = 2,
               typename HASH_PROC = FHashProc<K>,
               typename EQUAL_PROC = FEqualProc<K>,
               typename TOLERANCE_INC_PROC = FRegularToleranceIncProc >
    class open_hash_set : public dictionary<K, K,
    INITIAL_REBUILD_TOLERANCE,
    HASH_PROC,
    EQUAL_PROC,
    TOLERANCE_INC_PROC,
    FHashSetBucketAllocator<K>,
    FHashSetListNodeAllocator<K> >
    {
    public:
        typedef typename FHashSetBucketAllocator<K>::pointer bucket_pointer;
        typedef typename FHashSetListNodeAllocator<K>::value_type node_type;
        typedef typename FHashSetListNodeAllocator<K>::pointer node_pointer;
        typedef
        dictionary<K, K,
        INITIAL_REBUILD_TOLERANCE,
        HASH_PROC,
        EQUAL_PROC,
        TOLERANCE_INC_PROC,
        FHashSetBucketAllocator<K>,
        FHashSetListNodeAllocator<K> > base_type;
        
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
                if (from->m_iter_next) {
                    to = from->m_iter_next;
                }
                else {
                    open_hash_set* dict = (open_hash_set*)from->m_hash_table;
                    if (from->m_bucket_index < dict->m_num_buckets - 1) {
                        for (euint32 i = from->m_bucket_index + 1; i < dict->m_num_buckets; i++) {
                            if (dict->m_buckets[i].begin()) {
                                to = dict->m_buckets[i].begin();
                                return;
                            }
                        }
                        to = nullptr;
                    }
                    else {
                        to = nullptr;
                    }
                }
            }
        };
        template<typename N>
        struct FPrevProc {
            void operator() ( N *from, N *&to, euint ele_real_size ) {
                if (from->m_iter_prev) {
                    to = from->m_iter_prev;
                }
                else {
                    open_hash_set* dict = (open_hash_set*)from->m_hash_table;
                    if (from->m_bucket_index > 0) {
                        for (esint32 i = (esint32)from->m_bucket_index - 1; i >= 0; i--) {
                            if (dict->m_buckets[i]->begin()) {
                                to = dict->m_buckets[i]->begin();
                                return;
                            }
                        }
                        to = nullptr;
                    }
                    else {
                        to = nullptr;
                    }
                }
            }
        };
    public:
        class iterator : public bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type>,
        FPrevProc<node_type> >
        {
        public:
            typedef bidirectional_readwrite_iterator<
            node_type,
            FReadProc<node_type>,
            FWriteProc<node_type>,
            FNextProc<node_type>,
            FPrevProc<node_type> > base_type;
            iterator ( node_type *a, euint ele_real_size )
            : base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
            {}
            iterator (  )
            : base_type (
                         NULL,
                         0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
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
            inline K& operator * () {
                return base_type::m_ptr->get_value();
            }
        };
        class const_iterator : public const_bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type>,
        FPrevProc<node_type> >
        {
        public:
            typedef const_bidirectional_readwrite_iterator<
            node_type,
            FReadProc<node_type>,
            FWriteProc<node_type>,
            FNextProc<node_type>,
            FPrevProc<node_type> > base_type;
            const_iterator ( node_type *a, euint ele_real_size )
            : base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
            {}
            const_iterator (  )
            : base_type (
                         NULL,
                         0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
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
            inline const K& operator * () const {
                return base_type::m_ptr->get_value();
            }
        };
    public:
        K* insert ( const K &key ) {
            return base_type::_insert(key, key);
        }
        K* insert ( K &&key ) {
            return base_type::_insert(key, key);
        }
        iterator begin() {
            for ( euint32 i = 0; i < base_type::m_num_buckets; i++ ) {
                if ( base_type::m_buckets[i].begin() ) {
                    return iterator ( base_type::m_buckets[i].begin(), 0 );
                }
            }
            return iterator ( nullptr, 0 );
        }
        iterator end() {
            return iterator ( nullptr, 0 );
        }
        const_iterator begin() const {
            for ( euint32 i = 0; i < base_type::m_num_buckets; i++ ) {
                if ( base_type::m_buckets[i].begin() ) {
                    return const_iterator ( base_type::m_buckets[i].begin(), 0 );
                }
            }
            return const_iterator ( nullptr, 0 );
        }
        const_iterator end() const {
            return const_iterator ( nullptr, 0 );
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
