/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_DICTIONARY_HPP
#define XHN_DICTIONARY_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_singly_linked_list.hpp"
#include "xhn_memory.hpp"
#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
#include <new>

namespace xhn
{
    template <typename K, typename V>
    class hash_node
    {
    public:
        typedef typename remove_const< K >::type key_type;
        typedef typename remove_const< V >::type value_type;
        typedef typename TKeyValue< key_type >::type hash_key_type;
    public:
        ALIGN_FLAG ( 16 ) value_type second;
        ALIGN_FLAG ( 16 ) hash_key_type first;
        hash_node* m_iter_next;
        hash_node* m_iter_prev;
        euint32 m_hash_value;
        euint32 m_count;
        vptr m_hash_table;
        euint32 m_bucket_index;
        hash_node( const K &key, const V &value )
        : second(value)
        , first((hash_key_type)key)
        , m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        hash_node( K &&key, V &&value )
        : second(value)
        , first(key)
        , m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        hash_node()
        : m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
        inline void set_value(const V &value) {
            second = value;
        }
        inline void set_value(V &&value) {
            second = value;
        }
        inline V& get_value() {
            return second;
        }
        inline const V& get_value() const {
            return second;
        }
        inline K get_key() const {
            return static_cast<K>(first);
        }
        inline K* get_key_pointer() const {
            return (K*)(&first);
        }
    };
    
    template <typename K, typename V>
    class FHashBucketAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef singly_linked_list<hash_node<K, V>>* pointer;
        typedef const singly_linked_list<hash_node<K, V>>* const_pointer;
        typedef singly_linked_list<hash_node<K, V>> value_type;
        typedef singly_linked_list<hash_node<K, V>>& reference;
        typedef const singly_linked_list<hash_node<K, V>>& const_reference;
        
        pointer address(reference v) const                           { return &v; }
        const_pointer address(const_reference v) const               { return &v; }
        FHashBucketAllocator()                                       {}
        FHashBucketAllocator& operator=(const FHashBucketAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                      { Mfree(ptr); }
        pointer allocate(size_type count)                            { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)               { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_node<K, V>& v)        { new ( ptr ) value_type( v ); }
        void construct(const pointer ptr)                            { new ( ptr ) value_type (); }
        void destroy(pointer ptr)                                    { ptr->~value_type(); }
        
        size_type max_size() const                                   { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template <typename K, typename V>
    class FHashListNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef hash_node<K, V>* pointer;
        typedef const hash_node<K, V>* const_pointer;
        typedef hash_node<K, V> value_type;
        typedef hash_node<K, V>& reference;
        typedef const hash_node<K, V>& const_reference;
        
        pointer address(reference v) const                               { return &v; }
        const_pointer address(const_reference v) const                   { return &v; }
        FHashListNodeAllocator()                                         {}
        FHashListNodeAllocator& operator=(const FHashListNodeAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                          { Mfree(ptr); }
        pointer allocate(size_type count)                                { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)                   { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_node<K, V>& v)            { new ( ptr ) value_type( v ); }
        void construct(const pointer ptr)                                { new ( ptr ) value_type (); }
        void construct(pointer ptr, const K& k, const V& v)              { new ( ptr ) value_type (k, v); }
        void construct(pointer ptr, K&& k, V&& v)                        { new ( ptr ) value_type (k, v); }
        void destroy(pointer ptr)                                        { ptr->~value_type(); }
        
        size_type max_size() const                                       { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template< typename K,
              typename V,
              unsigned INITIAL_REBUILD_TOLERANCE = 2,
              typename HASH_PROC = FHashProc<K>,
              typename EQUAL_PROC = FEqualProc<K>,
              typename TOLERANCE_INC_PROC = FRegularToleranceIncProc,
              typename BUCKET_ALLOCATOR = FHashBucketAllocator<K, V>,
              typename NODE_ALLOCATOR = FHashListNodeAllocator<K, V> >
    class dictionary : public RefObjectBase, public MemObject
    {
    public:
        typedef typename BUCKET_ALLOCATOR::pointer bucket_pointer;
        typedef typename NODE_ALLOCATOR::value_type node_type;
        typedef typename NODE_ALLOCATOR::pointer node_pointer;
    public:
        bucket_pointer m_buckets;
        euint32 m_hash_mask;
        euint32 m_num_buckets;
        euint32 m_rebuild_tolerance;
        HASH_PROC m_hash_proc;
        EQUAL_PROC m_equal_proc;
        TOLERANCE_INC_PROC m_tolerance_inc_proc;
        BUCKET_ALLOCATOR m_bucket_allocator;
        NODE_ALLOCATOR m_node_allocator;
        bool m_need_dealloc;
    
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
                    dictionary* dict = (dictionary*)from->m_hash_table;
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
                    dictionary* dict = (dictionary*)from->m_hash_table;
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
        };
    private:
        void destroy_hash_node_list(bucket_pointer hash_node_list)
        {
            node_pointer current_node = hash_node_list->begin();
            while (current_node) {
                node_pointer deleted_node = current_node;
                current_node = hash_node_list->remove(current_node);
                m_node_allocator.destroy(deleted_node);
                m_node_allocator.deallocate(deleted_node, 1);
            }
        }
        void rebuild()
        {
            bucket_pointer old_buckets = m_buckets;
            euint32 old_num_buckets = m_num_buckets;
            m_hash_mask <<= 3;
            m_hash_mask |= 0x7;
            m_rebuild_tolerance = m_tolerance_inc_proc(m_rebuild_tolerance);
            euint32 new_num_buckets = m_num_buckets << 3;
            
            bucket_pointer new_buckets = m_bucket_allocator.allocate(new_num_buckets);
            for (euint32 i = 0; i < new_num_buckets; i++) {
                m_bucket_allocator.construct(&new_buckets[i]);
            }
            
            for (euint32 i = 0; i < old_num_buckets; i++) {
                bucket_pointer old_bucket = &old_buckets[i];
                node_pointer current_node = old_bucket->begin();
                while (current_node) {
                    node_pointer node = current_node;
                    current_node = current_node->m_iter_next;
                    bucket_pointer new_bucket = &new_buckets[node->m_hash_value & m_hash_mask];
                    euint32 count = 0;
                    if (new_bucket->begin()) {
                        count = new_bucket->begin()->m_count + 1;
                    }
                    node->m_count = count;
                    node->m_bucket_index = node->m_hash_value & m_hash_mask;
                    new_bucket->add(node);
                }
                m_bucket_allocator.destroy(old_bucket);
            }
            m_bucket_allocator.deallocate(old_buckets, old_num_buckets);
            
            m_buckets = new_buckets;
            m_num_buckets = new_num_buckets;
        }
    public:
        node_pointer find_hash_node ( const K &key ) const {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            bucket_pointer bucket = &m_buckets[ukey];
            node_pointer current_node = bucket->begin();
            while (current_node) {
                if (current_node->first == key)
                    return current_node;
                current_node = current_node->m_iter_next;
            }
            return nullptr;
        }
    private:
        dictionary(const dictionary& d)
        : m_hash_mask(0x7)
        , m_num_buckets(8)
        , m_rebuild_tolerance(INITIAL_REBUILD_TOLERANCE)
        , m_need_dealloc(true)
        {
            m_buckets = m_bucket_allocator.allocate(m_num_buckets);
            for (euint32 i = 0; i < m_num_buckets; i++) {
                m_bucket_allocator.construct(&m_buckets[i]);
            }
        }
    public:
        dictionary()
        : m_hash_mask(0x7)
        , m_num_buckets(8)
        , m_rebuild_tolerance(INITIAL_REBUILD_TOLERANCE)
        , m_need_dealloc(true)
        {
            m_buckets = m_bucket_allocator.allocate(m_num_buckets);
            for (euint32 i = 0; i < m_num_buckets; i++) {
                m_bucket_allocator.construct(&m_buckets[i]);
            }
        }
        void clear()
        {
            for (euint32 i = 0; i < m_num_buckets; i++) {
                destroy_hash_node_list(&m_buckets[i]);
            }
        }
        ~dictionary()
        {
            if (m_need_dealloc) {
                for (euint32 i = 0; i < m_num_buckets; i++) {
                    destroy_hash_node_list(&m_buckets[i]);
                    m_bucket_allocator.destroy(&m_buckets[i]);
                }
                m_bucket_allocator.deallocate(m_buckets, m_num_buckets);
            }
        }
        V* find ( const K &key ) const {
            node_pointer node = find_hash_node( key );
            if (node) {
                return &node->get_value();
            }
            else {
                return nullptr;
            }
        }
        
        node_pointer find_node ( const K &key ) const {
            return find_hash_node( key );
        }
        
        template <typename KK, typename VV>
        V* _insert(KK key, VV value) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            bucket_pointer bucket = &m_buckets[ukey];
            node_pointer current_node = bucket->begin();
            while (current_node) {
                if (current_node->first == key) {
                    current_node->set_value( value );
                    return &current_node->get_value( );
                }
                current_node = current_node->m_iter_next;
            }
            
            euint32 count = 0;
            node_pointer head = bucket->begin();
            if (head) {
                count = head->m_count;
            }
            node_pointer node = m_node_allocator.allocate(1);
            m_node_allocator.construct(node, key, value);
            node->m_hash_table = this;
            node->m_bucket_index = ukey;
            
            bucket->add(node);
            
            node->m_hash_value = hash_value;
            node->m_count = count + 1;
            
            if (m_hash_mask != 0xffffffff && count > m_rebuild_tolerance) {
                rebuild();
            }
            return &node->get_value( );
        }
        
        template <typename KK, typename VV>
        node_pointer _insert_node(KK key, VV value) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            bucket_pointer bucket = &m_buckets[ukey];
            node_pointer current_node = bucket->begin();
            while (current_node) {
                if (current_node->first == key) {
                    current_node->set_value( value );
                    return current_node;
                }
                current_node = current_node->m_iter_next;
            }
            
            euint32 count = 0;
            node_pointer head = bucket->begin();
            if (head) {
                count = head->m_count;
            }
            node_pointer node = m_node_allocator.allocate(1);
            m_node_allocator.construct(node, key, value);
            node->m_hash_table = this;
            node->m_bucket_index = ukey;
            
            bucket->add(node);
            
            node->m_hash_value = hash_value;
            node->m_count = count + 1;
            
            if (m_hash_mask != 0xffffffff && count > m_rebuild_tolerance) {
                rebuild();
            }
            return node;
        }

    public:
        
        V* insert ( const K &key, const V& value ) {
            return _insert(key, value);
        }
        V* insert ( K &&key, V&& value ) {
            return _insert(key, value);
        }
        V* insert ( const K &key, V&& value ) {
            return _insert(key, value);
        }
        V* insert ( K &&key, const V& value ) {
            return _insert(key, value);
        }
        
        node_pointer insert_node ( const K &key, const V& value ) {
            return _insert_node(key, value);
        }
        node_pointer insert_node ( K &&key, V&& value ) {
            return _insert_node(key, value);
        }
        node_pointer insert_node ( const K &key, V&& value ) {
            return _insert_node(key, value);
        }
        node_pointer insert_node ( K &&key, const V& value ) {
            return _insert_node(key, value);
        }
        
        void remove ( const K &key ) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            bucket_pointer bucket = &m_buckets[ukey];
            node_pointer current_node = bucket->begin();
            while (current_node) {
                if (current_node->first == key) {
                    euint32 count = 0;
                    node_pointer head = bucket->begin();
                    if (head) {
                        count = head->m_count - 1;
                    }
                    bucket->remove(current_node);
                    head = bucket->begin();
                    if (head) {
                        head->m_count = count;
                    }
                    m_node_allocator.destroy(current_node);
                    m_node_allocator.deallocate(current_node, 1);
                    return;
                }
                current_node = current_node->m_iter_next;
            }
        }
        iterator begin() {
            for (euint32 i = 0; i < m_num_buckets; i++) {
                if (m_buckets[i].begin()) {
                    return iterator ( m_buckets[i].begin(), 0 );
                }
            }
            return iterator ( nullptr, 0 );
        }
        iterator end() {
            return iterator ( nullptr, 0 );
        }
        const_iterator begin() const {
            for (euint32 i = 0; i < m_num_buckets; i++) {
                if (m_buckets[i].begin()) {
                    return const_iterator ( m_buckets[i].begin(), 0 );
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
