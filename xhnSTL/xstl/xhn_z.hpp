/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_Z_HPP
#define XHN_Z_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_smart_ptr.hpp"
#include "xhn_list.hpp"
#include "xhn_string.hpp"
#include "xhn_static_string.hpp"
#include "xhn_lock.hpp"
#include "xhn_btree.hpp"
#include "xhn_lambda.hpp"
#include "timer.h"

#define MASK_FULL           0xffffffffffffffff

#define MASK_EIGHTH         0xff00000000000000
#define MASK_QUARTER        0xffff000000000000
#define MASK_THREE_EIGHTHS  0xffffff0000000000
#define MASK_HALF           0xffffffff00000000
#define MASK_FIVE_EIGHTHS   0xffffffffff000000
#define MASK_THREE_QUARTERS 0xffffffffffff0000
#define MASK_SEVEN_EIGHTHS  0xffffffffffffff00

namespace xhn
{

enum CommandType
{
    EmptyCommand,
    ByteCommand,
    CompressCommand,
};
    
class ZDictionary;
struct OutputCommand
{
    CommandType type;
    unsigned char byteData;
    unsigned short compressHeader;
    unsigned char compressId;
    unsigned char compressLength;
    ZDictionary* dict;
    void Print();
    void TextPrint();
};
    
class MatchBuffer
{
public:
    ZDictionary* m_dictionary;
    unsigned long m_positionCount;
    unsigned char m_numberOfBytesOnPipeline;
public:
    unsigned short m_header;
    unsigned long long m_pattern;
public:
    MatchBuffer(ZDictionary* dictionary)
    : m_dictionary(dictionary)
    , m_positionCount(0)
    , m_numberOfBytesOnPipeline(0)
    {
    }
    void PrintPattern()
    {
        unsigned char com0 = (unsigned char)(m_pattern >> 56);
        unsigned char com1 = (unsigned char)(m_pattern >> 48);
        unsigned char com2 = (unsigned char)(m_pattern >> 40);
        unsigned char com3 = (unsigned char)(m_pattern >> 32);
        unsigned char com4 = (unsigned char)(m_pattern >> 24);
        unsigned char com5 = (unsigned char)(m_pattern >> 16);
        unsigned char com6 = (unsigned char)(m_pattern >> 8);
        unsigned char com7 = (unsigned char)(m_pattern >> 0);
        printf("PATTERN:%c%c%c%c%c%c%c%c\n", com0, com1, com2, com3, com4, com5, com6, com7);
    }
    OutputCommand PushAByte(unsigned char byte);
};
    
class ZDictionary
{
public:
    template <unsigned MAX_TAGS>
    class Entry
    {
    public:
        class Line
        {
        public:
            unsigned long long tag;
            unsigned long position;
            unsigned char id;
            Line()
            : tag(0)
            {}
            bool Match(unsigned long long pattern, unsigned long matchPosition, unsigned int* matchLength)
            {
                if (matchPosition - position < 11)
                    return false;
                if ((tag & MASK_HALF) == (pattern & MASK_HALF)) {
                    if ((tag & MASK_THREE_QUARTERS) == (pattern & MASK_THREE_QUARTERS)) {
                        if ((tag & MASK_SEVEN_EIGHTHS) == (pattern & MASK_SEVEN_EIGHTHS)) {
                            *matchLength = 7;
                        } else {
                            *matchLength = 6;
                        }
                    } else if ((tag & MASK_FIVE_EIGHTHS) == (pattern & MASK_FIVE_EIGHTHS)) {
                        *matchLength = 5;
                    } else {
                        *matchLength = 4;
                    }
                    return true;
                } else if ((tag & MASK_QUARTER) == (pattern & MASK_QUARTER)) {
                    if ((tag & MASK_THREE_EIGHTHS) == (pattern & MASK_THREE_EIGHTHS)) {
                        *matchLength = 3;
                    } else {
                        *matchLength = 2;
                    }
                    return true;
                } else {
                    return false;
                }
            }
        };
    public:
        Line m_lines[MAX_TAGS];
        unsigned int m_lineCount;
    public:
        Entry()
        : m_lineCount(0)
        {
            for (unsigned char i = 0; i < MAX_TAGS; i++) {
                m_lines[i].id = i;
            }
        }
        /// 提升一个行的位置
        void Promote(unsigned int index)
        {
            Line tmp = m_lines[index];
            m_lines[index] = m_lines[index - 1];
            m_lines[index - 1] = tmp;
        }
        bool FindLine(unsigned long long pattern, unsigned long matchPosition,
                      unsigned int* matchLength, unsigned int* idResult)
        {
            for (unsigned int i = 0; i < m_lineCount; i++) {
                if (m_lines[i].Match(pattern, matchPosition, matchLength)) {
                    *idResult = m_lines[i].id;
                    if (i) {
                        Promote(i);
                    }
                    return true;
                }
            }
            return false;
        }
        bool FindLine(unsigned int id, unsigned long long* patternResult)
        {
            for (unsigned int i = 0; i < m_lineCount; i++) {
                if (m_lines[i].id == id) {
                    *patternResult = m_lines[i].tag;
                    if (i) {
                        Promote(i);
                    }
                    return true;
                }
            }
            return false;
        }
        void PushNewLine(unsigned long long pattern, unsigned long position)
        {
            if (m_lineCount == MAX_TAGS) {
                m_lines[MAX_TAGS - 1].tag = pattern;
                m_lines[MAX_TAGS - 1].position = position;
            } else {
                m_lines[m_lineCount].tag = pattern;
                m_lines[m_lineCount].position = position;
                m_lineCount++;
            }
        }
    };
public:
    Entry<32> m_entries[65536];
public:
    ZDictionary()
    {
    }
    bool FindLine(unsigned short header,
                  unsigned long long pattern,
                  unsigned long matchPosition,
                  unsigned int* matchLength, unsigned int* idResult)
    {
        return m_entries[header].FindLine(pattern, matchPosition, matchLength, idResult);
    }
    bool FindLine(unsigned short header,
                  unsigned int id,
                  unsigned long long* patternResult)
    {
        return m_entries[header].FindLine(id, patternResult);
    }
    unsigned long long GetPattern(unsigned short header,
                                  unsigned int id)
    {
        return m_entries[header].m_lines[id].tag;
    }
    void PushNewLine(unsigned short header,
                     unsigned long long pattern,
                     unsigned long position)
    {
        m_entries[header].PushNewLine(pattern, position);
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
