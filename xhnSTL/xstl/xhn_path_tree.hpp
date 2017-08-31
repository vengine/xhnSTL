/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_PATH_TREE_HPP
#define XHN_PATH_TREE_HPP

#ifdef __cplusplus

#include "xhn_string.hpp"
#include "xhn_smart_ptr.hpp"
///#include "xhn_garbage_collector.hpp"
#define USE_SMART_PTR
namespace xhn
{
wstring get_upper_directory(const wstring& path);
void force_open_file(const wstring& path);
class wpath_node;
#ifdef USE_SMART_PTR
typedef SmartPtr<wpath_node> wpath_node_ptr;
#else
typedef garbage_collector::mem_handle<wpath_node> wpath_node_ptr;
#endif
class wpath_node : public RefObject
{
public:
	wstring path_name;
	wpath_node_ptr next;
    wpath_node_ptr children;
	bool is_folder;
public:
	wpath_node();
	~wpath_node();
	void search(const wchar_t* path);
	void get_paths(xhn::vector<xhn::wstring>& result);
};
class wpath_tree : public MemObject
{
public:
	wpath_node_ptr root;
public:
    wpath_tree(const wchar_t* dir);
	void get_all_paths(xhn::vector<xhn::wstring>& result);
};

class path_node;
#ifdef USE_SMART_PTR
typedef SmartPtr<path_node> path_node_ptr;
#else
typedef garbage_collector::mem_handle<path_node> path_node_ptr;
#endif

class walker : public RefObject
{
public:
    virtual void walk(path_node* node) = 0;
};
class path_node : public RefObject
{
public:
    string path_name;
    path_node_ptr next;
    path_node_ptr children;
    euint64 size;
    euint64 folder_count;
    euint64 file_count;
    bool is_folder;
public:
    path_node();
    ~path_node();
    void search(const char* path);
    void get_paths(xhn::vector<xhn::string>& result);
    void traverse(walker* walker);
};
    
class path_tree : public MemObject
{
public:
    euint64 totel_size;
    euint64 num_files;
    euint64 num_folders;
    path_node_ptr root;
public:
    path_tree(const char* dir);
    void get_all_paths(xhn::vector<xhn::string>& result);
    void traverse(walker* walker);
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
