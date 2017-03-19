//
//  symbol_buffer.h
//  xhnSTL
//
//  Created by 徐海宁 on 2017/3/18.
//  Copyright © 2017年 徐 海宁. All rights reserved.
//

#ifndef symbol_buffer_h
#define symbol_buffer_h

#ifdef __cplusplus

#include "utility.h"

namespace xhn
{

class SymbolBuffer
{
public:
    char buffer[1024];
    int bufferTop;
public:
    SymbolBuffer()
    {
        buffer[0] = 0;
        bufferTop = 0;
    }
    
    void AddCharacter(char c)
    {
        buffer[bufferTop] = c;
        bufferTop++;
        buffer[bufferTop] = 0;
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

#endif /* symbol_buffer_h */
