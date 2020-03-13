/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_FILE_STREAM_HPP
#define XHN_FILE_STREAM_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"

#include "xhn_memory.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_string.hpp"

#include "xhn_btree.hpp"
#include "xhn_void_vector.hpp"

#include "xhn_lock.hpp"

#include "xhn_operation_manager.hpp"

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 16
namespace xhn
{
    class file_stream;
    class FFileBlockAllocator;
    class XHN_EXPORT file_block :
    public btree_node<
    euint64,
    euint64,
    UnlockedMapNodeAllocator< range<euint64>, vptr >
    >
    {
    public:
        file_block* prev;
        file_block* next;
        euint64 buffered_size;
        file_stream* stream;
        btree<file_block, euint64, euint64, FFileBlockAllocator>* tree;
        void_vector buffer;
    public:
        file_block();
        ~file_block();
        void write(euint64 offs, const euint8* buf, euint64 size);
        void read(euint64 offs, euint8* buf, euint64 size);
        void init();
        euint8* access(euint64 offs, euint64 size);
        const euint8* access(euint64 offs, euint64 size) const;
    };
    class FFileBlockAllocator
    {
    public:
        FFileBlockAllocator() {}
        void deallocate(file_block* ptr, euint) { Mfree(ptr); }
        file_block* allocate(euint count) { return (file_block*)NMalloc(count * sizeof(file_block)); }
        void construct(file_block* ptr, vptr) { new ( ptr ) file_block (); }
        void pre_destroy(file_block* ptr) {}
        void destroy(file_block* ptr) { ptr->~file_block(); }
    };
    /// file_implement 里面必须携带文件句柄
    class file_implement : public RefObjectBase, public MemObject
    {
    public:
        virtual ~file_implement() {}
        virtual bool open(const xhn::wstring& path) = 0;
        virtual void close() = 0;
        virtual euint64 read(euint8* buf, euint64 size) = 0;
        virtual euint64 write(const euint8* buf, euint64 size) = 0;
        virtual euint64 set_pos(euint64 pos) = 0;
        virtual euint64 get_pos() = 0;
        virtual euint64 get_size() = 0;
    };
    typedef xhn::SmartPtr<file_implement> file_implement_ptr;
    class file_stream : public RefObjectBase, public MemObject
    {
        friend class file_block;
    private:
        MutexLock lock;
        file_block* head;
        file_block* tail;
        euint64 file_block_count;
        file_block* cache0;
        file_block* cache1;
        btree<
        file_block,
        euint64,
        euint64,
        xhn::FFileBlockAllocator> file_block_tree;
        SpinLock spin_lock;
    protected:
        static xhn::file_block* access(file_stream* file, euint64 pos);
        void synchronize_file();
        void push_file_block(xhn::file_block* node);
    public:
        file_stream()
        : head(NULL)
        , tail(NULL)
        , file_block_count(0)
        , cache0(NULL)
        , cache1(NULL)
        {}
        virtual ~file_stream() {}
        virtual euint64 read(euint8* buffer, euint64 size) = 0;
        virtual euint64 write(const euint8* buffer, euint64 size) = 0;
        virtual euint64 get_size() = 0;
        virtual euint64 get_pos() = 0;
        virtual euint64 set_pos(euint64 pos) = 0;
        virtual void set_base_offset(euint64 offs) = 0;
        virtual euint8& operator[] (euint64 pos) = 0;
        virtual const euint8& operator[] (euint64 pos)const = 0;
    };
    typedef xhn::SmartPtr<file_stream> file_stream_ptr;
    
    class XHN_EXPORT filer : public file_stream
    {
    public:
        file_implement_ptr m_implement;
        euint64 m_baseOffset;
    public:
        filer(file_implement_ptr implement)
        : m_implement(implement)
        , m_baseOffset(0)
        {}
        virtual ~filer();
        virtual euint64 read(euint8* buffer, euint64 size);
        virtual euint64 write(const euint8* buffer, euint64 size);
        virtual euint64 get_size();
        virtual euint64 get_pos();
        virtual euint64 set_pos(euint64 pos);
        virtual void set_base_offset(euint64 offs);
        virtual euint8& operator[] (euint64 pos);
        virtual const euint8& operator[] (euint64 pos)const;
    };
    
    class XHN_EXPORT console_output_device : public file_implement
    {
    public:
        virtual bool open(const xhn::wstring& path);
        virtual void close();
        virtual euint64 read(euint8* buf, euint64 size);
        virtual euint64 write(const euint8* buf, euint64 size);
        virtual euint64 set_pos(euint64 pos);
        virtual euint64 get_pos();
        virtual euint64 get_size();
    };
    
    class XHN_EXPORT memory_output_device : public file_implement
    {
    public:
        euint8* m_buffer;
        euint64 m_size;
        euint64 m_capacity;
    public:
        memory_output_device()
        : m_buffer(NULL)
        , m_size(0)
        , m_capacity(0)
        {}
        ~memory_output_device();
        virtual bool open(const xhn::wstring& path);
        virtual void close();
        virtual euint64 read(euint8* buf, euint64 size);
        virtual euint64 write(const euint8* buf, euint64 size);
        virtual euint64 set_pos(euint64 pos);
        virtual euint64 get_pos();
        virtual euint64 get_size();
    };
    
    /// 比特流采用先入先出设计
    /// 高                                     低
    /// |-------- -------- -------- --------|
    /// |-------- -------- -------- -------1| 先写入1个bit
    /// |-------- -------- -------- --543211| 写入5个bit
    /// |-------- -------- -------3 21543211| 写入3个bit
    /// |-------- -------- ------13 21543211| 写入1个bit
    /// |-------- -------- -----113 21543211| 写入1个bit
    /// |-------- -------- ------11 32154321| 读取1个bit
    /// |-------- -------- -------- ---11321| 读取5个bit
    /// |-------- -------- -------- ------11| 读取3个bit
    /// |-------- -------- -------- -------1| 读取1个bit
    /// |-------- -------- -------- --------| 读取1个bit
    
    class XHN_EXPORT bit_writer : public RefObjectBase, public MemObject
    {
    public:
        file_stream_ptr m_file;
        euint8 m_byte_buffer;
        euint m_bit_count;
        euint m_write_count;
    public:
        void synchronize_file();
        bit_writer(file_stream_ptr file)
        : m_file(file)
        , m_byte_buffer(0)
        , m_bit_count(0)
        , m_write_count(0)
        {}
        ~bit_writer();
        void write_bit(bool bit);
        void write_bits(void* bits, euint num_bits);
        void write_byte(euint8 byte);
        inline euint writed_size() {
            return m_write_count;
        }
        inline euint bit_count() {
            return m_bit_count;
        }
        inline euint8 byte_buffer() {
            return m_byte_buffer;
        }
        inline void reset() {
            m_byte_buffer = 0;
            m_bit_count = 0;
            m_write_count = 0;
        }
    };
    typedef SmartPtr<bit_writer> bit_writer_ptr;
    
    class XHN_EXPORT bit_reader : public RefObjectBase, public MemObject
    {
    public:
        file_stream_ptr m_file;
        euint8 m_byte_buffer;
        euint m_bit_count;
    public:
        bit_reader(file_stream_ptr file)
        : m_file(file)
        , m_byte_buffer(0)
        , m_bit_count(0)
        {}
        ~bit_reader() {}
        bool read_bit(bool* output);
        void read_bits(void* output, euint* num_bits);
        bool read_byte(euint8* output);
        inline void reset() {
            m_byte_buffer = 0;
            m_bit_count = 0;
        }
    };
    typedef SmartPtr<bit_reader> bit_reader_ptr;
    
    class path : public RefObjectBase, public MemObject
    {
    public:
        string m_path;
        string m_base_folder;
    };
    typedef SmartPtr<path> path_ptr;
    
    class path_enumerator : public RefObjectBase, public MemObject
    {
    public:
        virtual ~path_enumerator() {}
        virtual path_ptr next_path() = 0;
    };
    typedef SmartPtr<path_enumerator> path_enumerator_ptr;
    
    class XHN_EXPORT folder_information : public operation_data
    {
        DeclareRTTI;
    public:
        euint64 m_totel_size;
        euint64 m_image_size;
        euint64 m_movie_size;
        euint64 m_music_size;
        euint64 m_num_files;
        euint64 m_num_folders;
        bool m_cancel_flag;
    public:
        folder_information()
        : m_totel_size(0)
        , m_image_size(0)
        , m_movie_size(0)
        , m_music_size(0)
        , m_num_files(0)
        , m_num_folders(0)
        , m_cancel_flag(false)
        {}
        virtual ~folder_information() {}
        virtual void clear() {
            m_totel_size = 0;
            m_image_size = 0;
            m_movie_size = 0;
            m_music_size = 0;
            m_num_files = 0;
            m_num_folders = 0;
            m_cancel_flag = false;
        }
        virtual void collect_filename(const string& filename) = 0;
        virtual void collect_folder(const string& folder) = 0;
        inline void cancel() {
            m_cancel_flag = true;
        }
    };
    
    class XHN_EXPORT file_manager : public MemObject
    {
    public:
        virtual ~file_manager() {}
        virtual bool is_exist(const xhn::wstring& path, bool& is_directory) = 0;
        virtual bool is_exist(const xhn::string& path, bool& is_directory) = 0;
        virtual bool create_directory(const xhn::wstring& dir) = 0;
        virtual bool create_file(const xhn::wstring& path) = 0;
        virtual file_stream_ptr open(const xhn::wstring& path) = 0;
        virtual file_stream_ptr create_and_open(const xhn::wstring& path) = 0;
        virtual void delete_file(const xhn::wstring& path) = 0;
        virtual euint64 file_size(const xhn::wstring& path) = 0;
        virtual euint64 file_size(const xhn::string& path) = 0;
        virtual file_stream_ptr open(file_implement_ptr _interface, const xhn::wstring& path);
        virtual xhn::wstring get_home_dir() = 0;
        virtual void get_resource_dirs(xhn::vector<xhn::wstring>& result) = 0;
        virtual void get_system_font_dirs(xhn::vector<xhn::wstring>& result) = 0;
        virtual void set_access_permission(const xhn::wstring& path, euint32 accessPermisson) = 0;
        virtual bool is_non_sandbox_dir(const xhn::wstring& path) = 0;
        virtual bool get_folder_information(const wstring& path, folder_information& info) = 0;
        virtual bool get_folder_information(const string& path, folder_information& info) = 0;
        void force_create_file(const wstring& path);
        euint64 compare_two_files(const wstring& path0, const wstring& path1);
        static file_manager* get();
        static wstring get_upper_directory(const wstring& path);
        static string get_upper_directory(const string& path);
        static wstring get_file_name(const wstring& path);
        static string get_file_name(const string& path);
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
