#import <Foundation/Foundation.h>
#include "xhn_filesystem_node.h"

void _push(xhn::filesystem_node* rootNode,
           NSString* nsrootfolder,
           NSString* nssubpath,
           xhn::filesystem_node* node,
           xhn::map<xhn::string, xhn::filesystem_node*>& folderNodeMap)
{
    NSRange range = [nssubpath rangeOfString:@"/" options:NSBackwardsSearch];
    if (range.location != NSNotFound) {
        NSString* nsupperpath = [nssubpath substringWithRange:NSMakeRange(0, range.location)];
        xhn::string upperpath = [nsupperpath UTF8String];
        if (folderNodeMap.find(upperpath) == folderNodeMap.end()) {
            NSString* nsfullpath = [nsrootfolder stringByAppendingPathComponent:nssubpath];
            xhn::string fullpath = [nsfullpath UTF8String];
            xhn::filesystem_node* folder = new (rootNode->gc) xhn::filesystem_node( xhn::Utf8(fullpath, __FILE__, __LINE__), xhn::filesystem_node::folder_node );
            _push(rootNode, nsrootfolder, nsupperpath, folder, folderNodeMap);
        }
        
        folderNodeMap[upperpath]->add_child_node(node);
    }
    else {
        rootNode->add_child_node(node);
    }

    if (node->m_type == xhn::filesystem_node::folder_node) {
        xhn::string subpath = [nssubpath UTF8String];
        folderNodeMap[subpath] = node;
    }
}

xhn::filesystem_node* xhn::filesystem_node::collect_filesystem_tree(GCRootHandle& root, Utf8& path)
{
    NSString* nspath = [[NSString alloc] initWithUTF8String:((string)path).c_str()];
    map<string, filesystem_node*> folderNodeMap;
    BOOL isDir = NO;
    if ([[NSFileManager defaultManager] fileExistsAtPath:nspath isDirectory:&isDir]) {
        if (isDir) {
            filesystem_node* rootNode = new (root.m_gcRootObject->gc) filesystem_node(path, folder_node);
            root.m_gcRootObject->gc->Attach(root.m_gcRootObject->gcNode, rootNode->gcNode);
            
            NSError* subpathError = nil;
            NSError* attrError = nil;
            NSArray* nssubpaths = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:nspath error:&subpathError];
            for (NSString* nssubpath in nssubpaths) {
                if ([nssubpath hasSuffix:@".DS_Store"])
                    continue;
                
                NSString* nsfullpath = [nspath stringByAppendingPathComponent:nssubpath];
                string s = [nsfullpath UTF8String];
                string subpath = [nssubpath UTF8String];
                Utf8 utf8Fullpath( s, __FILE__, __LINE__ );
                NSDictionary* attrs = [[NSFileManager defaultManager] attributesOfItemAtPath:nsfullpath error:&attrError];
                
                filesystem_node* node = NULL;
                if ([attrs[NSFileType] isEqualToString:NSFileTypeRegular]) {
                    node = new (root.m_gcRootObject->gc) filesystem_node(utf8Fullpath, file_node);
                }
                else if ([attrs[NSFileType] isEqualToString:NSFileTypeDirectory]) {
                    node = new (root.m_gcRootObject->gc) filesystem_node(utf8Fullpath, folder_node);
                    
                }
                
                if (node) {
                    _push(rootNode, nspath, nssubpath, node, folderNodeMap);
                }
            }
            return rootNode;
        }
    }
    return NULL;
}