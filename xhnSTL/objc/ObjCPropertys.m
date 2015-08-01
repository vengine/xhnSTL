//
//  X7zPropertys.m
//  7zGUI
//
//  Created by 徐 海宁 on 13-11-14.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#import "ObjCPropertys.h"

@implementation ObjCPropertys
Singleton_Impl(ObjCPropertys);
- (void)initImpl
{
    self.sampleString = nil;
    self.curtStringEncoding = 0x00;
    self.xinputPath = nil;
    previewFileSystemItemTree = nil;
    self.ObjCPassword = nil;
    self.lastError = AllRight;
}
- (void)releaseImpl
{
}
@end
