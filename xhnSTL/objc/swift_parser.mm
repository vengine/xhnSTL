//
//  swift_parser.cpp
//  VEngine
//
//  Created by 徐海宁 on 16/4/9.
//  Copyright © 2016年 徐海宁. All rights reserved.
//

#include "swift_parser.hpp"
#include "utility.h"
#include "xhn_lock.hpp"
#import <Foundation/Foundation.h>

#define USING_AST_LOG 0

#if USING_AST_LOG

#define AST_BUFFER_SIZE (1024 * 512)
#define COMMAND_BUFFER_SIZE (1024)
#define ASTLog(fmt,...) { \
int size = \
snprintf(s_ASTBuffer,AST_BUFFER_SIZE - 2,fmt,##__VA_ARGS__); \
fwrite(s_ASTBuffer, 1, size, s_ASTFile); }

#define COMMANDLog(fmt,...) { \
int size = \
snprintf(s_COMMANDBuffer,COMMAND_BUFFER_SIZE - 2,fmt,##__VA_ARGS__); \
fwrite(s_COMMANDBuffer, 1, size, s_COMMANDFile); }

static char s_ASTBuffer[AST_BUFFER_SIZE];
static FILE* s_ASTFile = nullptr;

static char s_COMMANDBuffer[AST_BUFFER_SIZE];
static FILE* s_COMMANDFile = nullptr;

#else
#define ASTLog(fmt,...)
#define COMMANDLog(fmt,...)
#endif

static xhn::SpinLock s_SwiftCommandLineUtilsLock;
static NSMutableSet* s_SwiftCommandLineUtils = nil;

@interface SwiftCommandLineUtil : NSObject
@property (assign) xhn::SwiftParser* parser;
- (void) runCommand:(NSString*)commandToRun callback:(void(^)(const xhn::string&))proc;
@end

namespace xhn {
    
    const xhn::static_string SwiftParser::StrSourceFile("source_file");
    const xhn::static_string SwiftParser::StrClassDecl("class_decl");
    const xhn::static_string SwiftParser::StrImportDecl("import_decl");
    const xhn::static_string SwiftParser::StrFuncDecl("func_decl");
    const xhn::static_string SwiftParser::StrInherits("inherits:");
    
    const xhn::static_string SwiftParser::StrSceneNodeAgent("SceneNodeAgent");

    SwiftParser::SymbolBuffer::SymbolBuffer()
    {
        buffer[0] = 0;
        bufferTop = 0;
    }
    
    void SwiftParser::SymbolBuffer::AddCharacter(char c)
    {
        buffer[bufferTop] = c;
        bufferTop++;
        buffer[bufferTop] = 0;
    }
    
    bool SwiftParser::SymbolBuffer::IsInteger()
    {
        if (bufferTop) {
            int num;
            return to_int(buffer, &num);
        }
        else {
            return false;
        }
    }
    
    bool SwiftParser::SymbolBuffer::IsIdentifier()
    {
        if (bufferTop > 0) {
            bool isLetter =
            (buffer[0] >= 'a' && buffer[0] <= 'z') ||
            (buffer[0] >= 'A' && buffer[0] <= 'Z');
            return isLetter;
        }
        else {
            return false;
        }
    }
    
    bool SwiftParser::SymbolBuffer::IsFunctionResult()
    {
        if (bufferTop == 2 && buffer[0] == '-' && buffer[1] == '>')
            return true;
        else
            return false;
    }
    
    xhn::static_string SwiftParser::SymbolBuffer::GetSymbol()
    {
        return buffer;
    }
    
    void SwiftParser::BeginParse()
    {
        ///
    }
    string SwiftParser::EndParse()
    {
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            auto iter = root->children.begin();
            auto end = root->children.end();
            for (; iter != end; iter++) {
                ASTNode* node = *iter;
                if (StrClassDecl == node->type) {
                    ASTLog("CLASS:%s %s\n", node->type.c_str(), node->name.c_str());
                }
            }
        }
        return CreateBridgeFile();
    }
    string SwiftParser::CreateBridgeFile()
    {
        xhn::string bridgeFile;
        bridgeFile = "static xhn::SpinLock s_lock;\n";
        bridgeFile += "static NSMutableSet* s_agentSet = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_procDic = nil;\n";
        bridgeFile += "@interface CreateAgentProc : NSObject\n";
        bridgeFile += "@property (assign) SceneNodeAgent* (^createAgentProc)(void*);\n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateAgentProc \n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        bridgeFile += "void InitProcDic() {\n";
        bridgeFile += "    s_agentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_procDic = [NSMutableDictionary new];\n";
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            auto iter = root->children.begin();
            auto end = root->children.end();
            for (; iter != end; iter++) {
                ASTNode* node = *iter;
                if (StrClassDecl == node->type) {
                    if (node->inherits) {
                        auto iter = node->inherits->begin();
                        auto end = node->inherits->end();
                        for (; iter != end; iter++) {
                            if (StrSceneNodeAgent == *iter) {
                                char mbuf[512];
                                snprintf(mbuf, 511,
                                         "    s_procDic[@%c%s%c] = [[CreateAgentProc alloc] initWithProc:^(void* sceneNode)\n"
                                         "    { %s* ret = [[%s alloc] initWithSceneNode:[[VSceneNode alloc] initWithVSceneNode:sceneNode]];\n"
                                         "        [ret Start];\n"
                                         "        return ret;\n"
                                         "    }];\n",
                                         '"', node->name.c_str(), '"', node->name.c_str(), node->name.c_str());
                                bridgeFile += mbuf;
                                break;
                            }
                        }
                    }
                }
            }
        }
        bridgeFile += "}\n";
        bridgeFile += "void* CreateAgent(const char* type, void* sceneNode) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateAgentProc* proc = s_procDic[strType];\n";
        bridgeFile += "    SceneNodeAgent* agent = proc.createAgentProc(sceneNode);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_agentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_agentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID Update:elapsedTime];\n";
        bridgeFile += "}\n";
        ///printf("%s\n", bridgeFile.c_str());
        return bridgeFile;
    }
    void SwiftParser::Parse(const char* strBuffer, euint length)
    {
        euint count = 0;
        
        auto reduceNodeType = [this]()
        {
            if (m_isNodeType) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (StrSourceFile == symbol) {
                    m_roots.push_back(m_nodeStack.back());
                }
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->type = symbol;
                }
                m_isNodeType = false;
                m_isName = true;
            }
            else {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                if (StrInherits == symbol) {
                    m_isInherits = true;
                }
            }
        };
        auto reduceInherit = [this]() {
            xhn::static_string symbol = m_symbolBuffer.GetSymbol();
            if (xhn::static_string::empty_string != symbol) {
                if (m_nodeStack.size()) {
                    if (!m_nodeStack.back()->inherits) {
                        m_nodeStack.back()->inherits = VNEW xhn::vector<xhn::static_string>();
                    }
                    m_nodeStack.back()->inherits->push_back(symbol);
                }
            }
        };
        while (count < length) {
            char c = strBuffer[count];
            ASTLog("%c, m_isNodeType %d, m_isName %d, m_isApostropheBlock %d, m_isQuotationBlock %d\n",
                   c,   m_isNodeType,    m_isName,    m_isApostropheBlock,    m_isQuotationBlock);
            switch (c)
            {
                case '\0':
                    break;
                case '(':
                    m_symbolBuffer.bufferTop = 0;
                    m_isName = false;
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        m_nodeStack.push_back(VNEW ASTNode());
                        m_isNodeType = true;
                        ASTLog("NEW NODE\n");
                    }
                    else {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                case ')':
                    m_isName = false;
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        reduceNodeType();
                        m_symbolBuffer.bufferTop = 0;
                        ASTNode* currentNode = m_nodeStack.back();
                        m_nodeStack.pop_back();
                        if (m_nodeStack.size()) {
                            ASTNode* parentNode = m_nodeStack.back();
                            ASTLog("%s <- %s\n", parentNode->type.c_str(), currentNode->type.c_str());
                            parentNode->children.push_back(currentNode);
                        }
                        ASTLog("PUSH TO PARENT NODE\n");
                    }
                    else {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                case 0x22: /// 双引号
                    if (!m_isQuotationBlock) {
                        m_isQuotationBlock = true;
                    }
                    else {
                        if (m_isName) {
                            xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                            if (m_nodeStack.size()) {
                                m_nodeStack.back()->name = symbol;
                            }
                            m_isName = false;
                        }
                        m_symbolBuffer.bufferTop = 0;
                        m_isQuotationBlock = false;
                    }
                    break;
                case 0x27: /// 单引号
                    m_isName = false;
                    if (!m_isApostropheBlock) {
                        m_isApostropheBlock = true;
                    }
                    else {
                        m_symbolBuffer.bufferTop = 0;
                        m_isApostropheBlock = false;
                    }
                    break;
                case '=':
                case ' ':
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    if (m_isApostropheBlock &&
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduceNodeType();
                        m_symbolBuffer.bufferTop = 0;
                    }
                    break;
                case '\n':
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    if (m_isApostropheBlock &&
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduceNodeType();
                        m_symbolBuffer.bufferTop = 0;
                    }
                    if (m_isInherits) {
                        reduceInherit();
                        m_isInherits = false;
                    }
                    break;
                case ',':
                    if (m_isInherits) {
                        reduceInherit();
                    }
                    else {
                        if (m_isName && !m_isQuotationBlock) {
                            m_isName = false;
                        }
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                default:
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    m_symbolBuffer.AddCharacter(c);
                    break;
            }
            count++;
        }
    }
    void SwiftParser::ParseSwifts(const string& paths, xhn::Lambda<void (const xhn::string&)>& callback)
    {
        SwiftCommandLineUtil* sclu = [SwiftCommandLineUtil new];
        NSString* command = [[NSString alloc] initWithFormat:@"swiftc -dump-ast %@",
                             [[NSString alloc] initWithUTF8String:paths.c_str()]];
        __block xhn::Lambda<void (const xhn::string&)> tmpCallback = callback;
        void (^objcCallback)(const xhn::string&)  = ^(const xhn::string& result) {
            tmpCallback(result);
        };
        [sclu runCommand:command callback:objcCallback];
    }
    SwiftParser::SwiftParser()
    : m_isApostropheBlock(false)
    , m_isQuotationBlock(false)
    , m_isNodeType(false)
    , m_isName(false)
    , m_isInherits(false)
    {
#if USING_AST_LOG
        s_ASTFile = fopen("/Users/xhnsworks/Documents/测试工程/swiftTest/swiftTest/main2.txt", "wb");
#endif
    }
    SwiftParser::~SwiftParser()
    {
#if USING_AST_LOG
        fclose(s_ASTFile);
#endif
    }
}

@implementation SwiftCommandLineUtil
{
    void(^mCallback)(const xhn::string&);
    NSTask *mTask;
    NSPipe *mPipe;
    NSFileHandle *mFile;
}
- (void) dealloc
{
    ASTLog("here\n");
}

- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];

        self.parser->Parse((const char*)[data bytes], [data length]);
        ///fwrite([data bytes], 1, [data length], s_ASTFile);
    }
    else {
        xhn::string ret = self.parser->EndParse();
        ASTLog("%s\n", ret.c_str());
        delete self.parser;
        {
            auto inst = s_SwiftCommandLineUtilsLock.Lock();
            if (s_SwiftCommandLineUtils) {
                [s_SwiftCommandLineUtils removeObject:self];
            }
        }
        mCallback(ret);
    }
}

- (void) runCommand:(NSString*)commandToRun callback:(void(^)(const xhn::string&))proc
{
    mCallback = proc;
    mTask = [[NSTask alloc] init];
    [mTask setLaunchPath: @"/bin/sh"];
    
    NSArray *arguments = [NSArray arrayWithObjects:
                          @"-c" ,
                          [NSString stringWithFormat:@"%@", commandToRun],
                          nil];
#if USING_AST_LOG
    s_COMMANDFile = fopen("/Users/xhnsworks/Documents/测试工程/swiftTest/swiftTest/command.txt", "wb");
#endif
    COMMANDLog("run command: %s", [commandToRun UTF8String]);
#if USING_AST_LOG
    fclose(s_COMMANDFile);
#endif
    [mTask setArguments: arguments];
    
    mPipe = [NSPipe pipe];
    [mTask setStandardOutput: mPipe];
    [mTask setStandardError: [mTask standardOutput]];
    
    mFile = [mPipe fileHandleForReading];
    
    [mFile waitForDataInBackgroundAndNotify];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(receivedData:) name:NSFileHandleDataAvailableNotification object:mFile];
    
    {
        auto inst = s_SwiftCommandLineUtilsLock.Lock();
        if (!s_SwiftCommandLineUtils) {
            s_SwiftCommandLineUtils = [NSMutableSet new];
        }
        [s_SwiftCommandLineUtils addObject:self];
    }
    self.parser = VNEW xhn::SwiftParser();
    self.parser->BeginParse();
    [mTask launch];
}
@end
