//
//  llvm_parser.cpp
//  xhnSTL
//
//  Created by 徐海宁 on 2017/3/18.
//  Copyright © 2017年 徐 海宁. All rights reserved.
//

#include "llvm_parser.hpp"
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
}

- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];
        
        printf("%s", (char*)data.bytes);
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

void _TestLLVMParser()
{
    __block volatile BOOL stopFlag = NO;
    NSString* cmd =
    
    @"swiftc -parse-as-library -emit-ir /Users/xhnsworks/VEngineProjects/initial_script.swift /Users/xhnsworks/VEngineProjects/VEngineBridgingInterface.swift /Users/xhnsworks/VEngineProjects/VEngineBridgingInterface2.swift -module-name VEngineLogic";
    LLVMCommandLineUtil* util = [LLVMCommandLineUtil new];
    [util runCommand:cmd callback:^(){ stopFlag = YES; }];
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
