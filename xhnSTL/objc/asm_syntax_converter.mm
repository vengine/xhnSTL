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

@interface ASMCommandLineUtil : NSObject
- (void) runCommand:(NSString*)commandToRun callback:(void(^)())proc;
@end

@implementation ASMCommandLineUtil
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
    
    void _TestASMSyntaxConverter()
    {
        __block volatile BOOL stopFlag = NO;
        NSString* cmd =
        
        @"/Users/xhnsworks/Projects/llvm-bin/Debug/bin/llc /Users/xhnsworks/VEngineProjects/tmp0.ll\n";
        //@"/Users/xhnsworks/Projects/llvm-bin/Debug/bin/llc /Users/xhnsworks/VEngineProjects/tmp0.ll /Users/xhnsworks/VEngineProjects/tmp1.ll /Users/xhnsworks/VEngineProjects/tmp2.ll";
        ASMCommandLineUtil* util = [ASMCommandLineUtil new];
        [util runCommand:cmd callback:^(){ stopFlag = YES; }];
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
