/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef xhn_open_addressing_hash_table_hpp
#define xhn_open_addressing_hash_table_hpp

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_singly_linked_list.hpp"
#include "xhn_memory.hpp"
#include "xhn_utility.hpp"
#include <new>

namespace xhn
{
    template <typename K, typename V>
    class hash_node
    {
    public:
        ALIGN_FLAG ( 16 ) V m_value;
        ALIGN_FLAG ( 16 ) typename TKeyValue<K>::type m_key;
        hash_node* m_iter_next;
        hash_node* m_iter_prev;
        euint32 m_hash_value;
        euint32 m_count;
        vptr m_hash_table;
        euint32 m_bucket_index;
        hash_node( const K &key, const V &value )
        : m_value(value)
        , m_key(key)
        , m_hash_table(nullptr)
        , m_bucket_index(0)
        {}
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
        void destroy(pointer ptr)                                        { ptr->~value_type(); }
        
        size_type max_size() const                                       { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template< typename K,
              typename V,
              typename HASH_PROC = FHashProc<K>,
              typename EQUAL_PROC = FEqualProc<K>,
              typename BUCKET_ALLOCATOR = FHashBucketAllocator<K, V>,
              typename NODE_ALLOCATOR = FHashListNodeAllocator<K, V> >
    class open_addressing_hash_table : public RefObject
    {
    public:
        singly_linked_list<hash_node<K, V>>* m_buckets;
        euint32 m_hash_mask;
        euint32 m_num_buckets;
        HASH_PROC m_hash_proc;
        EQUAL_PROC m_equal_proc;
        BUCKET_ALLOCATOR m_bucket_allocator;
        NODE_ALLOCATOR m_node_allocator;
    private:
        void destroy_hash_node_list(singly_linked_list<hash_node<K, V>>* hash_node_list)
        {
            hash_node<K, V>* current_node = hash_node_list->begin();
            while (current_node) {
                hash_node<K, V>* deleted_node = current_node;
                current_node = hash_node_list->remove(current_node);
                m_node_allocator.destroy(deleted_node);
                m_node_allocator.deallocate(deleted_node, 1);
            }
        }
        void rebuild()
        {
            singly_linked_list<hash_node<K, V>>* old_buckets = m_buckets;
            euint32 old_num_buckets = m_num_buckets;
            m_hash_mask <<= 1;
            m_hash_mask |= 0x1;
            euint32 new_num_buckets = m_num_buckets << 1;
            
            singly_linked_list<hash_node<K, V>>* new_buckets = m_bucket_allocator.allocate(new_num_buckets);
            for (euint32 i = 0; i < new_num_buckets; i++) {
                m_bucket_allocator.construct(&new_buckets[i]);
            }
            
            for (euint32 i = 0; i < old_num_buckets; i++) {
                singly_linked_list<hash_node<K, V>>* old_bucket = &old_buckets[i];
                hash_node<K, V>* current_node = old_bucket->begin();
                while (current_node) {
                    hash_node<K, V>* node = current_node;
                    current_node = current_node->m_iter_next;
                    singly_linked_list<hash_node<K, V>>* new_bucket = &new_buckets[node->m_hash_value & m_hash_mask];
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
        
        hash_node<K, V>* find_hash_node ( const K &key ) const {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            singly_linked_list<hash_node<K, V>>* bucket = &m_buckets[ukey];
            hash_node<K, V>* current_node = bucket->begin();
            while (current_node) {
                if (current_node->m_key == key)
                    return current_node;
                current_node = current_node->m_iter_next;
            }
            return nullptr;
        }
    public:
        open_addressing_hash_table()
        : m_hash_mask(0x7)
        , m_num_buckets(8)
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
                m_bucket_allocator.destroy(&m_buckets[i]);
            }
            m_bucket_allocator.deallocate(m_buckets, m_num_buckets);
            
            m_hash_mask = 0x7;
            m_num_buckets = 8;
            m_buckets = m_bucket_allocator.allocate(m_num_buckets);
            for (euint32 i = 0; i < m_num_buckets; i++) {
                m_bucket_allocator.construct(&m_buckets[i]);
            }
        }
        ~open_addressing_hash_table()
        {
            for (euint32 i = 0; i < m_num_buckets; i++) {
                destroy_hash_node_list(&m_buckets[i]);
                m_bucket_allocator.destroy(&m_buckets[i]);
            }
            m_bucket_allocator.deallocate(m_buckets, m_num_buckets);
        }
        V* find ( const K &key ) const {
            hash_node<K, V>* node = find_hash_node( key );
            if (node) {
                return &node->m_value;
            }
            else {
                return nullptr;
            }
        }
        void insert ( const K &key, const V& value ) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            singly_linked_list<hash_node<K, V>>* bucket = &m_buckets[ukey];
            hash_node<K, V>* current_node = bucket->begin();
            while (current_node) {
                if (current_node->m_key == key) {
                    current_node->m_value = value;
                    return;
                }
                current_node = current_node->m_iter_next;
            }
            
            euint32 count = 0;
            hash_node<K, V>* head = bucket->begin();
            if (head) {
                count = head->m_count;
            }
            hash_node<K, V>* node = m_node_allocator.allocate(1);
            m_node_allocator.construct(node, key, value);
            node->m_hash_table = this;
            node->m_bucket_index = ukey;
            
            bucket->add(node);
            
            node->m_hash_value = hash_value;
            node->m_count = count + 1;
            
            if (count > 4) {
                rebuild();
            }
        }
        void remove ( const K &key ) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            singly_linked_list<hash_node<K, V>>* bucket = &m_buckets[ukey];
            hash_node<K, V>* current_node = bucket->begin();
            while (current_node) {
                if (current_node->m_key == key) {
                    bucket->remove(current_node);
                    
                    euint32 count = 0;
                    hash_node<K, V>* head = bucket->begin();
                    if (head) {
                        count = head->m_count;
                        count--;
                        head->m_count = count;
                    }
                    return;
                }
                current_node = current_node->m_iter_next;
            }
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
