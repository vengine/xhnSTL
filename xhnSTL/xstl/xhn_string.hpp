/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_STRING_HPP
#define XHN_STRING_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "xhn_utility.hpp"
#include <string.h>
#include "xhn_string_base.hpp"

namespace xhn
{
void itowa(wchar_t* wbuf, euint i);
    
#if USING_STRING_OLD
    
typedef string_base_old<char, 1, FStrLenProc, FStrCmpProc, FDefaultStrProc, FCharAllocator<char>> string;
typedef string_base_old<wchar_t, 1, FWStrLenProc, FWStrCmpProc, FDefaultWStrProc, FCharAllocator<wchar_t>> wstring;
    
#else

typedef string_base<char, 1, FStrLenProc, FStrCmpProc, FDefaultStrProc, FCharAllocator<char>> string;
typedef string_base<wchar_t, 1, FWStrLenProc, FWStrCmpProc, FDefaultWStrProc, FCharAllocator<wchar_t>> wstring;
    
#endif
    
string to_url(const string& utf8_path);
string to_utf8_path(const string& url);

class Utf8
{
private:
	string m_utf8;
private:
	bool utf8fromwcs(const wchar_t* wcs, vector< char, FGetCharRealSizeProc<char> >& outbuf);
public:
	Utf8(const wchar_t* wsz);
    Utf8(const string& s, const char* filename, int line)
    : m_utf8(s)
    {}
    Utf8(const Utf8& s, const char* filename, int line)
    : m_utf8(s.m_utf8)
    {}
public:
	inline operator const string&() const { return m_utf8; }
};

class Unicode
{
private:
	wstring m_unicode;
private:
	bool utf8towcs(const char *utf8, vector< wchar_t, FGetCharRealSizeProc<wchar_t> >& outbuf);
public:
	Unicode(const char* sz);
    Unicode(const string sz);
    Unicode(euint number);
    Unicode(const wstring s)
    : m_unicode(s)
    {}
    Unicode(const Unicode& s)
    : m_unicode(s)
    {}
public:
	inline operator const wstring&() const { return m_unicode; }
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
