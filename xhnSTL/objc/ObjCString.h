//
//  ObjCString.h
//  memtest
//
//  Created by 徐 海宁 on 13-12-7.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//
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
#ifdef __cplusplus
}
#endif
#endif