/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef symbol_buffer_h
#define symbol_buffer_h

#ifdef __cplusplus

#include "utility.h"

#define SYMBOL_BUFFER_SIZE 1024
namespace xhn
{

class SymbolBuffer
{
public:
    char buffer[SYMBOL_BUFFER_SIZE];
    int bufferTop;
public:
    SymbolBuffer()
    {
        buffer[0] = 0;
        bufferTop = 0;
    }
    
    void AddCharacter(char c)
    {
		if (bufferTop < SYMBOL_BUFFER_SIZE - 1) {
			buffer[bufferTop] = c;
			bufferTop++;
			buffer[bufferTop] = 0;
		}
		else {
			buffer[bufferTop - 3] = '.';
			buffer[bufferTop - 2] = '.';
			buffer[bufferTop - 1] = '.';
		}
    }
    
    bool IsInteger()
    {
        if (bufferTop) {
            int num;
            return to_int(buffer, &num);
        }
        else {
            return false;
        }
    }
    
    bool IsIdentifier()
    {
        if (bufferTop > 0) {
            bool isLetter =
            (buffer[0] >= 'a' && buffer[0] <= 'z') ||
            (buffer[0] >= 'A' && buffer[0] <= 'Z');
            return isLetter;
        }
        else {
            return false;
        }
    }
    
    bool IsFunctionResult()
    {
        if (bufferTop == 2 && buffer[0] == '-' && buffer[1] == '>')
            return true;
        else
            return false;
    }
    
    xhn::static_string GetSymbol()
    {
        return buffer;
    }
    
    xhn::string GetString()
    {
        return buffer;
    }
    
    void Clear()
    {
        bufferTop = 0;
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
