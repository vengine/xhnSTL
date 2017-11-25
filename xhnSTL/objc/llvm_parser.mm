//
//  llvm_parser.cpp
//  xhnSTL
//
//  Created by 徐海宁 on 2017/3/18.
//  Copyright © 2017年 徐 海宁. All rights reserved.
//

#include "llvm_parser.hpp"
#include "asm_syntax_converter.hpp"
#import <Foundation/Foundation.h>

static xhn::SpinLock s_LLVMCommandLineUtilsLock;
static NSMutableSet* s_LLVMCommandLineUtils = nil;

@interface LLVMCommandLineUtil : NSObject
- (void) runCommand:(NSString*)commandToRun callback:(void(^)())proc;
@end

@implementation LLVMCommandLineUtil
{
    void(^mCallback)();
    NSTask *mTask;
    NSPipe *mPipe;
    NSFileHandle *mFile;
    
    xhn::string mFilePath;
    euint32 mFileCount;
    NSMutableData* mWriteBuffer;
    xhn::string mMatchingString;
    xhn::vector<char> mMatchBuffer;
}
- (void) writeToFile
{
    if ([mWriteBuffer length] == 0)
        return;
    char mbuf[256];
    snprintf(mbuf, 256, "%s%d.ll", mFilePath.c_str(), mFileCount++);
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:mbuf]]) {
        NSError* error = nil;
        [fileManager removeItemAtPath:[NSString stringWithUTF8String:mbuf] error:&error];
    }
    [fileManager createFileAtPath:[NSString stringWithUTF8String:mbuf] contents:mWriteBuffer attributes:nil];
}
- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];
        
        const char* string = (const char*)data.bytes;
        
        for (euint i = 0; i < data.length; i++) {
            
            if (mMatchingString[mMatchBuffer.size()] == string[i]) {
                mMatchBuffer.push_back(string[i]);
                if (mMatchBuffer.size() == mMatchingString.size()) {
                    [self writeToFile];
                    mWriteBuffer = [NSMutableData new];
                    [mWriteBuffer appendBytes:mMatchBuffer.get() length:mMatchBuffer.size()];
                    mMatchBuffer.clear();
                }
            }
            else {
                if (mMatchBuffer.size()) {
                    [mWriteBuffer appendBytes:mMatchBuffer.get() length:mMatchBuffer.size()];
                }
                mMatchBuffer.clear();
                [mWriteBuffer appendBytes:&string[i] length:1];
            }
        }
    }
    else {
        if ([mWriteBuffer length]) {
            [self writeToFile];
            mWriteBuffer = nil;
        }
        mCallback();
    }
}

- (void) runCommand:(NSString*)commandToRun callback:(void(^)())proc
{
    mFilePath = "/Users/xhnsworks/VEngineProjects/tmp";
    mFileCount = 0;
    mMatchingString = "; ModuleID";
    mWriteBuffer = [NSMutableData new];
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

void _TestLLVMParser()
{
    __block volatile BOOL stopFlag = NO;
    NSString* cmd =
    
    @"xcrun -sdk iphoneos swiftc -target armv7-apple-ios8.0 -parse-as-library -emit-ir /Users/xhnsworks/VEngineProjects/initial_script.swift /Users/xhnsworks/VEngineProjects/VEngineBridgingInterface.swift /Users/xhnsworks/VEngineProjects/VEngineBridgingInterface2.swift -module-name VEngineLogic\n";
    //@"/Users/xhnsworks/Projects/llvm-bin/Debug/bin/llc /Users/xhnsworks/VEngineProjects/tmp0.ll /Users/xhnsworks/VEngineProjects/tmp1.ll /Users/xhnsworks/VEngineProjects/tmp2.ll";
    LLVMCommandLineUtil* util = [LLVMCommandLineUtil new];
    [util runCommand:cmd callback:^(){
        _TestASMSyntaxConverter();
        stopFlag = YES;
    }];
    ///while (!stopFlag) {}
    {
        auto inst = s_LLVMCommandLineUtilsLock.Lock();
        if (!s_LLVMCommandLineUtils) {
            s_LLVMCommandLineUtils = [NSMutableSet new];
        }
        [s_LLVMCommandLineUtils addObject:util];
    }
}

}
