//
//  Singleton.h
//  7zGUI
//
//  Created by 徐 海宁 on 13-11-13.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#import <Foundation/Foundation.h>

#define Singleton_Decl(classname) \
+ (classname *)instance; \
- (void)initImpl; \
- (void)releaseImpl;

#if !__has_feature(objc_arc)
#define Singleton_Impl(classname) \
static classname * classname##instance = nil; \
+ (classname *)instance { \
if (!classname##instance) { \
classname##instance = [[super allocWithZone:NULL] init]; \
[classname##instance initImpl]; \
} \
return classname##instance; \
} \
+ (id)allocWithZone:(NSZone *)zone { \
return [self instance]; \
} \
- (id)copyWithZone:(NSZone *)zone { \
return self; \
} \
- (id)init { \
if (classname##instance) { \
return classname##instance; \
} \
self = [super init]; \
return self; \
} \
- (id)retain { \
return self; \
} \
- (oneway void)release { \
[self releaseImpl]; \
} \
- (id)autorelease { \
return self; \
} \
- (NSUInteger)retainCount { \
return NSUIntegerMax; \
}
#else
#define Singleton_Impl(classname) \
static classname * classname##instance = nil; \
+ (classname *)instance { \
if (!classname##instance) { \
classname##instance = [[super allocWithZone:NULL] init]; \
[classname##instance initImpl]; \
} \
return classname##instance; \
} \
+ (id)allocWithZone:(NSZone *)zone { \
return [self instance]; \
} \
- (id)copyWithZone:(NSZone *)zone { \
return self; \
} \
- (id)init { \
if (classname##instance) { \
return classname##instance; \
} \
self = [super init]; \
return self; \
}
#endif
