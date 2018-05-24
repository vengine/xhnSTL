/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_STACKTRACE_HPP
#define XHN_STACKTRACE_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"
#include "xhn_string.hpp"

#if defined (_WIN32) || defined (_WIN64)
#elif defined (ANDROID) || defined (__ANDROID__)
#else

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

namespace xhn
{
    
string to_function_call(const char* str);

inline void get_stacktrace(vector<string>& result, int max_frames = 127)
{
    result.clear();
    
    void* addrs_buf[ max_frames + 1 ];
    int num_addrs = backtrace(addrs_buf, int(sizeof(addrs_buf) / sizeof(void*)));
    
    if (0 == num_addrs) {
        return;
    }
    
    char** symbol_list = backtrace_symbols(addrs_buf, num_addrs);
    
    for (int i = 1; i < num_addrs; i++)
    {
        result.push_back(to_function_call(symbol_list[i]));
    }
}
    
inline void get_stacktrace(string& result, int max_frames = 127)
{
    result.clear();
    
    void* addrs_buf[ max_frames + 1 ];
    int num_addrs = backtrace(addrs_buf, int(sizeof(addrs_buf) / sizeof(void*)));
    
    if (0 == num_addrs) {
        return;
    }
    
    char** symbol_list = backtrace_symbols(addrs_buf, num_addrs);
    
    for (int i = 1; i < num_addrs; i++)
    {
        result += to_function_call(symbol_list[i]);
        result += ";";
    }
}

void test_stacktrace();
    
}
#endif

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
