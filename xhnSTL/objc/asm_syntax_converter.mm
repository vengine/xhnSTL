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
    class DirectiveCatcher
    {
    public:
        xhn::string m_matchingString;
        xhn::vector<char> m_matchBuffer;
        xhn::vector<char> m_outputBuffer;
    public:
        DirectiveCatcher(const char* directive)
        : m_matchingString(directive)
        {}
        virtual void CatchImpl(char c) = 0;
        virtual void EndCatch() {
            m_outputBuffer.push_back('\n');
        }
        void Catch(const char* bytes, euint length)
        {
            bool isCatching = false;
            for (euint i = 0; i < length; i++) {
                if (isCatching) {
                    if ('\n' == bytes[i]) {
                        EndCatch();
                        isCatching = false;
                    }
                    else {
                        CatchImpl(bytes[i]);
                    }
                }
                else {
                    if (m_matchingString[m_matchBuffer.size()] == bytes[i]) {
                        m_matchBuffer.push_back(bytes[i]);
                        if (m_matchBuffer.size() == m_matchingString.size()) {
                            isCatching = true;
                            m_matchBuffer.clear();
                        }
                    }
                    else {
                        if (m_matchBuffer.size()) {
                            for (auto c : m_matchBuffer) {
                                m_outputBuffer.push_back(c);
                            }
                            m_matchBuffer.clear();
                        }
                        m_outputBuffer.push_back(bytes[i]);
                    }
                }
                
            }
        }
    };
    
    class SectionCatcher : public DirectiveCatcher
    {
    public:
        SectionCatcher()
        : DirectiveCatcher(".section")
        {}
        virtual void CatchImpl(char c) override {}
    };
    
    class ZerofillCatcher : public DirectiveCatcher
    {
    public:
        SymbolBuffer m_symbolBuffer;
        euint m_symbolCount;
        xhn::string m_symbol;
        xhn::string m_size;
    public:
        ZerofillCatcher()
        : DirectiveCatcher(".zerofill")
        , m_symbolCount(0)
        {}
        virtual void CatchImpl(char c) override {
            if (m_symbolCount < 2) {
                if (',' == c) {
                    m_symbolCount++;
                }
            }
            else if (m_symbolCount == 2) {
                if (',' == c) {
                    m_symbol = m_symbolBuffer.GetString();
                    m_symbolBuffer.Clear();
                    m_symbolCount++;
                }
                else if (' ' != c) {
                    m_symbolBuffer.AddCharacter(c);
                }
            }
            else if (m_symbolCount == 3) {
                if (',' == c) {
                    m_size = m_symbolBuffer.GetString();
                    m_symbolBuffer.Clear();
                    m_symbolCount++;
                }
                else if (' ' != c) {
                    m_symbolBuffer.AddCharacter(c);
                }
            }
        }
        virtual void EndCatch() override {
            m_outputBuffer.push_back('\n');
            
            xhn::string filledString = ".globl ";
            filledString += m_symbol;
            filledString += "\n";
            filledString += ".space ";
            filledString += m_size;
            filledString += ", 0\n";
            
            for (auto c : filledString) {
                m_outputBuffer.push_back(c);
            }
            
            m_symbol = "";
            m_symbolBuffer.Clear();
            m_symbolCount = 0;
            
        }
    };
    
    class NoDeadStripCatcher : public DirectiveCatcher
    {
    public:
        NoDeadStripCatcher()
        : DirectiveCatcher(".no_dead_strip")
        {}
        virtual void CatchImpl(char c) override {}
    };
    
    class AltEntryCatcher : public DirectiveCatcher
    {
    public:
        AltEntryCatcher()
        : DirectiveCatcher(".alt_entry")
        {}
        virtual void CatchImpl(char c) override {}
    };
    
    class LinkerOptionCatcher : public DirectiveCatcher
    {
    public:
        LinkerOptionCatcher()
        : DirectiveCatcher(".linker_option")
        {}
        virtual void CatchImpl(char c) override {}
    };
    
    class IndirectSymbolCatcher : public DirectiveCatcher
    {
    public:
        xhn::SymbolBuffer m_symbolBuffer;
    public:
        IndirectSymbolCatcher()
        : DirectiveCatcher(".indirect_symbol")
        {}
        virtual void CatchImpl(char c) override {
            if (' ' != c) {
                m_symbolBuffer.AddCharacter(c);
            }
        }
        virtual void EndCatch() override {
            m_outputBuffer.push_back('.');
            m_outputBuffer.push_back('l');
            m_outputBuffer.push_back('o');
            m_outputBuffer.push_back('n');
            m_outputBuffer.push_back('g');
            m_outputBuffer.push_back(' ');
            xhn::string symbol = m_symbolBuffer.GetString();
            for (auto c : symbol) {
                m_outputBuffer.push_back(c);
            }
            m_symbolBuffer.Clear();
            DirectiveCatcher::EndCatch();
        }
    };
    
    template <typename T>
    void RunDirectiveCatcher(const char* srcPath, const char* dstPath)
    {
        NSFileManager* fileManager = [NSFileManager defaultManager];
        NSData* srcData = [fileManager contentsAtPath:[NSString stringWithUTF8String:srcPath]];
        const char* bytes = (const char*)[srcData bytes];
        euint length = [srcData length];
        
        T c;
        c.Catch(bytes, length);
        
        NSMutableData* data = [NSMutableData new];
        [data appendBytes:c.m_outputBuffer.get() length:c.m_outputBuffer.size()];
        
        if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:dstPath]]) {
            NSError* error = nil;
            [fileManager removeItemAtPath:[NSString stringWithUTF8String:dstPath] error:&error];
        }
        [fileManager createFileAtPath:[NSString stringWithUTF8String:dstPath]
                             contents:data attributes:nil];
    }
    
    void RemoveAltEntryDirective(const char* srcPath, const char* dstPath)
    {
        RunDirectiveCatcher<AltEntryCatcher>(srcPath, dstPath);
    }
    void RemoveNoDeadStripDirective(const char* srcPath, const char* dstPath)
    {
        RunDirectiveCatcher<NoDeadStripCatcher>(srcPath, dstPath);
    }
    void ConvertZerofillDirective(const char* srcPath, const char* dstPath)
    {
        RunDirectiveCatcher<ZerofillCatcher>(srcPath, dstPath);
    }
    void RemoveSectionDirective(const char* srcPath, const char* dstPath)
    {
        RunDirectiveCatcher<SectionCatcher>(srcPath, dstPath);
    }
    void RemoveQuotes(const xhn::vector<char>& input, xhn::vector<char>& output)
    {
        const char* bytes = input.get();
        euint length = input.size();
        xhn::string prefixString = "\"L_selector";
        xhn::string selectorInfixingString = "(";
        xhn::string selectorDataInfixingString = "_data(";
        xhn::vector<char> matchBuffer;
        xhn::vector<char>& buffer = output;
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
    }
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
        
        if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:dstPath]]) {
            NSError* error = nil;
            [fileManager removeItemAtPath:[NSString stringWithUTF8String:dstPath] error:&error];
        }
        [fileManager createFileAtPath:[NSString stringWithUTF8String:dstPath]
                             contents:data attributes:nil];
    }
    
    void RunAllDirectiveCatchers(const char* srcPath, const char* dstPath)
    {
        NSFileManager* fileManager = [NSFileManager defaultManager];
        NSData* srcData = [fileManager contentsAtPath:[NSString stringWithUTF8String:srcPath]];
        const char* bytes = (const char*)[srcData bytes];
        euint length = [srcData length];
        
        xhn::vector<char> input;
        xhn::vector<char> output;
        input.resize(length);
        memcpy(input.get(), bytes, length);
        
        SectionCatcher sc;
        ZerofillCatcher zc;
        NoDeadStripCatcher nc;
        AltEntryCatcher ac;
        LinkerOptionCatcher lc;
        IndirectSymbolCatcher ic;
        RemoveQuotes(input, output);
        sc.Catch(output.get(), output.size());
        zc.Catch(sc.m_outputBuffer.get(), sc.m_outputBuffer.size());
        nc.Catch(zc.m_outputBuffer.get(), zc.m_outputBuffer.size());
        ac.Catch(nc.m_outputBuffer.get(), nc.m_outputBuffer.size());
        lc.Catch(ac.m_outputBuffer.get(), ac.m_outputBuffer.size());
        ic.Catch(lc.m_outputBuffer.get(), lc.m_outputBuffer.size());
        
        NSMutableData* data = [NSMutableData new];
        [data appendBytes:ic.m_outputBuffer.get() length:ic.m_outputBuffer.size()];
        
        if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:dstPath]]) {
            NSError* error = nil;
            [fileManager removeItemAtPath:[NSString stringWithUTF8String:dstPath] error:&error];
        }
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
            ///RemoveQuotes("/Users/xhnsworks/VEngineProjects/tmp0.s", "/Users/xhnsworks/VEngineProjects/tmp0.ss");
            RunAllDirectiveCatchers("/Users/xhnsworks/VEngineProjects/tmp0.s", "/Users/xhnsworks/VEngineProjects/tmp0.ss");
            /**
            RemoveSectionDirective("/Users/xhnsworks/VEngineProjects/tmp0.ss", "/Users/xhnsworks/VEngineProjects/tmp0.sss");
            ConvertZerofillDirective("/Users/xhnsworks/VEngineProjects/tmp0.sss", "/Users/xhnsworks/VEngineProjects/tmp0.ssss");
            RemoveNoDeadStripDirective("/Users/xhnsworks/VEngineProjects/tmp0.ssss", "/Users/xhnsworks/VEngineProjects/tmp0.sssss");
            RemoveAltEntryDirective("/Users/xhnsworks/VEngineProjects/tmp0.sssss", "/Users/xhnsworks/VEngineProjects/tmp0.ssssss");
             **/
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
