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
    unsigned long long m_pattern0;
    unsigned long long m_pattern1;
public:
    MatchBuffer(ZDictionary* dictionary)
    : m_dictionary(dictionary)
    , m_positionCount(0)
    , m_numberOfBytesOnPipeline(0)
    {
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
            unsigned long long tag0;
            unsigned long long tag1;
            unsigned long position;
            unsigned char id;
            Line()
            : tag0(0)
            , tag1(0)
            {}
            bool Match(unsigned long long pattern0, unsigned long long pattern1, unsigned long matchPosition, unsigned int* matchLength)
            {
                if (matchPosition - position < 11)
                    return false;
                if ((tag0 & MASK_FULL) == (pattern0 & MASK_FULL)) {
                    if ((tag1 & MASK_HALF) == (pattern1 & MASK_HALF)) {
                        if ((tag1 & MASK_FULL) == (pattern1 & MASK_FULL)) {
                            *matchLength = 16;
                        } else if ((tag1 & MASK_THREE_QUARTERS) == (pattern1 & MASK_THREE_QUARTERS)) {
                            if ((tag1 & MASK_SEVEN_EIGHTHS) == (pattern1 & MASK_SEVEN_EIGHTHS)) {
                                *matchLength = 15;
                            } else {
                                *matchLength = 14;
                            }
                        } else if ((tag1 & MASK_FIVE_EIGHTHS) == (pattern1 & MASK_FIVE_EIGHTHS)) {
                            *matchLength = 13;
                        } else {
                            *matchLength = 12;
                        }
                        return true;
                    } else if ((tag1 & MASK_QUARTER) == (pattern1 & MASK_QUARTER)) {
                        if ((tag1 & MASK_THREE_EIGHTHS) == (pattern1 & MASK_THREE_EIGHTHS)) {
                            *matchLength = 11;
                        } else {
                            *matchLength = 10;
                        }
                    } else if ((tag1 & MASK_EIGHTH) == (pattern1 & MASK_EIGHTH)) {
                        *matchLength = 9;
                    } else {
                        *matchLength = 8;
                    }
                    return true;
                } else if ((tag0 & MASK_HALF) == (pattern0 & MASK_HALF)) {
                    if ((tag0 & MASK_THREE_QUARTERS) == (pattern0 & MASK_THREE_QUARTERS)) {
                        if ((tag0 & MASK_SEVEN_EIGHTHS) == (pattern0 & MASK_SEVEN_EIGHTHS)) {
                            *matchLength = 7;
                        } else {
                            *matchLength = 6;
                        }
                    } else if ((tag0 & MASK_FIVE_EIGHTHS) == (pattern0 & MASK_FIVE_EIGHTHS)) {
                        *matchLength = 5;
                    } else {
                        *matchLength = 4;
                    }
                    return true;
                } else if ((tag0 & MASK_QUARTER) == (pattern0 & MASK_QUARTER)) {
                    if ((tag0 & MASK_THREE_EIGHTHS) == (pattern0 & MASK_THREE_EIGHTHS)) {
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
        bool FindLine(unsigned long long pattern0, unsigned long long pattern1,
                      unsigned long matchPosition,
                      unsigned int* matchLength, unsigned int* idResult)
        {
            for (unsigned int i = 0; i < m_lineCount; i++) {
                if (m_lines[i].Match(pattern0, pattern1, matchPosition, matchLength)) {
                    *idResult = m_lines[i].id;
                    if (i) {
                        Promote(i);
                    }
                    return true;
                }
            }
            return false;
        }
        bool FindLine(unsigned int id, unsigned long long* patternResult0, unsigned long long* patternResult1)
        {
            for (unsigned int i = 0; i < m_lineCount; i++) {
                if (m_lines[i].id == id) {
                    *patternResult0 = m_lines[i].tag0;
                    *patternResult1 = m_lines[i].tag1;
                    if (i) {
                        Promote(i);
                    }
                    return true;
                }
            }
            return false;
        }
        void PushNewLine(unsigned long long pattern0, unsigned long long pattern1, unsigned long position)
        {
            if (m_lineCount == MAX_TAGS) {
                m_lines[MAX_TAGS - 1].tag0 = pattern0;
                m_lines[MAX_TAGS - 1].tag1 = pattern1;
                m_lines[MAX_TAGS - 1].position = position;
            } else {
                m_lines[m_lineCount].tag0 = pattern0;
                m_lines[m_lineCount].tag1 = pattern1;
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
                  unsigned long long pattern0,
                  unsigned long long pattern1,
                  unsigned long matchPosition,
                  unsigned int* matchLength, unsigned int* idResult)
    {
        return m_entries[header].FindLine(pattern0, pattern1, matchPosition, matchLength, idResult);
    }
    bool FindLine(unsigned short header,
                  unsigned int id,
                  unsigned long long* patternResult0,
                  unsigned long long* patternResult1)
    {
        return m_entries[header].FindLine(id, patternResult0, patternResult1);
    }
    void PushNewLine(unsigned short header,
                     unsigned long long pattern0,
                     unsigned long long pattern1,
                     unsigned long position)
    {
        m_entries[header].PushNewLine(pattern0, pattern1, position);
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
