//
//  X7zPropertys.h
//  7zGUI
//
//  Created by 徐 海宁 on 13-11-14.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Singleton.h"
typedef enum
{
    AllRight,
    WrongPassword,
    ArchiveCorruption,
} ObjCLastError;

@class PreviewFileSystemItemTree;
@interface ObjCPropertys : NSObject
{
@public
    __weak PreviewFileSystemItemTree* previewFileSystemItemTree;
}
@property (assign) char* sampleString;
@property (assign) NSStringEncoding curtStringEncoding;
@property (strong) NSString* xinputPath;
@property (assign) ObjCLastError lastError;
@property (strong) NSString* ObjCPassword;
Singleton_Decl(ObjCPropertys);
@end
