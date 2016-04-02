//
//  xhn_open_addressing_hash_table.hpp
//  xhnSTL
//
//  Created by 徐海宁 on 16/4/2.
//  Copyright © 2016年 徐 海宁. All rights reserved.
//

#ifndef xhn_open_addressing_hash_table_hpp
#define xhn_open_addressing_hash_table_hpp

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_singly_linked_list.hpp"
#include "xhn_memory.hpp"
#include "xhn_utility.hpp"

namespace xhn
{
    template <typename T>
    class hash_node
    {
    public:
        ALIGN_FLAG ( 16 ) T m_value;
        hash_node* m_iter_next;
        hash_node* m_iter_prev;
        euint32 m_hash_value;
        euint32 m_count;
        hash_node( const T &value )
        : m_value(value)
        {}
    };
    
    template <typename T>
    class FHashBucketAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef singly_linked_list<hash_node<T>>* pointer;
        typedef const singly_linked_list<hash_node<T>>* const_pointer;
        typedef singly_linked_list<hash_node<T>> value_type;
        typedef singly_linked_list<hash_node<T>>& reference;
        typedef const singly_linked_list<hash_node<T>>& const_reference;
        
        pointer address(reference v) const                           { return &v; }
        const_pointer address(const_reference v) const               { return &v; }
        FHashBucketAllocator()                                       {}
        FHashBucketAllocator& operator=(const FHashBucketAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                      { Mfree(ptr); }
        pointer allocate(size_type count)                            { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)               { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_node<T>& v)           { new (ptr) value_type( v ); }
        void construct(const pointer ptr)                            { new ( ptr ) value_type (); }
        void construct(pointer ptr, const T& v)                      { new ( ptr ) value_type (v); }
        void destroy(pointer ptr)                                    { ptr->~value_type(); }
        
        size_type max_size() const                                   { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    template <typename T>
    class FHashListNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef hash_node<T>* pointer;
        typedef const hash_node<T>* const_pointer;
        typedef hash_node<T> value_type;
        typedef hash_node<T>& reference;
        typedef const hash_node<T>& const_reference;
        
        pointer address(reference v) const                               { return &v; }
        const_pointer address(const_reference v) const                   { return &v; }
        FHashListNodeAllocator()                                         {}
        FHashListNodeAllocator& operator=(const FHashListNodeAllocator&) { return (*this); }
        void deallocate(pointer ptr, size_type)                          { Mfree(ptr); }
        pointer allocate(size_type count)                                { return (pointer)NMalloc(count * sizeof(value_type)); }
        pointer allocate(size_type count, const void*)                   { return (pointer)NMalloc(count * sizeof(value_type)); }
        void construct(pointer ptr, const hash_node<T>& v)               { new (ptr) value_type( v ); }
        void construct(const pointer ptr)                                { new ( ptr ) value_type (); }
        void construct(pointer ptr, const T& v)                          { new ( ptr ) value_type (v); }
        void destroy(pointer ptr)                                        { ptr->~value_type(); }
        
        size_type max_size() const                                       { return static_cast<size_type>(-1) / sizeof(value_type); }
    };
    
    template< typename K,
              typename V,
              typename HASH_PROC = FHashProc<K>,
              typename BUCKET_ALLOCATOR = FHashBucketAllocator<V>,
              typename NODE_ALLOCATOR = FHashListNodeAllocator<V> >
    class open_addressing_hash_table : public RefObject
    {
    public:
        singly_linked_list<hash_node<V>>* m_buckets;
        euint32 m_hash_mask;
        euint32 m_num_buckets;
        HASH_PROC m_hash_proc;
        BUCKET_ALLOCATOR m_bucket_allocator;
        NODE_ALLOCATOR m_node_allocator;
    private:
        void destroy_hash_node_list(singly_linked_list<hash_node<V>>* hash_node_list)
        {
            hash_node<V>* current_node = hash_node_list->begin();
            while (current_node) {
                hash_node<V>* deleted_node = current_node;
                current_node = hash_node_list->remove(current_node);
                m_node_allocator.destroy(deleted_node);
                m_node_allocator.deallocate(deleted_node);
            }
        }
        void rebuild()
        {
            singly_linked_list<hash_node<V>>* old_buckets = m_buckets;
            euint32 old_num_buckets = m_num_buckets;
            m_hash_mask <<= 1;
            m_hash_mask |= 0x1;
            euint32 new_num_buckets = m_num_buckets << 1;
            
            singly_linked_list<hash_node<V>>* new_buckets = m_bucket_allocator.allocate(new_num_buckets);
            for (euint32 i = 0; i < new_num_buckets; i++) {
                m_bucket_allocator.construct(&new_buckets[i]);
            }
            
            for (euint32 i = 0; i < old_num_buckets; i++) {
                singly_linked_list<hash_node<V>>* old_bucket = &old_buckets[i];
                hash_node<V>* current_node = old_bucket->begin();
                while (current_node) {
                    hash_node<V>* node = current_node;
                    current_node = current_node->m_iter_next;
                    singly_linked_list<hash_node<V>>* new_bucket = &new_buckets[node->m_hash_value & m_hash_mask];
                    euint32 count = 0;
                    if (new_bucket->begin()) {
                        count = new_bucket->begin()->m_count + 1;
                    }
                    node->m_count = count;
                    new_bucket->add(node);
                }
                m_bucket_allocator.destroy(old_bucket);
            }
            m_bucket_allocator.deallocate(old_buckets, old_num_buckets);
            
            m_buckets = new_buckets;
            m_num_buckets = new_num_buckets;
        }
        
        hash_node<V>* find_hash_node ( const K &key ) {
            euint32 hash_value = m_hash_proc(key);
            euint32 ukey = hash_value & m_hash_mask;
            singly_linked_list<hash_node<V>>* bucket = &m_buckets[ukey];
            hash_node<V>* current_node = bucket->begin();
            while (current_node) {
                if (current_node->m_hash_value == hash_value)
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
        V* find ( const K &key ) {
            hash_node<V>* node = find_hash_node( key );
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
            singly_linked_list<hash_node<V>>* bucket = &m_buckets[ukey];
            hash_node<V>* current_node = bucket->begin();
            while (current_node) {
                if (current_node->m_hash_value == hash_value) {
                    current_node->m_value = value;
                    return;
                }
                current_node = current_node->m_iter_next;
            }
            
            euint32 count = 0;
            hash_node<V>* head = bucket->begin();
            if (head) {
                count = head->m_count;
            }
            hash_node<V>* node = m_node_allocator.allocate(1);
            m_node_allocator.construct(node, value);
            
            bucket->add(node);
            
            node->m_hash_value = hash_value;
            node->m_count = count + 1;
            
            if (count > 4) {
                rebuild();
            }
        }
    };
    
}

#endif

#endif /* xhn_open_addressing_hash_table_hpp */
