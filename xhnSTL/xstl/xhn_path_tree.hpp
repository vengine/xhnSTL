#ifndef XHN_PATH_TREE_HPP
#define XHN_PATH_TREE_HPP
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