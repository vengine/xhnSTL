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
    
#define PATTERN_SIZE 16
#define PIPELINE_LENGTH (PATTERN_SIZE + 2)
    
class MatchBuffer
{
public:
    ZDictionary* m_dictionary;
    unsigned long m_positionCount;
    unsigned char m_numberOfBytesOnPipeline;
public:
    unsigned short m_header;
    unsigned char* m_pattern;
public:
    MatchBuffer(unsigned char* input,
                ZDictionary* dictionary)
    : m_pattern(input)
    , m_dictionary(dictionary)
    , m_positionCount(0)
    , m_numberOfBytesOnPipeline(PATTERN_SIZE)
    {
    }
    OutputCommand PushAByte();
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
            unsigned short tagHeader;
            unsigned char* tag;
            unsigned long position;
            unsigned char id;
            Line()
            : tagHeader(0)
            , tag(nullptr)
            , position(0)
            , id(0)
            {}
            bool Match(unsigned short header,
                       unsigned char* pattern,
                       unsigned long matchPosition, unsigned int* matchLength)
            {
                if (tagHeader != header)
                    return false;
                if (matchPosition - position < PIPELINE_LENGTH + 1)
                    return false;
                unsigned char matchCount = 0;
                for (int i = 0; i < PATTERN_SIZE; i++) {
                    if (pattern[i] != tag[i])
                        break;
                    matchCount++;
                }
                if (matchCount > 1) {
                    *matchLength = matchCount;
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
        bool FindLine(unsigned short header,
                      unsigned char* pattern,
                      unsigned long matchPosition,
                      unsigned int* matchLength, unsigned int* idResult)
        {
            unsigned int mostLengthIndex = 0;
            unsigned int mostLength = 0;
            unsigned int mostLengthId = 0;
            unsigned int tmpLength = 0;
            for (unsigned int i = 0; i < m_lineCount; i++) {
                if (m_lines[i].Match(header, pattern, matchPosition, &tmpLength)) {
                    if (tmpLength > mostLength) {
                        mostLengthIndex = i;
                        mostLength = tmpLength;
                        mostLengthId = m_lines[i].id;
                    }
                }
            }
            if (mostLength) {
                *matchLength = mostLength;
                *idResult = mostLengthId;
                if (mostLengthIndex) {
                    Promote(mostLengthIndex);
                }
                return true;
            }
            return false;
        }
        bool FindLine(unsigned int id, unsigned char** patternResult)
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
        void PushNewLine(unsigned short header,
                         unsigned char* pattern,
                         unsigned long position)
        {
            if (m_lineCount == MAX_TAGS) {
                m_lines[MAX_TAGS - 1].tagHeader = header;
                m_lines[MAX_TAGS - 1].tag = pattern;
                m_lines[MAX_TAGS - 1].position = position;
            } else {
                m_lines[m_lineCount].tagHeader = header;
                m_lines[m_lineCount].tag = pattern;
                m_lines[m_lineCount].position = position;
                m_lineCount++;
            }
        }
    };
public:
    /// 0~511 的mask是1ff 共9个bit，id 用 3bit 0~7 长度 4bit 2~16
    Entry<8> m_entries[512];
public:
    ZDictionary()
    {}
    inline unsigned short HeaderHash(unsigned short header)
    {
        return ((header >> 8) ^ (header << 4)) & 0x1ff;
    }
    bool FindLine(unsigned short header,
                  unsigned char* pattern,
                  unsigned long matchPosition,
                  unsigned int* matchLength, unsigned int* idResult)
    {
        return m_entries[ HeaderHash(header) ].FindLine(header, pattern, matchPosition, matchLength, idResult);
    }
    bool FindLine(unsigned short header,
                  unsigned int id,
                  unsigned char** patternResult)
    {
        return m_entries[ HeaderHash(header) ].FindLine(id, patternResult);
    }
    void PushNewLine(unsigned short header,
                     unsigned char* pattern,
                     unsigned long position)
    {
        m_entries[ HeaderHash(header) ].PushNewLine(header, pattern, position);
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
