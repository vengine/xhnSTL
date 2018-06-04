/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#include "elog.h"
#include "string.h"
#include <unistd.h>

#if defined(ANDROID) || defined(__ANDROID__)
void (*s_logFunction)(const char*) = NULL;
void TestLogFunction()
{
    s_logFunction("this is xhnSTL");
}
#endif

void (*GetHomeDirectory)(char* output, int outlen) = NULL;
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
ELock g_elog_lock = 0;
#else
ELock g_elog_lock = OS_UNFAIR_LOCK_INIT;
#endif
#else
ELock g_elog_lock = 0;
#endif
char g_elog_buffer[ELOG_BUFFER_SIZE];
#ifdef USE_LOG_SYSTEM
LOCKED_STATIC FILE* g_elog_file = NULL;
#endif
void ELog_Init()
{
#if DEBUG
    ELock_lock(&g_elog_lock);
#ifdef USE_LOG_SYSTEM
    if (!g_elog_file) {
#if defined(_WIN32) || defined(_WIN64)
        g_elog_file = SafeFOpen("log.txt", "w+");
#elif defined(__APPLE__)
        char homeDir[256];
        GetHomeDirectory(homeDir, 255);
        char logFilename[256];
        snprintf(logFilename, 256, "%s/Documents/log.txt", homeDir);
        g_elog_file = SafeFOpen(logFilename, "w+");
#endif
    }
#endif
    
    ELock_unlock(&g_elog_lock);
    
#endif

}

void ELog_write()
{
#if defined(ANDROID) || defined(__ANDROID__)
    s_logFunction(g_elog_buffer);
#else
#if DEBUG
#ifdef USE_LOG_SYSTEM
    if (g_elog_file) {
        snprintf(g_elog_buffer, ELOG_BUFFER_SIZE, "%s\n", g_elog_buffer);
        fwrite(g_elog_buffer, strlen(g_elog_buffer), 1, g_elog_file);
        fflush(g_elog_file);
    }
#else
    printf("%s\n", g_elog_buffer);
#endif
#endif
#endif
}

void ELog2_Init(struct elogger* logger)
{
#if DEBUG
    ELock_lock(&g_elog_lock);
#if defined(_WIN32) || defined(_WIN64)
    logger->logFile = SafeFOpen("log.txt", "w+");
#elif defined(__APPLE__)
    char homeDir[256];
    GetHomeDirectory(homeDir, 255);
    char logFilename[256];
    int logCount = 0;
    snprintf(logFilename, 256, "%s/Documents/log%d.txt", homeDir, logCount);
    while (access(logFilename, 0) == 0) {
        logCount++;
        snprintf(logFilename, 256, "%s/Documents/log%d.txt", homeDir, logCount);
    }
    logger->logFile = SafeFOpen(logFilename, "w+");
#endif
    ELock_unlock(&g_elog_lock);
#endif
}

void ELog2_write(struct elogger* logger)
{
#if defined(ANDROID) || defined(__ANDROID__)
    s_logFunction(g_elog_buffer);
#else
#if DEBUG
    snprintf(g_elog_buffer, ELOG_BUFFER_SIZE, "%s\n", g_elog_buffer);
    fwrite(g_elog_buffer, strlen(g_elog_buffer), 1, logger->logFile);
    fflush(logger->logFile);
#endif
#endif
}

void ELog_Release()
{
#if DEBUG
#ifdef USE_LOG_SYSTEM
    if (g_elog_file) {
        fclose(g_elog_file);
        g_elog_file = NULL;
    }
#endif
#endif
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
