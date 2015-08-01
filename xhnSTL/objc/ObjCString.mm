//
//  ObjCString.m
//  memtest
//
//  Created by 徐 海宁 on 13-12-7.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#import "ObjCString.h"
#include "xhn_memory.hpp"
#include "xhn_string.hpp"
#include <string>
#include <vector>
#import "ObjCPropertys.h"
/**
class ObjCStringBody : public MemObject
{
public:
    std::string m_strBase;
};
class ObjCStringArrayIteratorBody : public MemObject
{
public:
    std::vector<ObjCString*>::iterator m_iter;
    std::vector<ObjCString*>::iterator m_end;
};
class ObjCStringArrayBody : public MemObject
{
public:
    std::vector<ObjCString*> m_arrayBase;
    ~ObjCStringArrayBody() {
#if !__has_feature(objc_arc)
        std::vector<ObjCString*>::iterator iter = m_arrayBase.begin();
        std::vector<ObjCString*>::iterator end = m_arrayBase.end();
        for (; iter != end; iter++) {
            ObjCString* str = *iter;
            [str release];
        }
#endif
        m_arrayBase.clear();
    }
};

@implementation ObjCString
- (id)init
{
    return [self initWithCString:""];
}
- (id)initWithCString:(const char *)str
{
    self = [super init];
    if (self) {
        ObjCStringBody* strBody = VNEW ObjCStringBody;
        strBody->m_strBase = str;
        string = [[NSString alloc] initWithUTF8String:strBody->m_strBase.c_str()];
        body = strBody;
    }
    return self;
}
- (id)initWithNSString:(NSString*)str
{
    self = [super init];
    if (self) {
        ObjCStringBody* strBody = VNEW ObjCStringBody;
        strBody->m_strBase = [str UTF8String];
        string = [[NSString alloc] initWithUTF8String:strBody->m_strBase.c_str()];
        body = strBody;
    }
    return self;
}
- (id)initWithString:(NSString*)str
{
    return [self initWithNSString:str];
}
- (id)initWithUTF8String:(const char*)str
{
    return [self initWithCString:str];
}
- (id)initWhthCPPString:(std::string&)str
{
    self = [super init];
    if (self) {
        ObjCStringBody* strBody = VNEW ObjCStringBody;
        strBody->m_strBase = str;
        string = [[NSString alloc] initWithUTF8String:strBody->m_strBase.c_str()];
        body = strBody;
    }
    return self;
}
- (void)dealloc
{
    if (body) {
        ObjCStringBody* strBody = (ObjCStringBody*)body;
        delete strBody;
        body = NULL;
#if !__has_feature(objc_arc)
        [string release];
#endif
        string = nil;
    }
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}
- (NSString*)string
{
    return string;
}
- (ObjCString*)add:(const ObjCString*)str
{
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    ObjCStringBody* otherBody = (ObjCStringBody*)str->body;
    std::string tmp = selfBody->m_strBase + otherBody->m_strBase;
    return [[ObjCString alloc] initWhthCPPString:tmp];
}
- (ObjCString*)addCString:(const char*)str
{
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    std::string tmp = selfBody->m_strBase + str;
    return [[ObjCString alloc] initWhthCPPString:tmp];
}
- (void)addAssign:(const ObjCString*)str
{
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    const ObjCStringBody* otherBody = (const ObjCStringBody*)str->body;
    selfBody->m_strBase += otherBody->m_strBase;
}
- (void)addAssignCString:(const char*)str
{
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    selfBody->m_strBase += str;
}
- (ObjCStringArray*)splitWithString:(const ObjCString*)str
{
    ObjCStringArray* ret = [[ObjCStringArray alloc] init];
    ObjCStringArrayBody* array = (ObjCStringArrayBody*)[ret getBody];
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    const ObjCStringBody* otherBody = (const ObjCStringBody*)str->body;
    size_t prevPos = 0;
    size_t pos = selfBody->m_strBase.find_first_of(otherBody->m_strBase);
    while (pos != std::string::npos) {
        std::string tmp = selfBody->m_strBase.substr(prevPos, pos - prevPos);
        ObjCString* objcStr = [[ObjCString alloc] initWhthCPPString:tmp];
        array->m_arrayBase.push_back(objcStr);
        prevPos = pos + otherBody->m_strBase.size();
        pos = selfBody->m_strBase.find(otherBody->m_strBase, prevPos);
    }
    if (prevPos != std::string::npos && prevPos < selfBody->m_strBase.size()) {
        std::string tmp = selfBody->m_strBase.substr(prevPos, selfBody->m_strBase.size() - prevPos);
        ObjCString* objcStr = [[ObjCString alloc] initWhthCPPString:tmp];
        array->m_arrayBase.push_back(objcStr);
    }
    return ret;
}
- (ObjCStringArray*)splitWithCString:(const char*)str
{
    ObjCStringArray* ret = [[ObjCStringArray alloc] init];
    ObjCStringArrayBody* array = (ObjCStringArrayBody*)[ret getBody];
    ObjCStringBody* selfBody = (ObjCStringBody*)body;
    std::string otherStr = str;
    size_t prevPos = 0;
    size_t pos = selfBody->m_strBase.find_first_of(otherStr);
    while (pos != std::string::npos) {
        std::string tmp = selfBody->m_strBase.substr(prevPos, pos - prevPos);
        ObjCString* objcStr = [[ObjCString alloc] initWhthCPPString:tmp];
        array->m_arrayBase.push_back(objcStr);
        prevPos = pos + otherStr.size();
        pos = selfBody->m_strBase.find(otherStr, prevPos);
    }
    if (prevPos != std::string::npos && prevPos < selfBody->m_strBase.size()) {
        std::string tmp = selfBody->m_strBase.substr(prevPos, selfBody->m_strBase.size() - prevPos);
        ObjCString* objcStr = [[ObjCString alloc] initWhthCPPString:tmp];
        array->m_arrayBase.push_back(objcStr);
    }
    return ret;
}
- (BOOL)isEqualToNSString:(NSString*)str
{
    return [string isEqualToString:str];
}
- (BOOL)isEqualToString:(NSString *)str
{
    return [string isEqualToString:str];
}
@end

@implementation ObjCStringArrayIterator
- (id)initWithArray:(std::vector<ObjCString*>&)array
{
    self = [super init];
    if (self) {
        ObjCStringArrayIteratorBody* strArrIterBody = VNEW ObjCStringArrayIteratorBody;
        strArrIterBody->m_iter = array.begin();
        strArrIterBody->m_end = array.end();
        body = strArrIterBody;
    }
    return self;
}
- (void)dealloc
{
    ObjCStringArrayIteratorBody* strArrIterBody = (ObjCStringArrayIteratorBody*)body;
    delete strArrIterBody;
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}
- (ObjCString*)nextString
{
    ObjCString* ret = nil;
    ObjCStringArrayIteratorBody* strArrIterBody = (ObjCStringArrayIteratorBody*)body;
    if (strArrIterBody->m_iter != strArrIterBody->m_end) {
        ret = *strArrIterBody->m_iter;
        strArrIterBody->m_iter++;
    }
    return ret;
}
@end

@implementation ObjCStringArray
- (id)init
{
    self = [super init];
    if (self) {
        ObjCStringArrayBody* strArrBody = VNEW ObjCStringArrayBody;
        body = strArrBody;
    }
    return self;
}
- (void)dealloc
{
    ObjCStringArrayBody* strArrBody = (ObjCStringArrayBody*)body;
    delete strArrBody;
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}
- (void*)getBody
{
    return body;
}
- (ObjCStringArrayIterator*)getIterator
{
    ObjCStringArrayBody* selfBody = (ObjCStringArrayBody*)body;
    ObjCStringArrayIterator* ret = [[ObjCStringArrayIterator alloc] initWithArray:selfBody->m_arrayBase];
    return ret;
}
- (NSInteger)numStrings
{
    ObjCStringArrayBody* selfBody = (ObjCStringArrayBody*)body;
    return selfBody->m_arrayBase.size();
}
- (void)addString:(ObjCString*)str
{
    ObjCStringArrayBody* selfBody = (ObjCStringArrayBody*)body;
    selfBody->m_arrayBase.push_back(str);
}
@end
**/
#if 0
static const NSStringEncoding AllowedEncodings[] = {
    /* Western */
    NSISOLatin1StringEncoding, // ISO Latin 1
    (NSStringEncoding) 0x80000203, // ISO Latin 3
    (NSStringEncoding) 0x8000020F, // ISO Latin 9
    NSMacOSRomanStringEncoding, // Mac
    NSWindowsCP1252StringEncoding, // Windows
    /* Baltic */
    (NSStringEncoding) -1,
    (NSStringEncoding) 0x8000020D, // ISO Latin 7
    (NSStringEncoding) 0x80000507, // Windows
    /* Central European */
    (NSStringEncoding) -1,
    NSISOLatin2StringEncoding, // ISO Latin 2
    (NSStringEncoding) 0x80000204, // ISO Latin 4
    (NSStringEncoding) 0x8000001D, // Mac
    NSWindowsCP1250StringEncoding, // Windows
    /* Cyrillic */
    (NSStringEncoding) -1,
    (NSStringEncoding) 0x80000A02, // KOI8-R
    (NSStringEncoding) 0x80000205, // ISO Latin 5
    (NSStringEncoding) 0x80000007, // Mac
    NSWindowsCP1251StringEncoding, // Windows
    /* Japanese */
    (NSStringEncoding) -1, // Divider
    (NSStringEncoding) 0x80000A01, // ShiftJIS
    NSISO2022JPStringEncoding, // ISO-2022-JP
    NSJapaneseEUCStringEncoding, // EUC
    (NSStringEncoding) 0x80000001, // Mac
    NSShiftJISStringEncoding, // Windows
    /* Simplified Chinese */
    (NSStringEncoding) -1, // Divider
    (NSStringEncoding) 0x80000632, // GB 18030
    (NSStringEncoding) 0x80000631, // GBK
    (NSStringEncoding) 0x80000930, // EUC
    (NSStringEncoding) 0x80000019, // Mac
    (NSStringEncoding) 0x80000421, // Windows
    /* Traditional Chinese */
    (NSStringEncoding) -1, // Divider
    (NSStringEncoding) 0x80000A03, // Big5
    (NSStringEncoding) 0x80000A06, // Big5 HKSCS
    (NSStringEncoding) 0x80000931, // EUC
    (NSStringEncoding) 0x80000002, // Mac
    (NSStringEncoding) 0x80000423, // Windows
    /* Korean */
    (NSStringEncoding) -1, // Divider
    (NSStringEncoding) 0x80000940, // EUC
    (NSStringEncoding) 0x80000003, // Mac
    (NSStringEncoding) 0x80000422, // Windows
    /* Hebrew */
    (NSStringEncoding) -1, // Divider
    (NSStringEncoding) 0x80000208, // ISO-8859-8
    (NSStringEncoding) 0x80000005, // Mac
    (NSStringEncoding) 0x80000505, // Windows
    /* End */ 0 
};
#endif
static ShowDialog s_show_select_string_encoding_dialog = nullptr;
static HideDialog s_hide_select_string_encoding_dialog = nullptr;
void set_select_string_encoding_dialog_callback(ShowDialog sd, HideDialog hd)
{
    s_show_select_string_encoding_dialog = sd;
    s_hide_select_string_encoding_dialog = hd;
}

char* string_convert_to_utf8(const char* input)
{
    @autoreleasepool {
        NSString* str = nil;
    select_string_encoding:
        if (s_show_select_string_encoding_dialog) {
            ObjCPropertys* props =[ObjCPropertys instance];
            if (props.curtStringEncoding == 0x00) {
                if (props.sampleString) {
                    free(props.sampleString);
                }
                props.sampleString = (char*)malloc(strlen(input));
                memcpy(props.sampleString, input, strlen(input) + 1);
                s_show_select_string_encoding_dialog();
            }
            NSStringEncoding se = props.curtStringEncoding;
            str = [[NSString alloc] initWithCString:input encoding:se];
        }
        else {
            str = [[NSString alloc] initWithCString:input encoding:0x80000632];
        }
        if (nil == str) {
            ObjCPropertys* props =[ObjCPropertys instance];
            props.curtStringEncoding = 0x00;
            goto select_string_encoding;
        }
        const char* utf8 = [str UTF8String];
        char* output = (char*)malloc(strlen(utf8) + 1);
        memcpy(output, utf8, strlen(utf8));
        output[strlen(utf8)] = 0;
        return output;
    }
}

char* localized_string(const char* identifier)
{
    NSString* str = NSLocalizedString([[NSString alloc] initWithUTF8String:identifier], nil);
    const char* utf8 = [str UTF8String];
    char* output = (char*)malloc(strlen(utf8) + 1);
    memcpy(output, utf8, strlen(utf8));
    output[strlen(utf8)] = 0;
    return output;
}

const wchar_t* utf8_to_unicode(const char* input)
{
    xhn::Unicode uni(input);
    wchar_t* ret = (wchar_t*)malloc((((xhn::wstring)uni).size() + 1) * sizeof(wchar_t));
    memcpy(ret, ((xhn::wstring)uni).c_str(), (((xhn::wstring)uni).size() + 1) * sizeof(wchar_t));
    return ret;
}