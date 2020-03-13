/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ELOG_H
#define ELOG_H
#include "common.h"
#include "etypes.h"
#include "spin_lock.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
#if defined(ANDROID) || defined(__ANDROID__)
extern void (*s_logFunction)(const char*);
void TestLogFunction();
#endif
    
extern XHN_EXPORT void (*GetHomeDirectory)(char* output, int outlen);
extern XHN_EXPORT ELock g_elog_lock;
#define ELOG_BUFFER_SIZE 4096 * 2
XHN_EXPORT void ELog_Init(void);
XHN_EXPORT void ELog_Release(void);
extern XHN_EXPORT char g_elog_buffer[ELOG_BUFFER_SIZE];
XHN_EXPORT void ELog_write(void);
    
#if DEBUG

#define elog(fmt,...) { ELock_lock(&g_elog_lock); \
    snprintf(g_elog_buffer,ELOG_BUFFER_SIZE,fmt,##__VA_ARGS__); \
    ELog_write(); \
    ELock_unlock(&g_elog_lock); }
    
#else
    
#define elog(fmt,...)
    
#endif
    
struct elogger
{
    FILE* logFile;
};
    
XHN_EXPORT void ELog2_Init(struct elogger* logger);
XHN_EXPORT void ELog2_write(struct elogger* logger);
    
#if DEBUG
    
#define elog2(renderSys, fmt,...) { ELock_lock(&g_elog_lock); \
snprintf(g_elog_buffer,ELOG_BUFFER_SIZE,fmt,##__VA_ARGS__); \
ELog2_write(renderSys); \
ELock_unlock(&g_elog_lock); }
    
#else
    
#define elog2(renderSys, fmt,...)
    
#endif
    
#ifdef __cplusplus
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
