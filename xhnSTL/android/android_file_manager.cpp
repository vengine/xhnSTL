/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "android_file_manager.hpp"
#include "xhn_set.hpp"
#include "xhn_string.hpp"
#include "elog.h"
#include <sys/stat.h>
#include <dirent.h>
#include <strings.h>
#include <unistd.h>

AAssetManager* s_AAssetManager = nullptr;

void _GetHomeDirectory(char* output, int outlen)
{
    snprintf(output, outlen, "/");
}

void AndroidFileInit(AAssetManager* assetManager)
{
    GetHomeDirectory = _GetHomeDirectory;
    s_AAssetManager = assetManager;
}

void GetFilenames(FileDirectory dir, FilenameArray* filenames)
{
}

void dir_oper(char const*path,
              xhn::vector<xhn::string>& subFolders,
              xhn::vector<xhn::string>& paths)
{
    struct dirent *filename;
    struct stat s_buf;
    DIR *dp = opendir(path);
    
    /*readdir()必须循环调用，要读完整个目录的文件，readdir才会返回NULL
     若未读完，就让他循环*/
    while(filename = readdir(dp))
    {
        /*判断一个文件是目录还是一个普通文件*/
        char file_path[200];
        bzero(file_path,200);
        strcat(file_path,path);
        strcat(file_path,"/");
        strcat(file_path,filename->d_name);
        
        /*在linux下每一个目录都有隐藏的. 和..目录，一定要把这两个排除掉。因为没有意义且会导致死循环*/
        if(strcmp(filename->d_name,".")==0||strcmp(filename->d_name,"..")==0)
        {
            continue;
        }
        
        /*获取文件信息，把信息放到s_buf中*/
        stat(file_path,&s_buf);
        
        /*判断是否目录*/
        if(S_ISDIR(s_buf.st_mode))
        {
            subFolders.push_back(xhn::string(file_path));
            dir_oper(file_path, subFolders, paths);
        }
        
        /*判断是否为普通文件*/
        if(S_ISREG(s_buf.st_mode))
        {
            paths.push_back(xhn::string(file_path));
        }
    }
}

void GetPaths(const char* baseFolder,
              xhn::vector<xhn::string>& subFolders,
              xhn::vector<xhn::string>& paths)
{
    dir_oper(baseFolder, subFolders, paths);
}

AndroidFileManager::AndroidFileManager()
{
}
AndroidFileManager::~AndroidFileManager()
{
}
bool AndroidFileManager::is_exist(const xhn::wstring& dir, bool& is_directory)
{
    xhn::Utf8 utf8Path(dir.c_str());
    return is_exist((xhn::string)utf8Path, is_directory);
}
bool AndroidFileManager::is_exist(const xhn::string& path, bool& is_directory)
{
    if (path.size() == 0) {
        is_directory = true;
        return true;
    }
    AAsset* asset = AAssetManager_open(s_AAssetManager, path.c_str(), AASSET_MODE_STREAMING);
    if (asset) {
        AAsset_close(asset);
        is_directory = false;
        return true;
    }
    AAssetDir* assetDir = AAssetManager_openDir(s_AAssetManager, path.c_str());
    if (assetDir) {
        bool ret = false;
        if (AAssetDir_getNextFileName(assetDir)) {
            is_directory = true;
            ret = true;
        }
        AAssetDir_close(assetDir);
        return ret;
    }
    return false;
}
bool AndroidFileManager::create_directory(const xhn::wstring& dir)
{
    return false;
}
bool AndroidFileManager::create_file(const xhn::wstring& path)
{
    return false;
}
xhn::file_stream_ptr AndroidFileManager::open(const xhn::wstring& path)
{
    return nullptr;
}
xhn::file_stream_ptr AndroidFileManager::create_and_open(const xhn::wstring& path)
{
    return nullptr;
}
void AndroidFileManager::delete_file(const xhn::wstring& path)
{
}
euint64 AndroidFileManager::file_size(const xhn::wstring& path)
{
    return 0;
}
euint64 AndroidFileManager::file_size(const xhn::string& path)
{
    return 0;
}
xhn::wstring AndroidFileManager::get_home_dir()
{
    return L"/home";
}
void AndroidFileManager::get_resource_dirs(xhn::vector<xhn::wstring>& result)
{
}
void AndroidFileManager::get_system_font_dirs(xhn::vector<xhn::wstring>& result)
{
}

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;  
    FILE *fp;  
    fp = fopen(path, "r");  
    if(fp == NULL)  
        return filesize;  
    fseek(fp, 0L, SEEK_END);  
    filesize = ftell(fp);  
    fclose(fp);  
    return filesize;  
} 

//void _get_folder_information(char const*path, xhn::folder_information& info)
//{
//    struct dirent *filename;
//    struct stat s_buf;
//    DIR *dp = opendir(path);
//
//    /*readdir()必须循环调用，要读完整个目录的文件，readdir才会返回NULL
//     若未读完，就让他循环*/
//    while(filename = readdir(dp))
//    {
//        /*判断一个文件是目录还是一个普通文件*/
//        char file_path[200];
//        bzero(file_path,200);
//        strcat(file_path,path);
//        strcat(file_path,"/");
//        strcat(file_path,filename->d_name);
//
//        /*在linux下每一个目录都有隐藏的. 和..目录，一定要把这两个排除掉。因为没有意义且会导致死循环*/
//        if(strcmp(filename->d_name,".")==0||strcmp(filename->d_name,"..")==0)
//        {
//            continue;
//        }
//
//        /*获取文件信息，把信息放到s_buf中*/
//        stat(file_path,&s_buf);
//
//        /*判断是否目录*/
//        if(S_ISDIR(s_buf.st_mode))
//        {
//            info.m_num_folders++;
//            info.collect_folder(file_path);
//            _get_folder_information(file_path, info);
//        }
//
//        /*判断是否为普通文件*/
//        if(S_ISREG(s_buf.st_mode))
//        {
//            unsigned long file_size = get_file_size(file_path);
//            info.m_totel_size += file_size;
//        }
//    }
//}

void _get_folder_information(char const* folder, xhn::folder_information& info)
{
    AAssetDir* assetDir = AAssetManager_openDir(s_AAssetManager, folder);
    if (assetDir) {
        const char* filename = nullptr;
        do {
            filename = AAssetDir_getNextFileName(assetDir);
            if (filename) {
                info.collect_filename(xhn::string(folder) + "/" + filename);
            }
        } while (filename);
        AAssetDir_close(assetDir);
    }
}
bool AndroidFileManager::get_folder_information(const xhn::wstring& path,
                                                xhn::folder_information& info)
{
    xhn::Utf8 utf8(path.c_str());
    xhn::string strPath = utf8;
    _get_folder_information(strPath.c_str(), info);
    return false;
}
bool AndroidFileManager::get_folder_information(const xhn::string& path,
                                                xhn::folder_information& info)
{
    _get_folder_information(path.c_str(), info);
    return false;
}
void AndroidFileManager::set_access_permission(const xhn::wstring& path,
                                               euint32 accessPermisson)
{
}
bool AndroidFileManager::is_non_sandbox_dir(const xhn::wstring& path)
{
    return true;
}
AndroidFile::~AndroidFile()
{
}

euint64 AndroidFile::read(euint8* buffer, euint64 size)
{
    return 0;
}
euint64 AndroidFile::write(const euint8* buffer, euint64 size)
{
    return 0;
}
euint64 AndroidFile::get_size()
{
    return 0;
}
euint64 AndroidFile::get_pos()
{
    return 0;
}
euint64 AndroidFile::set_pos(euint64 pos)
{
    return 0;
}
void AndroidFile::set_base_offset(euint64 offs)
{
    m_baseOffset = offs;
}

euint8& AndroidFile::operator[] (euint64 pos)
{
    m_needSynchronizeFile = true;
    pos += m_baseOffset;
    xhn::file_block* block = access(this, pos);
    return *((euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}

const euint8& AndroidFile::operator[] (euint64 pos) const
{
    pos += m_baseOffset;
    AndroidFile* file = (AndroidFile*)this;
    const xhn::file_block* block = access((AndroidFile*)file, pos);
    return *((const euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}
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
