/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef __ecg__apple_file_manager__
#define __ecg__apple_file_manager__

#ifdef __cplusplus

#include "xhn_vector.hpp"
#include "xhn_string.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_file_stream.hpp"
#include "xhn_btree.hpp"
#include "xhn_lock.hpp"
void AppleFileInit();

class FilenameArray
{
public:
    virtual void AddFilename(const char* filename) = 0;
};

enum FileDirectory
{
    AppDirectory,
    UsrDirectory,
};

void GetFilenames(FileDirectory dir, FilenameArray* filenames);

void GetPaths(const char* baseFolder,
              xhn::vector<xhn::string>& subFolders,
              xhn::vector<xhn::string>& paths);

class AppleFile : public xhn::file_stream
{
    friend class AppleFileManager;
private:
    vptr m_fileHandle;
    xhn::wstring m_path;
    euint64 m_baseOffset;
    bool m_needSynchronizeFile;
public:
    AppleFile()
    : m_fileHandle(NULL)
    , m_baseOffset(0)
    , m_needSynchronizeFile(false)
    {}
    ~AppleFile();
    virtual euint64 read(euint8* buffer, euint64 size);
    virtual euint64 write(const euint8* buffer, euint64 size);
    virtual euint64 get_size();
    virtual euint64 get_pos();
    virtual euint64 set_pos(euint64 pos);
    virtual void set_base_offset(euint64 offs);
    virtual euint8& operator[] (euint64 pos);
    virtual const euint8& operator[] (euint64 pos) const;
};

class AppleFileManager : public xhn::file_manager
{
public:
    AppleFileManager();
    ~AppleFileManager();
    virtual bool is_exist(const xhn::wstring& path, bool& is_directory);
    virtual bool is_exist(const xhn::string& path, bool& is_directory);
    virtual bool create_directory(const xhn::wstring& dir);
    virtual bool create_file(const xhn::wstring& path);
    virtual xhn::file_stream_ptr open(const xhn::wstring& path);
    virtual xhn::file_stream_ptr create_and_open(const xhn::wstring& path);
    virtual void delete_file(const xhn::wstring& path);
    virtual euint64 file_size(const xhn::wstring& path);
    virtual euint64 file_size(const xhn::string& path);
    virtual xhn::wstring get_home_dir();
    virtual void get_resource_dirs(xhn::vector<xhn::wstring>& result);
    virtual void get_system_font_dirs(xhn::vector<xhn::wstring>& result);
    virtual bool get_folder_information(const xhn::wstring& path, xhn::folder_information& info);
    virtual bool get_folder_information(const xhn::string& path, xhn::folder_information& info);
    virtual void set_access_permission(const xhn::wstring& path, euint32 accessPermisson);
    virtual bool is_non_sandbox_dir(const xhn::wstring& path);
};

#endif

#endif /* defined(__ecg__apple_file_manager__) */

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