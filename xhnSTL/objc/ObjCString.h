/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
/**
#import <Foundation/Foundation.h>

@class ObjCStringArray;
@interface ObjCString : NSObject
{
@private
    void* body;
    NSString* string;
}
- (id)initWithCString:(const char*)str;
- (id)initWithNSString:(NSString*)str;
- (id)initWithString:(NSString*)str;
- (id)initWithUTF8String:(const char*)str;
- (NSString*)string;
- (ObjCString*)add:(const ObjCString*)str;
- (ObjCString*)addCString:(const char*)str;
- (void)addAssign:(const ObjCString*)str;
- (void)addAssignCString:(const char*)str;
- (ObjCStringArray*)splitWithString:(const ObjCString*)str;
- (ObjCStringArray*)splitWithCString:(const char*)str;
- (BOOL)isEqualToNSString:(NSString*)str;
- (BOOL)isEqualToString:(NSString *)str;
@end

@interface ObjCStringArrayIterator : NSObject
{
@private
    void* body;
}
- (ObjCString*)nextString;
@end

@interface ObjCStringArray : NSObject
{
@private
    void* body;
}
- (void*)getBody;
- (ObjCStringArrayIterator*)getIterator;
- (NSInteger)numStrings;
- (void)addString:(ObjCString*)str;
@end
**/
#ifndef OBJC_STRING_H
#define OBJC_STRING_H
#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C"
{
#endif
void set_select_string_encoding_dialog_callback(ShowDialog sd, HideDialog hd);
char* string_convert_to_utf8(const char* input);
char* localized_string(const char* identifier);
/// 通过free来释放
const wchar_t* utf8_to_unicode(const char* input);
const char* convert_utf8_path_to_url(const char* utf8_path);
const char* convert_url_to_utf8_path(const char* url);
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
