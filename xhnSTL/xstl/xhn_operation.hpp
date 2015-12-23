/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef __xhn_operation__
#define __xhn_operation__

#include "xhn_operation_manager.hpp"
#include "xhn_map.hpp"
#include "xhn_vector.hpp"
#include "xhn_file_stream.hpp"
#include "xhn_duplicate_finder.h"
#include "xhn_lock.hpp"
namespace xhn
{
///==========================================================================///
///  collect_folder_information                                              ///
///==========================================================================///
    
class collect_folder_information_operation : public operation
{
private:
    operation_data_ptr m_data;
    operation_data_ptr m_folder_information;
    const char* m_folder;
public:
    collect_folder_information_operation(const char* folder, folder_information* info)
    : m_folder(folder)
    , m_folder_information(info)
    {}
    virtual bool run();
    virtual float get_progress_rate();
    virtual void send_data(operation_handle* handle);
    virtual void cancel();
    virtual const static_string type() const;
};
    
class collect_folder_information_operatoion_handle : public operation_handle
{
private:
    virtual void process_impl(operation_data_ptr data);
public:
    collect_folder_information_operatoion_handle(collect_folder_information_operation* op)
    : operation_handle(op)
    {}
    virtual void collect_folder_information(folder_information* info) = 0;
};
    
template <typename T>
collect_folder_information_operatoion_handle*
new_collect_folder_information_operation(const char* folder, folder_information* info)
{
    collect_folder_information_operatoion_handle* handle =
    operation_manager::get()->add_operatoion2<
    collect_folder_information_operation,
    T,
    const char*,
    folder_information*>
    (folder, info);
    return handle;
}
///==========================================================================///
///  collect_folder_information                                              ///
///==========================================================================///
///==========================================================================///
///  search_duplicate_files                                                  ///
///==========================================================================///
class duplicate_node;
class duplicate_node_cache;
class duplicate_files : public operation_data
{
    DeclareRTTI;
public:
    map<euint64, vector<duplicate_node*>> m_duplicate_files;
    void print();
};
    
class search_duplicate_files_operation : public operation
{
private:
    duplicate_node_cache* m_cache;
    operation_data_ptr m_data;
    path_enumerator_ptr m_paths;
    euint64 m_proced_size;
    euint64 m_totel_size;
    volatile esint32 m_cancel_flag;
    search_duplicate_files_status_ptr m_status;
    SpinLock m_lock;
public:
    search_duplicate_files_operation(path_enumerator_ptr paths)
    : m_cache(nullptr)
    , m_paths(paths)
    , m_proced_size(0)
    , m_totel_size(MAX_EUINT64)
    , m_cancel_flag(0)
    {}
    virtual ~search_duplicate_files_operation();
    virtual bool run();
    virtual float get_progress_rate();
    virtual void send_data(operation_handle* handle);
    virtual void cancel();
    virtual const static_string type() const;
    void prepare();
    void find();
    void print();
    inline search_duplicate_files_status* get_status() {
        return m_status.get();
    }
    inline SpinLock& get_lock() {
        return m_lock;
    }
    inline bool get_cancel_flag() {
        return m_cancel_flag;
    }
};
    
class search_duplicate_files_operation_handle : public operation_handle
{
private:
    search_duplicate_files_operation* m_search_duplicate_files_operation;
private:
    virtual void process_impl(operation_data_ptr data);
public:
    search_duplicate_files_operation_handle(search_duplicate_files_operation* op)
    : operation_handle(op)
    , m_search_duplicate_files_operation(op)
    {}
    inline search_duplicate_files_status* get_status() {
        return m_search_duplicate_files_operation->get_status();
    }
    inline SpinLock& get_lock() {
        return m_search_duplicate_files_operation->get_lock();
    }
    virtual void collect_duplicate_files(duplicate_files* files) = 0;
    inline bool get_cancel_flag() {
        return m_search_duplicate_files_operation->get_cancel_flag();
    }
};
    
template <typename T>
search_duplicate_files_operation_handle*
new_search_duplicate_files_operation(path_enumerator_ptr paths)
{
    search_duplicate_files_operation_handle* handle =
    operation_manager::get()->add_operatoion<
    search_duplicate_files_operation,
    T,
    path_enumerator_ptr>
    (paths);
    return handle;
}
    
template <typename T, typename P>
search_duplicate_files_operation_handle*
new_search_duplicate_files_operation(path_enumerator_ptr paths, P p)
{
    search_duplicate_files_operation_handle* handle =
    operation_manager::get()->add_operatoion<
    search_duplicate_files_operation,
    T,
    path_enumerator_ptr,
    P>
    (paths, p);
    return handle;
}
///==========================================================================///
///  search_duplicate_files                                                  ///
///==========================================================================///
}

#endif /* defined(__cleaner__xhn_operation__) */

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
