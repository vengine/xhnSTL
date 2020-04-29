/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "win_file_manager.hpp"
#include "xhn_set.hpp"
#include "xhn_string.hpp"
#include "elog.h"
#include <sys/stat.h>
#include <windows.h>
#include <unistd.h>  

#include "xhn_vector.hpp"
#include "xhn_string.hpp"

void _GetHomeDirectory(char* output, int outlen)
{
    snprintf(output, outlen, "/");
}

void WinFileInit()
{
    GetHomeDirectory = _GetHomeDirectory;
}

void GetFilenames(FileDirectory dir, FilenameArray* filenames)
{
}

void CollectFilesInDirectory(LPCSTR szPath, xhn::vector<xhn::string>& subDirs, xhn::vector<xhn::string>& subFiles)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];

	lstrcpyA(szFilePath, szPath);
	lstrcatA(szFilePath, "\\*");

	hListFile = FindFirstFileA(szFilePath, &FindFileData);
	if (hListFile != INVALID_HANDLE_VALUE) {
		do
		{

			if (lstrcmpA(FindFileData.cFileName, ".") == 0 ||
				lstrcmpA(FindFileData.cFileName, "..") == 0) {
				continue;
			}

			CHAR szFullFilePath[MAX_PATH];
			lstrcpyA(szFullFilePath, szPath);
			lstrcatA(szFullFilePath, "\\");
			lstrcatA(szFullFilePath, FindFileData.cFileName);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				subDirs.push_back(szFullFilePath);

				CollectFilesInDirectory(szFullFilePath, subDirs, subFiles);
			}
			else {
				subFiles.push_back(szFullFilePath);
			}

		} while (FindNextFileA(hListFile, &FindFileData));
	}
	FindClose(hListFile);
}


void GetPaths(const char* baseFolder,
              xhn::vector<xhn::string>& subFolders,
              xhn::vector<xhn::string>& paths)
{
	CollectFilesInDirectory(baseFolder, subFolders, paths);
}

WinFileManager::WinFileManager()
{
}
WinFileManager::~WinFileManager()
{
}
bool WinFileManager::is_exist(const xhn::wstring& dir, bool& is_directory)
{
    xhn::Utf8 utf8Path(dir.c_str());
    return is_exist((xhn::string)utf8Path, is_directory);
}
bool WinFileManager::is_exist(const xhn::string& path, bool& is_directory)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];

	lstrcpyA(szFilePath, path.c_str());

	hListFile = FindFirstFileA(szFilePath, &FindFileData);
	if (hListFile != INVALID_HANDLE_VALUE) {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			is_directory = true;
		}
		else {
			is_directory = false;
		}
		return true;
	}
    return false;
}
bool WinFileManager::create_directory(const xhn::wstring& dir)
{
	return CreateDirectoryW(dir.c_str(), nullptr);
}
bool WinFileManager::create_file(const xhn::wstring& path)
{
	HANDLE handle =
		CreateFileW(
			path.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
	if (INVALID_HANDLE_VALUE != handle) {
		CloseHandle(handle);
		return true;
	}
	else {
		return false;
	}
}
xhn::file_stream_ptr WinFileManager::open(const xhn::wstring& path)
{
	WinFile* file = VNEW WinFile;
	file->m_path = path;
	_wfopen_s(&file->m_fileHandle, path.c_str(), L"rb");
	file->m_needSynchronizeFile = true;
    return file;
}
xhn::file_stream_ptr WinFileManager::create_and_open(const xhn::wstring& path)
{
	create_file(path);
	return open(path);
}
void WinFileManager::delete_file(const xhn::wstring& path)
{
	DeleteFileW(path.c_str());
}
euint64 WinFileManager::file_size(const xhn::wstring& path)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttrs;
	if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fileAttrs)) {
		euint64 ret = fileAttrs.nFileSizeHigh;
		ret <<= 32;
		ret |= static_cast<euint64>(fileAttrs.nFileSizeLow);
		return ret;
	}
	else {
		return 0;
	}
}
euint64 WinFileManager::file_size(const xhn::string& path)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttrs;
	if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fileAttrs)) {
		euint64 ret = fileAttrs.nFileSizeHigh;
		ret <<= 32;
		ret |= static_cast<euint64>(fileAttrs.nFileSizeLow);
		return ret;
	}
	else {
		return 0;
	}
}
xhn::wstring WinFileManager::get_home_dir()
{
    return L"/home";
}
void WinFileManager::get_resource_dirs(xhn::vector<xhn::wstring>& result)
{
}
void WinFileManager::get_system_font_dirs(xhn::vector<xhn::wstring>& result)
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

void _get_folder_information(char const*path, xhn::folder_information& info)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];

	lstrcpyA(szFilePath, path);
	lstrcatA(szFilePath, "\\*");


	hListFile = FindFirstFileA(szFilePath, &FindFileData);
	if (hListFile != INVALID_HANDLE_VALUE) {
		do
		{

			if (lstrcmpA(FindFileData.cFileName, ".") == 0 ||
				lstrcmpA(FindFileData.cFileName, "..") == 0) {
				continue;
			}

			CHAR szFullFilePath[MAX_PATH];
			lstrcpyA(szFullFilePath, path);
			lstrcatA(szFullFilePath, "\\");
			lstrcatA(szFullFilePath, FindFileData.cFileName);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				info.collect_folder(szFullFilePath);
				info.m_num_folders++;
				_get_folder_information(szFullFilePath, info);
			}
			else {
				info.collect_filename(szFullFilePath);
				info.m_num_files++;
			}

		} while (FindNextFileA(hListFile, &FindFileData) && !info.m_cancel_flag);
	}
	FindClose(hListFile);
}

bool WinFileManager::get_folder_information(const xhn::wstring& path, xhn::folder_information& info)
{
    xhn::Utf8 utf8(path.c_str());
    xhn::string strPath = utf8;
    _get_folder_information(strPath.c_str(), info);

    return true;
}
bool WinFileManager::get_folder_information(const xhn::string& path, xhn::folder_information& info)
{
    _get_folder_information(path.c_str(), info);
    return true;
}
void WinFileManager::set_access_permission(const xhn::wstring& path, euint32 accessPermisson)
{
}
bool WinFileManager::is_non_sandbox_dir(const xhn::wstring& path)
{
    return true;
}
WinFile::~WinFile()
{
	if (m_fileHandle) {
		fclose(m_fileHandle);
	}
}

euint64 WinFile::read(euint8* buffer, euint64 size)
{
	return fread(buffer, size, 1, m_fileHandle);
}
euint64 WinFile::write(const euint8* buffer, euint64 size)
{
	return fwrite(buffer, size, 1, m_fileHandle);
}
euint64 WinFile::get_size()
{
	euint64 pos = ftell(m_fileHandle);
	fseek(m_fileHandle, 0, SEEK_END);
	euint64 size = ftell(m_fileHandle);
	fseek(m_fileHandle, pos, SEEK_SET);
	return size;
}
euint64 WinFile::get_pos()
{
    return ftell(m_fileHandle);
}
euint64 WinFile::set_pos(euint64 pos)
{
	fseek(m_fileHandle, pos, SEEK_SET);
    return ftell(m_fileHandle);
}
void WinFile::set_base_offset(euint64 offs)
{
    m_baseOffset = offs;
}

euint8& WinFile::operator[] (euint64 pos)
{
    m_needSynchronizeFile = true;
    pos += m_baseOffset;
    xhn::file_block* block = access(this, pos);
    return *((euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}

const euint8& WinFile::operator[] (euint64 pos) const
{
    pos += m_baseOffset;
	WinFile* file = (WinFile*)this;
    const xhn::file_block* block = access((WinFile*)file, pos);
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
