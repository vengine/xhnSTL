/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LOGGER_HPP
#define XHN_LOGGER_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_string.hpp"
#include <cstdarg>

namespace xhn {
    
class CLoggerImpl
{
private:
    string m_buffer;
public:
    void put(char c) {
        m_buffer += c;
    }
    void append(const char* str) {
        m_buffer += str;
    }
    void flush(const string& indentation) {
        printf("%s%s\n", indentation.c_str(), m_buffer.c_str());
        m_buffer = "";
    }
};
    
template <typename IMPL>
class logger
{
private:
    string m_indentation;
    IMPL m_impl;
private:
    void print_int(char* mbuf, int len, int i) const
    {
        snprintf(mbuf, len, "%d", i);
    }
    void print_float(char* mbuf, int len, double f) const
    {
        snprintf(mbuf, len, "%f", f);
    }
public:
    void log( const char* format, ...)
    {
        va_list arg;
        
        va_start (arg, format);
        
        while( *format != '\0')
        {
            if( *format == '%')
            {
                if( *(format+1) == 'c' ) {
                    char c = (char)va_arg(arg, int);
                    m_impl.put(c);
                } else if( *(format+1) == 'd' || *(format+1) == 'i') {
                    char mbuf[256];
                    int i = va_arg(arg, int);
                    print_int(mbuf, 256, i);
                    m_impl.append(mbuf);
                } else if( *(format+1) == 'f') {
                    char mbuf[256];
                    double f = va_arg(arg, double);
                    print_float(mbuf, 256, f);
                    m_impl.append(mbuf);
                } else if( *(format+1) == 's' ) {
                    char* str = va_arg(arg, char*);
                    m_impl.append(str);
                }
                
                format += 2;
            } else {
                m_impl.put(*format++);
            }
        }
        
        va_end (arg);
        flush();
    }
    void push() {
        m_indentation += "  ";
    }
    void pop() {
        m_indentation = m_indentation.substr(0, m_indentation.size() - 2);
    }
    void flush() {
        m_impl.flush(m_indentation);
    }
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
