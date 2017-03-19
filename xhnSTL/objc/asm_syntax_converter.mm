//
//  asm_syntax_converter.cpp
//  xhnSTL
//
//  Created by 徐海宁 on 2017/3/19.
//  Copyright © 2017年 徐 海宁. All rights reserved.
//

#include "asm_syntax_converter.hpp"
#import <Foundation/Foundation.h>

static xhn::SpinLock s_ASMCommandLineUtilsLock;
static NSMutableSet* s_ASMCommandLineUtils = nil;

@interface LLCCommandLineUtil : NSObject
- (void) runCommand:(NSString*)commandToRun callback:(void(^)())proc;
@end

@implementation LLCCommandLineUtil
{
    void(^mCallback)();
    NSTask *mTask;
    NSPipe *mPipe;
    NSFileHandle *mFile;
}

- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];
        
        const char* string = (const char*)data.bytes;
        
        printf("%s", string);
    }
    else {

        mCallback();
    }
}

- (void) runCommand:(NSString*)commandToRun callback:(void(^)())proc
{
    mCallback = proc;
    mTask = [[NSTask alloc] init];
    [mTask setLaunchPath: @"/bin/sh"];
    
    NSArray *arguments = [NSArray arrayWithObjects:
                          @"-c" ,
                          [NSString stringWithFormat:@"%@", commandToRun],
                          nil];
    
    NSLog(@"run command: %s", [commandToRun UTF8String]);
    
    [mTask setArguments: arguments];
    
    mPipe = [NSPipe pipe];
    [mTask setStandardOutput: mPipe];
    [mTask setStandardError: [mTask standardOutput]];
    
    mFile = [mPipe fileHandleForReading];
    
    [mFile waitForDataInBackgroundAndNotify];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(receivedData:) name:NSFileHandleDataAvailableNotification object:mFile];
    
    [mTask launch];
}
@end

namespace xhn
{
    void RemoveQuotes(const char* srcPath, const char* dstPath)
    {
        NSFileManager* fileManager = [NSFileManager defaultManager];
        NSData* srcData = [fileManager contentsAtPath:[NSString stringWithUTF8String:srcPath]];
        const char* bytes = (const char*)[srcData bytes];
        euint length = [srcData length];
        xhn::string prefixString = "\"L_selector";
        xhn::string selectorInfixingString = "(";
        xhn::string selectorDataInfixingString = "_data(";
        xhn::vector<char> matchBuffer;
        xhn::vector<char> buffer;
        bool isWaitingProcessingIsCompleted = false;
        bool isMatchingSelectorString = false;
        bool isMatchingSelectorDataString = false;
        bool isWaitingCompletedForSelector = false;
        bool isWaitingCompletedForSelectorData = false;
        buffer.reserve(length + 1);
        for (euint i = 0; i < length; i++) {
            ///printf("%c\n", bytes[i]);
            if (isWaitingProcessingIsCompleted) {
                
                if (isWaitingCompletedForSelector) {
                    if (')' != bytes[i]) {
                        if (':' != bytes[i]) {
                            matchBuffer.push_back(bytes[i]);
                        }
                        else {
                            matchBuffer.push_back('S');
                        }
                    }
                    else {
                        for (auto c : prefixString) {
                            if ('"' != c)
                                buffer.push_back(c);
                        }
                        
                        buffer.push_back('_');
                        buffer.push_back('P');
                        buffer.push_back('_');
                        
                        for (auto c : matchBuffer) {
                            buffer.push_back(c);
                        }
                        
                        buffer.push_back('_');
                        buffer.push_back('P');
                        buffer.push_back('_');
                        
                        matchBuffer.clear();
                        isWaitingCompletedForSelector = false;
                        isWaitingProcessingIsCompleted = false;
                        /// ++ 的目的是为了跳过末尾的引号字符
                        i++;
                        continue;
                    }
                }
                else if (isWaitingCompletedForSelectorData) {
                    if (')' != bytes[i]) {
                        if (':' != bytes[i]) {
                            matchBuffer.push_back(bytes[i]);
                        }
                        else {
                            matchBuffer.push_back('S');
                        }
                    }
                    else {
                        for (auto c : prefixString) {
                            if ('"' != c)
                                buffer.push_back(c);
                        }
                        buffer.push_back('_');
                        buffer.push_back('d');
                        buffer.push_back('a');
                        buffer.push_back('t');
                        buffer.push_back('a');
                        
                        buffer.push_back('_');
                        buffer.push_back('P');
                        buffer.push_back('_');
                        
                        for (auto c : matchBuffer) {
                            buffer.push_back(c);
                        }
                        
                        buffer.push_back('_');
                        buffer.push_back('P');
                        buffer.push_back('_');
                        
                        matchBuffer.clear();
                        isWaitingCompletedForSelectorData = false;
                        isWaitingProcessingIsCompleted = false;
                        /// ++ 的目的是为了跳过末尾的引号字符
                        i++;
                        continue;
                    }
                }
                
                if (!isMatchingSelectorString && !isMatchingSelectorDataString &&
                    !isWaitingCompletedForSelector && !isWaitingCompletedForSelectorData) {
                    if ('(' == bytes[i]) {
                        EAssert(matchBuffer.size() == 0, "error %lld", matchBuffer.size());
                    }
                    if (selectorInfixingString[matchBuffer.size()] == bytes[i]) {
                        EAssert(matchBuffer.size() == 0, "error");
                        isWaitingCompletedForSelector = true;
                        matchBuffer.clear();
                    }
                    else if (selectorDataInfixingString[matchBuffer.size()] == bytes[i]) {
                        isMatchingSelectorDataString = true;
                        matchBuffer.push_back(bytes[i]);
                    }
                    else {
                        EAssert(0, "error");
                    }
                    
                }
                else if (isMatchingSelectorString &&
                         !isWaitingCompletedForSelector && !isWaitingCompletedForSelectorData) {
                    if (selectorInfixingString[matchBuffer.size()] == bytes[i]) {
                        matchBuffer.push_back(bytes[i]);
                        if (matchBuffer.size() == selectorInfixingString.size()) {
                            isMatchingSelectorString = false;
                            isWaitingCompletedForSelector = true;
                            matchBuffer.clear();
                        }
                    }
                    else {
                        EAssert(0, "error");
                    }
                }
                else if (isMatchingSelectorDataString &&
                         !isWaitingCompletedForSelector && !isWaitingCompletedForSelectorData) {
                    if (selectorDataInfixingString[matchBuffer.size()] == bytes[i]) {
                        matchBuffer.push_back(bytes[i]);
                        if (matchBuffer.size() == selectorDataInfixingString.size()) {
                            isMatchingSelectorDataString = false;
                            isWaitingCompletedForSelectorData = true;
                            matchBuffer.clear();
                        }
                    }
                    else {
                        for (euint j = i - 20; j < i + 20; j++) {
                            printf("%c", bytes[j]);
                        }
                        EAssert(0, "error");
                    }
                }
            }
            
            if (!isWaitingProcessingIsCompleted &&
                !isMatchingSelectorString &&
                !isMatchingSelectorDataString &&
                !isWaitingCompletedForSelector &&
                !isWaitingCompletedForSelectorData) {
                if (prefixString[matchBuffer.size()] == bytes[i]) {
                    matchBuffer.push_back(bytes[i]);
                    if (matchBuffer.size() == prefixString.size()) {
                        isWaitingProcessingIsCompleted = true;
                        matchBuffer.clear();
                    }
                }
                else {
                    if (matchBuffer.size()) {
                        for (auto c : matchBuffer) {
                            buffer.push_back(c);
                        }
                        isWaitingProcessingIsCompleted = false;
                        matchBuffer.clear();
                    }
                    buffer.push_back(bytes[i]);
                }
            }
        }
        NSMutableData* data = [NSMutableData new];
        [data appendBytes:buffer.get() length:buffer.size()];
        [fileManager createFileAtPath:[NSString stringWithUTF8String:dstPath]
                             contents:data attributes:nil];
    }
    void _TestASMSyntaxConverter()
    {
        __block volatile BOOL stopFlag = NO;
        NSString* cmd =
        
        @"/Users/xhnsworks/Projects/llvm-bin/Debug/bin/llc /Users/xhnsworks/VEngineProjects/tmp0.ll\n";
        //@"/Users/xhnsworks/Projects/llvm-bin/Debug/bin/llc /Users/xhnsworks/VEngineProjects/tmp0.ll /Users/xhnsworks/VEngineProjects/tmp1.ll /Users/xhnsworks/VEngineProjects/tmp2.ll";
        LLCCommandLineUtil* util = [LLCCommandLineUtil new];
        [util runCommand:cmd callback:^(){
            RemoveQuotes("/Users/xhnsworks/VEngineProjects/tmp0.s", "/Users/xhnsworks/VEngineProjects/tmp0.ss");
            stopFlag = YES;
        }];
        ///while (!stopFlag) {}
        {
            auto inst = s_ASMCommandLineUtilsLock.Lock();
            if (!s_ASMCommandLineUtils) {
                s_ASMCommandLineUtils = [NSMutableSet new];
            }
            [s_ASMCommandLineUtils addObject:util];
        }
    }
    
}
