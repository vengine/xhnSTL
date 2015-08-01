//
//  xhn_duplicate_finder.h
//  x_duplicate_cleaner
//
//  Created by 徐海宁 on 14-1-24.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef __x_duplicate_cleaner__xhn_duplicate_finder__
#define __x_duplicate_cleaner__xhn_duplicate_finder__
#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include "xhn_void_vector.hpp"
#include "xhn_file_stream.hpp"
#include "xhn_string.hpp"
#include "xhn_static_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_set.hpp"
#include "xhn_map.hpp"
#include "xhn_path_tree.hpp"
#include "xhn_lock.hpp"

namespace xhn
{
    class search_duplicate_files_status : public RefObject
    {
    public:
        enum status
        {
            Idling,
            Reading,
            Matching,
        };
    public:
        euint64 m_remainder_file_count;
        const char* m_processing_file;
        status m_current_status;
    public:
        search_duplicate_files_status(euint64 num_files)
        : m_remainder_file_count(num_files)
        , m_processing_file(nullptr)
        , m_current_status(Idling)
        {}
        search_duplicate_files_status* clone();
    };
    typedef SmartPtr<search_duplicate_files_status> search_duplicate_files_status_ptr;
    
    class file_handle
    {
    public:
        FILE* m_file;
        file_handle(const string& path)
        : m_file(nullptr)
        {
        }
        ~file_handle() {
            close();
        }
        inline void open(const string& path) {
            m_file = fopen(path.c_str(), "rb");
        }
        inline void Seek(euint64 offs) {
            fseek(m_file, offs, SEEK_SET);
        }
        inline void Read(void* buf, euint64 size) {
            fread(buf, 1, size, m_file);
        }
        inline void close() {
            if (m_file) {
                fclose(m_file);
                m_file = nullptr;
            }
        }
        inline bool operator!() const {
            return !m_file;
        }
        inline operator bool () const {
            return m_file != NULL;
        }
    };
    
    class duplicate_node;
    class duplicate_node_cache : public MemObject
    {
    public:
        duplicate_node* m_head;
        duplicate_node* m_tail;
        euint m_node_count;
    public:
        duplicate_node_cache()
        : m_head(nullptr)
        , m_tail(nullptr)
        , m_node_count(0)
        {}
        ~duplicate_node_cache();
        void open(duplicate_node* node);
        void remove(duplicate_node* node);
        void clear();
        static void test();
    };
    typedef euint SYMBOL;
    class duplicate_node : public MemObject
    {
    private:
        static search_duplicate_files_status* s_status;
    public:
        static void set_status(search_duplicate_files_status* status);
    public:
        duplicate_node* m_prev;
        duplicate_node* m_next;
        duplicate_node* m_next0;
        duplicate_node* m_next1;
        string m_path;
        static_string m_base_folder;
        file_handle m_file;
        euint64 m_offset;
        euint64 m_remainder;
        SYMBOL m_symbol;
        bool m_is_matched;
    public:
        duplicate_node(const string& path,
                       const static_string base_folder,
                       euint64 remainder)
        : m_prev(nullptr)
        , m_next(nullptr)
        , m_next0(nullptr)
        , m_next1(nullptr)
        , m_path(path)
        , m_base_folder(base_folder)
        , m_file(path)
        , m_offset(0)
        , m_remainder(remainder)
        , m_symbol(0)
        , m_is_matched(false)
        {}
        ~duplicate_node();
        euint64 prepare(duplicate_node_cache* cache,
                        search_duplicate_files_status* status,
                        SpinLock& lock);
        void match(vector<duplicate_node*>& entrances,
                   search_duplicate_files_status* status,
                   SpinLock& lock);
        euint64 complete(duplicate_node_cache* cache);
        void print();
        static void test();
    };
}

#endif /* defined(__x_duplicate_cleaner__xhn_duplicate_finder__) */
