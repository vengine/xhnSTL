#include "xhn_path_tree.hpp"
#if defined(_WIN32) || defined(_WIN64)
#    include <io.h>
#elif defined(__APPLE__)
#    include "apple_file_manager.h"
#else
#    error
#endif



xhn::wstring xhn::get_upper_directory(const wstring& path)
{
    wstring ret;
    wstring tmp = path;
    transform(tmp.begin(), tmp.end(), tmp.begin(), FWCharFormat());
    if (tmp.size() && tmp[tmp.size() - 1] == L'/') {
        tmp[tmp.size() - 1] = 0;
        tmp.resize(tmp.size() - 1);
    }
    ret.resize(tmp.size());
    wstring::iterator iter = tmp.begin();
    wstring::iterator end = tmp.end();
    euint count = 0;
    euint lastSlash = (euint)-1;
    bool foundSlash = false;
    for (; iter != end; iter++) {
        if (*iter == L'/') {
            if (!foundSlash) {
                foundSlash = true;
                ret[count] = *iter;
                lastSlash = count;
                count++;
            }
        }
        else {
            foundSlash = false;
            ret[count] = *iter;
            count++;
        }
    }
    if (lastSlash != (euint)-1) {
        ret[lastSlash + 1] = 0;
        ret.resize(lastSlash + 1);
    }
    else {
        ret[count] = 0;
        ret.resize(count);
    }
    return ret;
}

void xhn::force_open_file(const wstring& path)
{
    wstring upperDir = get_upper_directory(path);
    list<wstring> dirStack;
    xhn::file_manager* mgr = xhn::file_manager::get();
    bool isDir = false;
    while (!mgr->is_exist(upperDir, isDir)) {
        dirStack.push_front(upperDir);
        upperDir = get_upper_directory(upperDir);
    }
    if (dirStack.size()) {
        list<wstring>::iterator iter = dirStack.begin();
        list<wstring>::iterator end = dirStack.end();
        for (; iter != end; iter++) {
            wstring& dir = *iter;
            mgr->create_directory(dir);
        }
    }
    mgr->create_file(path);
}

int StrCmpW(const wchar_t* ws0, const wchar_t* ws1)
{
	int count = 0;
	while (ws0[count] && ws1[count]) {
		if (ws0[count] > ws1[count])
			return 1;
		else if (ws0[count] < ws1[count])
			return -1;
		count++;
	}
	if (ws0[count] && !ws1[count])
		return 1;
	else if (!ws0[count] && ws1[count])
		return -1;
	else
		return 0;
}

xhn::wpath_node::wpath_node()
: is_folder(false)
{
}

xhn::wpath_node::~wpath_node()
{
}

void xhn::wpath_node::search(const wchar_t* path)
{
	path_name = path;
	transform(path_name.begin(), path_name.end(), path_name.begin(), FWCharFormat());
    wstring search_path = path_name;
#if defined(_WIN32) || defined(_WIN64)
	if (search_path.size() && search_path[search_path.size() - 1] != L'\\') {
		search_path += L'\\';
	}
#elif defined(__APPLE__)
    if (search_path.size() && search_path[search_path.size() - 1] != L'/') {
		search_path += L'/';
	}
#else
#    error
#endif
#if defined(_WIN32) || defined(_WIN64)
	wstring tmp = search_path +  L"*";

	struct _wfinddata_t fd;
	long h = _wfindfirst(tmp.c_str(), &fd);
	int ret = 0;
	while (h != -1L && !ret)
	{
		if (StrCmpW(fd.name, L".") != 0 && StrCmpW(fd.name, L"..") != 0)
		{
			wpath_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW wpath_node;
#else
            child = GC_ALLOC(wpath_node);
#endif
			child->search((search_path + fd.name).c_str());
			if (fd.attrib == 0x10) {
			    child->is_folder = true;
			}
			child->next = children;
			children = child;
		}
		ret = _wfindnext(h, &fd);
	}
	_findclose(h);
#elif defined(__APPLE__)
    Utf8 utf8(search_path.c_str());
    vector<string> subFolders;
    vector<string> paths;
    GetPaths(((string)utf8).c_str(), subFolders, paths);
    {
        vector<string>::iterator iter = subFolders.begin();
        vector<string>::iterator end = subFolders.end();
        for (; iter != end; iter++) {
            string& subFolder = *iter;
            Unicode uc16(subFolder.c_str());
            wpath_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW wpath_node;
#else
            child = GC_ALLOC(wpath_node);
#endif
            child->search(((wstring)uc16).c_str());
            child->is_folder = true;
            child->next = children;
			children = child;
        }
    }
    {
        vector<string>::iterator iter = paths.begin();
        vector<string>::iterator end = paths.end();
        for (; iter != end; iter++) {
            string& path = *iter;
            Unicode uc16(path.c_str());
            wpath_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW wpath_node;
#else
            child = GC_ALLOC(wpath_node);
#endif
            child->path_name = uc16;
            child->is_folder = false;
            child->next = children;
			children = child;
        }
    }
#else
#    error
#endif
}

void xhn::wpath_node::get_paths(xhn::vector<xhn::wstring>& result)
{
	if (!children) {
		if (!is_folder)
		    result.push_back(path_name);
		return;
	}
    wpath_node_ptr child;
	child = children;
	while (child) {
		child->get_paths(result);
		child = child->next;
	}
}

xhn::wpath_tree::wpath_tree(const wchar_t* dir)
{
#ifdef USE_SMART_PTR
    root = VNEW wpath_node;
#else
	root = xhn::garbage_collector::get()->alloc<path_node>(__FILE__, __LINE__, "root");
#endif
	root->search(dir);
}

void xhn::wpath_tree::get_all_paths(xhn::vector<xhn::wstring>& result)
{
    root->get_paths(result);
}



xhn::path_node::path_node()
: is_folder(false)
, size(0)
, folder_count(0)
, file_count(0)
{
}

xhn::path_node::~path_node()
{
}

void xhn::path_node::search(const char* path)
{
	path_name = path;
	transform(path_name.begin(), path_name.end(), path_name.begin(), FCharFormat());
    string search_path = path_name;
#if defined(_WIN32) || defined(_WIN64)
	if (search_path.size() && search_path[search_path.size() - 1] != '\\') {
		search_path += '\\';
	}
#elif defined(__APPLE__)
    if (search_path.size() && search_path[search_path.size() - 1] != '/') {
		search_path += '/';
	}
#else
#    error
#endif
#if defined(_WIN32) || defined(_WIN64)
	string tmp = search_path +  "*";
    
	struct _finddata_t fd;
	long h = _findfirst(tmp.c_str(), &fd);
	int ret = 0;
	while (h != -1L && !ret)
	{
		if (strcmp(fd.name, L".") != 0 && strcmp(fd.name, L"..") != 0)
		{
			path_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW path_node;
#else
            child = GC_ALLOC(path_node);
#endif
			child->search((search_path + fd.name).c_str());
			if (fd.attrib == 0x10) {
			    child->is_folder = true;
			}
			child->next = children;
			children = child;
		}
		ret = _findnext(h, &fd);
	}
	_findclose(h);
#elif defined(__APPLE__)
    vector<string> subFolders;
    vector<string> paths;
    GetPaths(search_path.c_str(), subFolders, paths);
    {
        vector<string>::iterator iter = subFolders.begin();
        vector<string>::iterator end = subFolders.end();
        for (; iter != end; iter++) {
            string& subFolder = *iter;
            path_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW path_node;
#else
            child = GC_ALLOC(path_node);
#endif
            child->search(subFolder.c_str());
            child->is_folder = true;
            child->next = children;
			children = child;
            
            size += child->size;
            folder_count += child->folder_count;
            file_count += child->file_count;
            folder_count++;
        }
    }
    {
        vector<string>::iterator iter = paths.begin();
        vector<string>::iterator end = paths.end();
        for (; iter != end; iter++) {
            string& path = *iter;
            path_node_ptr child;
#ifdef USE_SMART_PTR
            child = VNEW path_node;
#else
            child = GC_ALLOC(path_node);
#endif
            child->path_name = path;
            child->is_folder = false;
            child->next = children;
			children = child;
            
            size += file_manager::get()->file_size(child->path_name);
            file_count++;
        }
    }
#else
#    error
#endif
}

void xhn::path_node::get_paths(xhn::vector<xhn::string>& result)
{
	if (!children) {
		if (!is_folder)
		    result.push_back(path_name);
		return;
	}
    path_node_ptr child;
	child = children;
	while (child) {
		child->get_paths(result);
		child = child->next;
	}
}

void xhn::path_node::traverse(xhn::walker* walker)
{
    path_node_ptr child;
	child = children;
	while (child) {
		child->traverse(walker);
		child = child->next;
	}
    walker->walk(this);
}

xhn::path_tree::path_tree(const char* dir)
{
#ifdef USE_SMART_PTR
    root = VNEW path_node;
#else
	root = xhn::garbage_collector::get()->alloc<path_node>(__FILE__, __LINE__, "root");
#endif
	root->search(dir);
    totel_size = root->size;
    num_folders = root->folder_count;
    num_files = root->file_count;
}

void xhn::path_tree::get_all_paths(xhn::vector<xhn::string>& result)
{
    root->get_paths(result);
}

void xhn::path_tree::traverse(xhn::walker* walker)
{
    root->traverse(walker);
}