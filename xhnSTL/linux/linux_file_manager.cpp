/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "linux_file_manager.hpp"
#include "xhn_set.hpp"
#include "xhn_string.hpp"
#include "elog.h"
#include <sys/stat.h>
#include <dirent.h>
#include <strings.h>
#include <unistd.h>  

void _GetHomeDirectory(char* output, int outlen)
{
    snprintf(output, outlen, "/");
}

void LinuxFileInit()
{
    GetHomeDirectory = _GetHomeDirectory;
}

void GetFilenames(FileDirectory dir, FilenameArray* filenames)
{
/**
    NSArray *documentPaths = NULL;
    switch (dir)
    {
        case AppDirectory:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSApplicationDirectory, NSUserDomainMask, YES);
            break;
        case UsrDirectory:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSUserDirectory, NSUserDomainMask, YES);
            break;
        default:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSApplicationDirectory, NSUserDomainMask, YES);
            break;
    }
    for(NSString *filename in documentPaths)
    {
        const char *str = [filename cStringUsingEncoding:NSASCIIStringEncoding];
        filenames->AddFilename(str);
    }
    **/
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
/**
    NSFileManager *fileManager = [NSFileManager defaultManager];
    @autoreleasepool {
        NSString *strFolder = [NSString stringWithCString:baseFolder encoding:NSUTF8StringEncoding];
        NSURL *directoryURL = [[NSURL alloc] initWithString:strFolder]; // URL pointing to the directory you want to browse
        NSArray *keys = [NSArray arrayWithObject:NSURLIsDirectoryKey];

        NSDirectoryEnumerator *enumerator = [fileManager
                                             enumeratorAtURL:directoryURL
                                             includingPropertiesForKeys:keys
                                             options:0
                                             errorHandler:^(NSURL *url, NSError *error) {
                                                 // Handle the error.
                                                 // Return YES if the enumeration should continue after the error.
                                                 return YES;
                                             }];

        for (NSURL *url in enumerator) {
            NSError *error;
            NSNumber *isDirectory = nil;
            if (! [url getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:&error]) {
                // handle error
            }
            else if (! [isDirectory boolValue]) {
                // No error and it’s not a directory; do something with the file
                NSString* path = [url path];
                xhn::string strPath = [path UTF8String];
                if (strPath.size() >= strlen(".DS_Store")) {
                    if (strPath.find_first_of(".DS_Store", strPath.size() - strlen(".DS_Store")) == xhn::string::npos)
                        paths.push_back(strPath);
                }
                else {
                    paths.push_back(strPath);
                }
            }
            else if ( [isDirectory boolValue]) {
                //
                NSString* subFolder = [url path];
                xhn::string strSubFolder = [subFolder UTF8String];
                subFolders.push_back(strSubFolder);
            }
        }
    }
    **/
}

LinuxFileManager::LinuxFileManager()
{
}
LinuxFileManager::~LinuxFileManager()
{
}
bool LinuxFileManager::is_exist(const xhn::wstring& dir, bool& is_directory)
{
    xhn::Utf8 utf8Path(dir.c_str());
    return is_exist((xhn::string)utf8Path, is_directory);
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        xhn::Utf8 utf8(dir.c_str());
        NSString *strDir = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        BOOL isDir = NO;
        BOOL ret = [fileManager fileExistsAtPath:strDir isDirectory:&isDir];
        is_directory = (isDir == YES);
        return ret == YES;
    }
    **/
    ///return false;
}
bool LinuxFileManager::is_exist(const xhn::string& path, bool& is_directory)
{
    if (access("test.txt", F_OK)==0) {
        struct stat s_buf;
        stat(path.c_str(),&s_buf);
        if(S_ISDIR(s_buf.st_mode)) {
            is_directory = true;
        }
        else {
            is_directory = false;
        }
        return true;
    }
    
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *strDir = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
        BOOL isDir = NO;
        BOOL ret = [fileManager fileExistsAtPath:strDir isDirectory:&isDir];
        is_directory = (isDir == YES);
        return ret == YES;
    }
    **/
    return false;
}
bool LinuxFileManager::create_directory(const xhn::wstring& dir)
{
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        xhn::Utf8 utf8(dir.c_str());
        NSString *strDir = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        BOOL ret = [fileManager createDirectoryAtPath:strDir
                          withIntermediateDirectories:NO
                                           attributes:nil
                                                error:nil];
        return ret == YES;
    }
    **/
    return false;
}
bool LinuxFileManager::create_file(const xhn::wstring& path)
{
/**
    @autoreleasepool {
        NSString *str = @"";
        xhn::Utf8 utf8(path.c_str());
        NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];

        NSError* error = nil;
        BOOL ret = [str writeToFile:strPath
                         atomically:YES
                           encoding:NSUTF8StringEncoding
                              error:&error];
        return ret == YES;
    }
    **/
    return false;
}
xhn::file_stream_ptr LinuxFileManager::open(const xhn::wstring& path)
{
/**
    xhn::file_stream_ptr ret;
    {
        @autoreleasepool {
            NSFileManager *fileManager = [NSFileManager defaultManager];
            xhn::Utf8 utf8(path.c_str());
            NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
            BOOL isDir = NO;
            BOOL isExist = [fileManager fileExistsAtPath:strPath isDirectory:&isDir];
            if (isExist == YES && isDir == NO) {
                AppleFile* file = VNEW AppleFile;
                file->m_path = path;
                NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:strPath];
#if !__has_feature(objc_arc)
                [fileHandle retain];
                file->m_fileHandle = (vptr)fileHandle;
#else
                file->m_fileHandle = (__bridge_retained vptr)fileHandle;
#endif
                ret = file;
            }
        }
    }
    return ret;
    **/
    return nullptr;
}
xhn::file_stream_ptr LinuxFileManager::create_and_open(const xhn::wstring& path)
{
/**
    create_file(path);
    return open(path);
    **/
    return nullptr;
}
void LinuxFileManager::delete_file(const xhn::wstring& path)
{
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        xhn::Utf8 utf8(path.c_str());
        NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        NSError* error = nil;
        [fileManager removeItemAtPath:strPath error:&error];
    }
    **/
}
euint64 LinuxFileManager::file_size(const xhn::wstring& path)
{
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        xhn::Utf8 utf8(path.c_str());
        NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        NSError* error = nil;
        NSDictionary* attrs = [fileManager attributesOfItemAtPath:strPath error:&error];
        NSNumber *fileSize = [attrs objectForKey:NSFileSize];
        if (fileSize) {
            return [fileSize longLongValue];
        }
        else {
            return 0;
        }
    }
    **/
    return 0;
}
euint64 LinuxFileManager::file_size(const xhn::string& path)
{
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *strPath = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
        NSError* error = nil;
        NSDictionary* attrs = [fileManager attributesOfItemAtPath:strPath error:&error];
        NSNumber *fileSize = [attrs objectForKey:NSFileSize];
        if (fileSize) {
            return [fileSize longLongValue];
        }
        else {
            return 0;
        }
    }
    **/
    return 0;
}
xhn::wstring LinuxFileManager::get_home_dir()
{
/**
    @autoreleasepool {
        NSString* homeDir = NSHomeDirectory();
        const char* str = [homeDir UTF8String];
        xhn::Unicode uniStr(str);
        return uniStr;
    }
    **/
    return L"/home";
}
void LinuxFileManager::get_resource_dirs(xhn::vector<xhn::wstring>& result)
{
/**
    result.clear();
    @autoreleasepool {
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        const char* str = [resourcePath UTF8String];
        xhn::Unicode uniStr(str);
        result.push_back(uniStr);
        NSArray* frameworks = [NSBundle allFrameworks];
        for (NSBundle* framework in frameworks) {
            NSString* resPath = [framework resourcePath];

            if ([resPath containsString:@"VEngine"]) {
                const char* str = [resPath UTF8String];
                xhn::Unicode uniStr(str);
                result.push_back(uniStr);
            }
        }
    }
    **/
}
void LinuxFileManager::get_system_font_dirs(xhn::vector<xhn::wstring>& result)
{
/**
    result.clear();
    NSString* dir = @"/System/Library/Fonts";
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSError* err = nil;
    NSArray* subpaths = [fileManager subpathsOfDirectoryAtPath:dir error:&err];
    for (NSString* subpath in subpaths) {
        NSLog(@"%@", subpath);
    }
    result.push_back(L"/System/Library/Fonts");
    **/
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

void _get_folder_information(char const*path, xhn::folder_information& info)
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
            info.m_num_folders++;
            info.collect_folder(file_path);
            _get_folder_information(file_path, info);
        }
        
        /*判断是否为普通文件*/
        if(S_ISREG(s_buf.st_mode))
        {
            unsigned long file_size = get_file_size(file_path);
            info.m_totel_size += file_size;
        }
    }
}
// void _get_folder_information(const char* folder, xhn::folder_information& info)
// {
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSArray* subpaths = [fileManager subpathsAtPath:folder];
        if (subpaths && [subpaths count]) {
            NSEnumerator* iter = [subpaths objectEnumerator];
            NSString* subpath = [iter nextObject];
            while (subpath && !info.m_cancel_flag) {
                NSRange range = [subpath rangeOfString:@".DS_Store"];
                if (range.location == NSNotFound) {
                    NSString* fullPath = [[NSString alloc] initWithFormat:@"%@/%@", folder, subpath];
                    BOOL isDir = NO;
                    if ([fileManager fileExistsAtPath:fullPath isDirectory:&isDir]) {
                        if (!isDir) {
                            xhn::string filename([fullPath UTF8String]);
                            info.collect_filename(filename);
                            info.m_num_files++;
                            NSError* error = nil;
                            NSDictionary* attrs = [fileManager attributesOfItemAtPath:fullPath error:&error];
                            NSString* fileType = [attrs objectForKey:NSFileType];
                            NSNumber *fileSize = [attrs objectForKey:NSFileSize];
                            if ([fileType isEqualToString:NSFileTypeRegular] && fileSize) {
                                info.m_totel_size += [fileSize longLongValue];
                            }
                        }
                        else {
                            xhn::string folder([fullPath UTF8String]);
                            info.collect_folder(folder);
                            info.m_num_folders++;
                        }
                    }
                }
                subpath = [iter nextObject];
            }
        }
    }
    **/
// }
bool LinuxFileManager::get_folder_information(const xhn::wstring& path, xhn::folder_information& info)
{
    xhn::Utf8 utf8(path.c_str());
    xhn::string strPath = utf8;
    _get_folder_information(strPath.c_str(), info);
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL isDir = NO;
        xhn::Utf8 utf8(path.c_str());
        NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        if ([fileManager fileExistsAtPath:strPath isDirectory:&isDir] && isDir) {
            _get_folder_information(strPath, info);
            return true;
        }
        else {
            return false;
        }
    }
    **/
    return false;
}
bool LinuxFileManager::get_folder_information(const xhn::string& path, xhn::folder_information& info)
{
    _get_folder_information(path.c_str(), info);
/**
    @autoreleasepool {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL isDir = NO;
        NSString *strPath = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
        if ([fileManager fileExistsAtPath:strPath isDirectory:&isDir] && isDir) {
            _get_folder_information(strPath, info);
            return true;
        }
        else {
            return false;
        }
    }
    **/
    return false;
}
void LinuxFileManager::set_access_permission(const xhn::wstring& path, euint32 accessPermisson)
{
/**
    @autoreleasepool {
        NSFileManager* mgr = [NSFileManager defaultManager];
        xhn::Utf8 utf8Path(path.c_str());
        NSString* strPath =
#if __has_feature(objc_arc)
        [[NSString alloc] initWithUTF8String:((xhn::string)utf8Path).c_str()];
#else
        [[[NSString alloc] initWithUTF8String:((xhn::string)utf8Path).c_str()] autorelease];
#endif
        NSError* error = nil;
#if __has_feature(objc_arc)
        NSNumber* numAccessPermisson =
        [[NSNumber alloc] initWithUnsignedInt:accessPermisson];
        NSDictionary* attribs =
        [[NSDictionary alloc] initWithObjectsAndKeys:numAccessPermisson, NSFilePosixPermissions, nil];
#else
        NSNumber* numAccessPermisson =
        [[[NSNumber alloc] initWithUnsignedInt:accessPermisson] autorelease];
        NSDictionary* attribs =
        [[[NSDictionary alloc] initWithObjectsAndKeys:numAccessPermisson, NSFilePosixPermissions, nil] autorelease];
#endif
        [mgr setAttributes:attribs ofItemAtPath:strPath error:&error];
    }
    **/
}
bool LinuxFileManager::is_non_sandbox_dir(const xhn::wstring& path)
{
    return true;
}
LinuxFile::~LinuxFile()
{
/**
    if (m_needSynchronizeFile)
        synchronize_file();
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        [fileHandle closeFile];
#if !__has_feature(objc_arc)
        [fileHandle release];
#else
        CFTypeRef handle = (__bridge_retained CFTypeRef)fileHandle;
        CFRelease(handle);
#endif
    }
    **/
}

euint64 LinuxFile::read(euint8* buffer, euint64 size)
{
/**
    euint64 ret = 0;
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        @autoreleasepool {
            NSData* data = [fileHandle readDataOfLength: size];
            ret = [data length];
            [data getBytes:buffer length:size];
        }
    }
    return ret;
    **/
    return 0;
}
euint64 LinuxFile::write(const euint8* buffer, euint64 size)
{
/**
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        @autoreleasepool {
            NSData* data = [NSData dataWithBytes:buffer length:size];
            [fileHandle writeData:data];
        }
        return size;
    }
    else
        return 0;
        **/
    return 0;
}
euint64 LinuxFile::get_size()
{
/**
    euint64 ret = 0;
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        @autoreleasepool {
            euint64 pos = [fileHandle offsetInFile];
            [fileHandle seekToEndOfFile];
            ret = [fileHandle offsetInFile];
            [fileHandle seekToFileOffset:pos];
        }
    }
    return ret;
    **/
    return 0;
}
euint64 LinuxFile::get_pos()
{
/**
    euint64 ret = 0;
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        ret = [fileHandle offsetInFile];
    }
    return ret;
    **/
    return 0;
}
euint64 LinuxFile::set_pos(euint64 pos)
{
/**
    euint64 ret = 0;
    if (m_fileHandle) {
#if __has_feature(objc_arc)
        NSFileHandle *fileHandle = (__bridge NSFileHandle*)m_fileHandle;
#else
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
#endif
        [fileHandle seekToFileOffset:pos];
        ret = [fileHandle offsetInFile];
    }
    return ret;
    **/
    return 0;
}
void LinuxFile::set_base_offset(euint64 offs)
{
    m_baseOffset = offs;
}

euint8& LinuxFile::operator[] (euint64 pos)
{
    m_needSynchronizeFile = true;
    pos += m_baseOffset;
    xhn::file_block* block = access(this, pos);
    return *((euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}

const euint8& LinuxFile::operator[] (euint64 pos) const
{
    pos += m_baseOffset;
    LinuxFile* file = (LinuxFile*)this;
    const xhn::file_block* block = access((LinuxFile*)file, pos);
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
