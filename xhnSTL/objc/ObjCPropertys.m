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
