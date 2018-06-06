

#include "swift_parser.hpp"
#include "utility.h"
#include "xhn_lock.hpp"
#import <Foundation/Foundation.h>

//NSString* swiftc = @"/Users/xhnsworks/Projects/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/swiftc";
//NSString* sdk = @"/Users/xhnsworks/Projects/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk";
NSString* swiftc = @"/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/swiftc";
NSString* sdk = @"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk";
NSString* usedVersion = @"3";

#define USING_AST_LOG 0

#if USING_AST_LOG

#define AST_BUFFER_SIZE (1024 * 512)
#define COMMAND_BUFFER_SIZE (1024)

#define ASTLog(fmt,...) { \
int size = \
snprintf(s_ASTBuffer,AST_BUFFER_SIZE,fmt,##__VA_ARGS__); \
fwrite(s_ASTBuffer, 1, size, s_ASTLogFile); }

#define ASTNodeLog(fmt,...) { \
int size = \
snprintf(s_ASTNodeBuffer,AST_BUFFER_SIZE,fmt,##__VA_ARGS__); \
fwrite(s_ASTNodeBuffer, 1, size, s_ASTNodeLogFile); }

#define ClassHierarchyLog(fmt,...) { \
int size = \
snprintf(s_ClassHierarchyBuffer,AST_BUFFER_SIZE,fmt,##__VA_ARGS__); \
fwrite(s_ClassHierarchyBuffer, 1, size, s_ClassHierarchyLogFile); }

#define GUILog(fmt,...) { \
int size = \
snprintf(s_GUIBuffer,AST_BUFFER_SIZE,fmt,##__VA_ARGS__); \
fwrite(s_GUIBuffer, 1, size, s_GUILogFile); }

#define COMMANDLog(fmt,...) { \
int size = \
snprintf(s_COMMANDBuffer,COMMAND_BUFFER_SIZE,fmt,##__VA_ARGS__); \
fwrite(s_COMMANDBuffer, 1, size, s_COMMANDFile); }

static char s_ASTBuffer[AST_BUFFER_SIZE];
static FILE* s_ASTLogFile = nullptr;

static char s_ASTNodeBuffer[AST_BUFFER_SIZE];
static FILE* s_ASTNodeLogFile = nullptr;

static char s_ClassHierarchyBuffer[AST_BUFFER_SIZE];
static FILE* s_ClassHierarchyLogFile = nullptr;

static char s_GUIBuffer[AST_BUFFER_SIZE];
static FILE* s_GUILogFile = nullptr;

static char s_COMMANDBuffer[AST_BUFFER_SIZE];
static FILE* s_COMMANDFile = nullptr;

static FILE* s_ASTFile = nullptr;

#else
#define ASTLog(fmt,...)
#define ASTNodeLog(fmt,...)
#define ClassHierarchyLog(fmt,...);
#define GUILog(fmt,...)
#define COMMANDLog(fmt,...)
#endif

static xhn::SpinLock s_SwiftCommandLineUtilsLock;
static NSMutableSet* s_SwiftCommandLineUtils = nil;

@interface SwiftCommandLineUtil : NSObject
@property (assign) UInt32 parseMask;
@property (assign) xhn::Parser* parser;
- (void) getSwiftVersion:(NSString*)commandToRun
                  logDir:(const xhn::string&)logDir
                callback:(void(^)(const xhn::string&))proc;
- (void) parseSwiftSourceFiles:(NSString*)commandToRun
                        logDir:(const xhn::string&)logDir
                   reformatter:(xhn::ASTReformatterPtr)reformatter
                      callback:(void(^)(const xhn::string& objcBridgeFile,
                                        const xhn::string& swiftBridgeFile,
                                        const xhn::string& stateActionFile,
                                        const xhn::vector<xhn::static_string>&,
                                        const xhn::vector<xhn::static_string>&,
                                        const xhn::vector<xhn::static_string>&))proc;
@end

namespace xhn {
    
    ImplementRootRTTI(Parser);
    ImplementRTTI(SwiftVerisonInfoParser, Parser);
    ImplementRTTI(SwiftParser, Parser);
    
    void SwiftVerisonInfoParser::BeginParse()
    {
    }
    
    void SwiftVerisonInfoParser::EndParser(string& result)
    {
        result = m_versionInfo;
    }
    
    void SwiftVerisonInfoParser::Parse(const char* strBuffer, euint length)
    {
        char* buf = (char*)malloc(length + 1);
        memcpy(buf, strBuffer, length);
        buf[length] = 0x00;
        m_versionInfo += buf;
        free(buf);
    }
    
    void SwiftVerisonInfoParser::GetSwiftVersion(const string& logDir,
                                                 Lambda<void (const xhn::string& versionInfo)>& callback)
    {
        SwiftCommandLineUtil* sclu = [SwiftCommandLineUtil new];
        NSString* command = [[NSString alloc] initWithFormat:@"%@ -v", swiftc];
        __block xhn::Lambda<void (const xhn::string& versionInfo)> tmpCallback = callback;
        void (^objcCallback)(const xhn::string& versionInfo)  = ^(const xhn::string& versionInfo) {
            tmpCallback(versionInfo);
        };
        [sclu getSwiftVersion:command
                       logDir:logDir
                     callback:objcCallback];
    }
    
    const static_string SwiftParser::StrSourceFile("source_file");
    const static_string SwiftParser::StrClassDecl("class_decl");
    const static_string SwiftParser::StrImportDecl("import_decl");
    const static_string SwiftParser::StrFuncDecl("func_decl");
    const static_string SwiftParser::StrDeclRefExpr("declref_expr");
    const static_string SwiftParser::StrInherits("inherits:");
    const static_string SwiftParser::StrAccess("access");
    const static_string SwiftParser::StrPrivate("private");
    const static_string SwiftParser::StrInternal("internal");
    const static_string SwiftParser::StrPublic("public");
    const static_string SwiftParser::StrOpen("open");
    const static_string SwiftParser::StrDecl("decl");
    const static_string SwiftParser::StrTypealias("typealias");
    const static_string SwiftParser::StrType("type");
    const static_string SwiftParser::StrInterface("interface");
    
    const static_string SwiftParser::StrSceneNodeAgent("SceneNodeAgent");
    const static_string SwiftParser::StrSceneNodeState("SceneNodeState");
    const static_string SwiftParser::StrSceneNodeStateInterface("SceneNodeStateInterface");
    
    const static_string SwiftParser::StrGUIAgent("GUIAgent");
    const static_string SwiftParser::StrGUIState("GUIState");
    const static_string SwiftParser::StrGUIStateInterface("GUIStateInterface");
    const static_string SwiftParser::StrNormalState("NormalState");
    const static_string SwiftParser::StrHoveringState("HoveringState");
    const static_string SwiftParser::StrSelectedState("SelectedState");
    const static_string SwiftParser::StrPressedState("PressedState");
    const static_string SwiftParser::StrDraggingState("DraggingState");
    const static_string SwiftParser::StrNormalStateBase("NormalStateBase");
    const static_string SwiftParser::StrHoveringStateBase("HoveringStateBase");
    const static_string SwiftParser::StrSelectedStateBase("SelectedStateBase");
    const static_string SwiftParser::StrPressedStateBase("PressedStateBase");
    const static_string SwiftParser::StrDraggingStateBase("DraggingStateBase");
    
    const static_string SwiftParser::StrActorAgent("ActorAgent");
    const static_string SwiftParser::StrAction("Action");
    const static_string SwiftParser::StrActionInterface("ActionInterface");
    
    void SwiftParser::BeginParse()
    {
        ///
    }
    void SwiftParser::EndParse(string& bridgeFile, string& stateActionFile, BridgeFileLanguage language)
    {
        map<static_string, static_string> aliasMap;
        map<static_string, vector<static_string>> inheritMap;
        map<static_string, vector<static_string>> childrenClassMap;
        map<static_string, ASTNode*> classMap;
        
        Lambda<void (ASTNode*)> makeAliasMap;
        Lambda<void (const string&, ASTNode*)> makeInheritMapProc;
        Lambda<void (const string&, ASTNode*)> makeChildrenClassMapProc;
        Lambda<void (const string&, ASTNode*)> makeClassMapProc;
        Lambda<bool (static_string, static_string, vector<static_string>&)> isInheritFromClassProc;
        
        const string dotType(".Type");
        makeAliasMap = [&makeAliasMap, &aliasMap, &dotType](ASTNode* node) -> void {
            if (StrTypealias == node->nodetype) {
                xhn::string strIntType = node->interfacetype.c_str();
                euint pos = strIntType.rfind(dotType);
                if (xhn::string::npos != pos &&
                    pos + dotType.size() == strIntType.size()) {
                    strIntType = strIntType.substr(0, pos);
                    aliasMap[strIntType.c_str()] = node->type;
                    ASTLog("++:%s -> %s\n", strIntType.c_str(), node->type.c_str());
                }
            }
            if (node->children) {
                auto childIter = node->children->begin();
                auto childEnd = node->children->end();
                for (; childIter != childEnd; childIter++) {
                    ASTNode* child = *childIter;
                    makeAliasMap(child);
                }
            }
        };
        
        makeInheritMapProc = [&makeInheritMapProc, &inheritMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->inherits) {
                    auto inhIter = node->inherits->begin();
                    auto inhEnd = node->inherits->end();
                    for (; inhIter != inhEnd; inhIter++) {
                        inheritMap[nodePath.c_str()].push_back(*inhIter);
                    }
                }
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        ASTNode* child = *childIter;
                        
                        makeInheritMapProc(nodePath, child);
                    }
                }
            }
            else {
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        ASTNode* child = *childIter;
                        makeInheritMapProc(parentPath, child);
                    }
                }
            }
        };
        
        makeChildrenClassMapProc = [&makeChildrenClassMapProc, &childrenClassMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        makeChildrenClassMapProc(nodePath, *childIter);
                    }
                }
                if (parentPath.size()) {
                    childrenClassMap[parentPath.c_str()].push_back(nodePath.c_str());
                }
            }
        };
        
        makeClassMapProc = [&makeClassMapProc, &classMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        makeClassMapProc(nodePath, *childIter);
                    }
                }
                classMap[nodePath.c_str()] = node;
            }
        };
        
        isInheritFromClassProc = [&isInheritFromClassProc, &inheritMap](static_string _class,
                                                                        static_string parentClass,
                                                                        vector<static_string>& inheritPath) -> bool {
            auto iter = inheritMap.find(_class);
            if (iter != inheritMap.end()) {
                auto inhIter = iter->second.begin();
                auto inhEnd = iter->second.end();
                for (; inhIter != inhEnd; inhIter++) {
                    if (*inhIter == parentClass) {
                        inheritPath.push_back(parentClass);
                        return true;
                    }
                    vector<static_string> path(inheritPath);
                    ///path.push_back(_class);
                    if (isInheritFromClassProc(*inhIter, parentClass, path)) {
                        inheritPath = path;
                        inheritPath.insert(inheritPath.begin(), (*inhIter));
                        return true;
                    }
                }
            }
            return false;
        };
        
        string emptyPath;
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                makeAliasMap(node);
                makeInheritMapProc(emptyPath, node);
                makeChildrenClassMapProc(emptyPath, node);
                makeClassMapProc(emptyPath, node);
            }
        }
        
        ///
        auto inhMapIter = inheritMap.begin();
        auto inhMapEnd = inheritMap.end();
        for (; inhMapIter != inhMapEnd; inhMapIter++) {
            ASTLog("##%s : ", inhMapIter->first.c_str());
            auto inhIter = inhMapIter->second.begin();
            auto inhEnd = inhMapIter->second.end();
            for (; inhIter != inhEnd; inhIter++) {
                auto aliasiter = aliasMap.find(*inhIter);
                if (aliasiter != aliasMap.end()) {
                    *inhIter = aliasiter->second;
                }
                ASTLog("%s ", (*inhIter).c_str());
            }
            ASTLog("\n");
        }
        vector<static_string> inheritPath;
        if (isInheritFromClassProc("TranslationScript.ZMoveState", "NSObject", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is inherit from NSObject\n");
            ASTLog("PATH: ");
            auto i = inheritPath.begin();
            auto e = inheritPath.end();
            for (; i != e; i++) {
                ASTLog("%s, ", (*i).c_str());
            }
            ASTLog("\n");
        }
        inheritPath.clear();
        if (isInheritFromClassProc("TranslationScript.ZMoveState", "State", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is inherit from State\n");
            ASTLog("PATH: ");
            auto i = inheritPath.begin();
            auto e = inheritPath.end();
            for (; i != e; i++) {
                ASTLog("%s, ", (*i).c_str());
            }
            ASTLog("\n");
        }
        inheritPath.clear();
        if (!isInheritFromClassProc("TranslationScript.ZMoveState", "None", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is not inherit from None\n");
        }
        inheritPath.clear();
        auto classMapIter = classMap.begin();
        auto classMapEnd = classMap.end();
        for (; classMapIter != classMapEnd; classMapIter++) {
            ClassHierarchyLog("#%s\n", classMapIter->first.c_str());
        }
        ClassHierarchyLog("\n");
        auto childMapIter = childrenClassMap.begin();
        auto childMapEnd = childrenClassMap.end();
        for (; childMapIter != childMapEnd; childMapIter++) {
            ClassHierarchyLog("%c %s:\n", '%', childMapIter->first.c_str());
            auto childIter = childMapIter->second.begin();
            auto childEnd = childMapIter->second.end();
            for (; childIter != childEnd; childIter++) {
                ClassHierarchyLog("  %s\n", (*childIter).c_str());
            }
            ClassHierarchyLog("\n");
        }
        bridgeFile = CreateBridgeFile(inheritMap, childrenClassMap, classMap, isInheritFromClassProc, language);
        stateActionFile = CreateStateActionFile(inheritMap, childrenClassMap, classMap, isInheritFromClassProc);
    }
#define TEST_AS_OBJC if (ObjC == language) {
#define ELSE         } else {
#define END          }
    string SwiftParser::CreateBridgeFile(const map<static_string, vector<static_string>>& inheritMap,
                                         const map<static_string, vector<static_string>>& childrenClassMap,
                                         const map<static_string, ASTNode*>& classMap,
                                         Lambda<bool (static_string, static_string,
                                                      vector<static_string>&)>& isInheritFromClassProc,
                                         BridgeFileLanguage language)
    {
        xhn::string bridgeFile;
        char mbuf[1024];
        TEST_AS_OBJC
        bridgeFile = "static xhn::SpinLock s_lock;\n";
        bridgeFile += "void InitSwiftWrapper() {\n";
        bridgeFile += "    [SwiftWrapper InitSwiftWrapper];\n";
        bridgeFile += "}\n";
//        bridgeFile += "void ReleaseObject(void* ptr) {\n";
//        bridgeFile += "    id<Blanker> blanker = (__bridge_transfer id<Blanker>)(ptr);\n";
//        bridgeFile += "    [blanker blankPointer];\n";
//        bridgeFile += "}\n";
//        bridgeFile += "class SwiftSceneNodeFilter : public VEngine::VSceneNodeFilter\n";
//        bridgeFile += "{\n";
//        bridgeFile += "public:\n";
//        bridgeFile += "    id<VSceneNodeFilter> m_swiftSceneNodeFilter;\n";
//        bridgeFile += "public:\n";
//        bridgeFile += "    virtual bool IsFiltered(VEngine::VSceneNodeNamePtr name, VEngine::VType::NodeType type) override {\n";
//        bridgeFile += "        VSceneNodeName* swiftName = (__bridge VSceneNodeName*)name->m_slot;\n";
//        bridgeFile += "        if (!swiftName) {\n";
//        bridgeFile += "            swiftName = [[VSceneNodeName alloc] initWithVSceneNodeName:name.get()];\n";
//        bridgeFile += "        }\n";
//        bridgeFile += "        VSceneNodeType swiftType = UnknownNode;\n";
//        bridgeFile += "        switch (type)\n";
//        bridgeFile += "        {\n";
//        bridgeFile += "           case VEngine::VType::ColladaMeshNode:  swiftType = MeshNode; break;\n";
//        bridgeFile += "           case VEngine::VType::LightNode:        swiftType = LightNode; break;\n";
//        bridgeFile += "           case VEngine::VType::CameraNode:       swiftType = CameraNode; break;\n";
//        bridgeFile += "           case VEngine::VType::EmptyNode:        swiftType = EmptyNode; break;\n";
//        bridgeFile += "           case VEngine::VType::GUIFieldNode:     swiftType = GUIFieldNode; break;\n";
//        bridgeFile += "           case VEngine::VType::GUIContainerNode: swiftType = GUIContainerNode; break;\n";
//        bridgeFile += "           case VEngine::VType::JointNode:        swiftType = JointNode; break;\n";
//        bridgeFile += "           default:                               break;\n";
//        bridgeFile += "        }\n";
//        bridgeFile += "        if (m_swiftSceneNodeFilter) {\n";
//        bridgeFile += "            return [m_swiftSceneNodeFilter isFiltered:swiftName type:swiftType];\n";
//        bridgeFile += "        }\n";
//        bridgeFile += "        return false;\n";
//        bridgeFile += "    }\n";
//        bridgeFile += "};\n";
//        bridgeFile += "class SwiftCallback : public RefObject\n";
//        bridgeFile += "{\n";
//        bridgeFile += "public:\n";
//        bridgeFile += "    void (^m_callback)(void);\n";
//        bridgeFile += "};\n";
        bridgeFile += "static void (*s_printLogProc)(int, const char*) = NULL;\n";
        bridgeFile += "NSMutableSet* s_sceneNodeAgentSet = nil;\n";
        bridgeFile += "NSMutableSet* s_guiAgentSet = nil;\n";
        bridgeFile += "NSMutableSet* s_actorAgentSet = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createSceneNodeAgentProcDic = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createGUIAgentProcDic = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createActorAgentProcDic = nil;\n";
        bridgeFile += "@interface CreateSceneNodeAgentProc : NSObject\n";
        bridgeFile += "@property (strong) SceneNodeAgent* (^createAgentProc)(void*);\n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateSceneNodeAgentProc \n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        bridgeFile += "@interface CreateGUIAgentProc : NSObject\n";
        bridgeFile += "@property (strong) GUIAgent* (^createAgentProc)(void*, void*);\n";
        bridgeFile += "-(id)initWithProc:(GUIAgent*(^)(void*, void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateGUIAgentProc \n";
        bridgeFile += "-(id)initWithProc:(GUIAgent*(^)(void*, void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        bridgeFile += "@interface CreateActorAgentProc : NSObject\n";
        bridgeFile += "@property (strong) ActorAgent* (^createAgentProc)(void*, void*);\n";
        bridgeFile += "-(id)initWithProc:(ActorAgent*(^)(void*, void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateActorAgentProc \n";
        bridgeFile += "-(id)initWithProc:(ActorAgent*(^)(void*, void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        ELSE
        bridgeFile = "func ReleaseObject(ptr : UnsafeRawPointer) {\n";
        bridgeFile += "    let obj = bridgeTransfer(ptr : ptr) as AnyObject\n";
        bridgeFile += "    let blanker = obj as! Blanker\n";
        bridgeFile += "    blanker.blankPointer()\n";
        bridgeFile += "}\n";
        bridgeFile += "open class SwiftCallbackHandle\n";
        bridgeFile += "{\n";
        bridgeFile += "    var callback : () -> Void\n";
        bridgeFile += "    init (callback : @escaping () -> Void) {\n";
        bridgeFile += "        self.callback = callback\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "public func CallbackToVoidPointer(callback : @escaping () -> Void) -> UnsafeRawPointer {\n";
        bridgeFile += "    let handle = SwiftCallbackHandle(callback: callback)\n";
        bridgeFile += "    return bridgeRetained(obj : handle)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func DoCallback(ptr : UnsafeRawPointer) {\n";
        bridgeFile += "    let handle = bridgeTransfer(ptr : ptr) as SwiftCallbackHandle\n";
        bridgeFile += "    handle.callback()\n";
        bridgeFile += "}\n";
        bridgeFile += "public class CreateSceneNodeAgentProc\n";
        bridgeFile += "{\n";
        bridgeFile += "    let createAgentProc : (_ : UnsafeRawPointer)->SceneNodeAgent\n";
        bridgeFile += "    init( proc : @escaping (_ : UnsafeRawPointer)->SceneNodeAgent ) {\n";
        bridgeFile += "        createAgentProc = proc\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "public class CreateGUIAgentProc\n";
        bridgeFile += "{\n";
        bridgeFile += "    let createAgentProc : (_ : UnsafeRawPointer, _ : UnsafeRawPointer)->GUIAgent\n";
        bridgeFile += "    init( proc : @escaping (_ : UnsafeRawPointer, _ : UnsafeRawPointer)->GUIAgent ) {\n";
        bridgeFile += "        createAgentProc = proc\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "public class CreateActorAgentProc\n";
        bridgeFile += "{\n";
        bridgeFile += "    let createAgentProc : (_ : UnsafeRawPointer, _ : UnsafeRawPointer)->ActorAgent\n";
        bridgeFile += "    init( proc : @escaping (_ : UnsafeRawPointer, _ : UnsafeRawPointer)->ActorAgent ) {\n";
        bridgeFile += "        createAgentProc = proc\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "var s_createSceneNodeAgentProcDic = Dictionary<String, CreateSceneNodeAgentProc>()\n";
        bridgeFile += "var s_createGUIAgentProcDic = Dictionary<String, CreateGUIAgentProc>()\n";
        bridgeFile += "var s_createActorAgentProcDic = Dictionary<String, CreateActorAgentProc>()\n";
        bridgeFile += "func swiftClassString(_ nameSpace : String, className : String) -> String {\n";
        bridgeFile += "    let appName = \"VEngineLogic\"\n";
        bridgeFile += "    var classStringName = \"_TtCC\"\n";
        bridgeFile += "    classStringName += String(appName.count)\n";
        bridgeFile += "    classStringName += appName\n";
        bridgeFile += "    classStringName += String(nameSpace.count)\n";
        bridgeFile += "    classStringName += nameSpace\n";
        bridgeFile += "    classStringName += String(className.count)\n";
        bridgeFile += "    classStringName += className\n";
        bridgeFile += "    return classStringName\n";
        bridgeFile += "}\n";
        bridgeFile += "func swiftClassStringFromPath(_ path : String) -> String {\n";
        bridgeFile += "    let appName = \"VEngineLogic\"\n";
        bridgeFile += "    var classStringName = \"_TtCC\"\n";
        bridgeFile += "    classStringName += String(appName.count)\n";
        bridgeFile += "    classStringName += appName\n";
        bridgeFile += "    let subpaths = path.components(separatedBy: \".\")\n";
        bridgeFile += "    for subpath in subpaths {\n";
        bridgeFile += "        classStringName = classStringName + String(subpath.count) + subpath\n";
        bridgeFile += "    }\n";
        bridgeFile += "    return classStringName\n";
        bridgeFile += "}\n";
        bridgeFile += "public func CreateSceneNodeAgent(type : UnsafePointer<Int8>, sceneNode : UnsafeRawPointer)\n";
        bridgeFile += "    -> UnsafeRawPointer? {\n";
        bridgeFile += "    let strType = String(cString : UnsafePointer<Int8>(type))\n";
        bridgeFile += "    guard let proc = s_createSceneNodeAgentProcDic[strType] else { return nil }\n";
        bridgeFile += "    let agent = proc.createAgentProc(sceneNode)\n";
        bridgeFile += "    s_agentSetLock.lock()\n";
        bridgeFile += "    _ = s_sceneNodeAgentSet.add(object : agent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "    return bridgeToPtr(obj : agent)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func RemoveSceneNodeAgent(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    let snagent = bridgeToObject(ptr : agent) as SceneNodeAgent\n";
        bridgeFile += "    s_agentSetLock.lock()\n";
        bridgeFile += "    s_sceneNodeAgentSet.remove(snagent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func UpdateSceneNodeAgent(agent : UnsafeRawPointer, elapsedTime : Double) {\n";
        bridgeFile += "    let snagent = bridgeToObject(ptr : agent) as SceneNodeAgent\n";
        bridgeFile += "    snagent.update(elapsedTime)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func SceneNodeAgentGotoState(agent : UnsafeRawPointer, nextStateName : UnsafePointer<Int8>) {\n";
        bridgeFile += "    let snagent = bridgeToObject(ptr : agent) as SceneNodeAgent\n";
        bridgeFile += "    let strNextStateName = String(cString: nextStateName)\n";
        bridgeFile += "    snagent.gotoState(strNextStateName)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func CreateGUIAgent(type : UnsafePointer<Int8>, renderSys : UnsafeRawPointer, widget : UnsafeRawPointer)\n";
        bridgeFile += "    -> UnsafeRawPointer?\n";
        bridgeFile += "{\n";
        bridgeFile += "    let strType = String(cString: type)\n";
        bridgeFile += "    guard let proc = s_createGUIAgentProcDic[strType] else { return nil }\n";
        bridgeFile += "    let agent = proc.createAgentProc(renderSys, widget)\n";
        bridgeFile += "    s_agentSetLock.lock()\n";
        bridgeFile += "    _ = s_guiAgentSet.add(object : agent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "    return bridgeToPtr(obj: agent)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func RemoveGUIAgent(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    s_agentSetLock.lock()\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    s_guiAgentSet.remove(gagent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func UpdateGUIAgent(agent : UnsafeRawPointer, elapsedTime : Double) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    gagent.update(elapsedTime)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func GotoGUIAgentState(agent : UnsafeRawPointer, stateKind : Int) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    var strStateKind : String? = nil\n";
        bridgeFile += "    switch (stateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "    case 0: strStateKind = \"NormalState\"\n";
        bridgeFile += "    case 1: strStateKind = \"HoveringState\"\n";
        bridgeFile += "    case 2: strStateKind = \"SelectedState\"\n";
        bridgeFile += "    case 3: strStateKind = \"PressedState\"\n";
        bridgeFile += "    case 4: strStateKind = \"DraggingState\"\n";
        bridgeFile += "    default: strStateKind = nil\n";
        bridgeFile += "    }\n";
        bridgeFile += "    if let strStateKindValue = strStateKind {\n";
        bridgeFile += "        gagent.gotoState(strStateKindValue)\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "public func AllowGUIStateChange(agent : UnsafeRawPointer, prevStateKind : Int, nextStateKind : Int) -> Bool {\n";
        bridgeFile += "    var strPrevStateKind : String? = nil\n";
        bridgeFile += "    switch (prevStateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "    case 0: strPrevStateKind = \"NormalState\"\n";
        bridgeFile += "    case 1: strPrevStateKind = \"HoveringState\"\n";
        bridgeFile += "    case 2: strPrevStateKind = \"SelectedState\"\n";
        bridgeFile += "    case 3: strPrevStateKind = \"PressedState\"\n";
        bridgeFile += "    case 4: strPrevStateKind = \"DraggingState\"\n";
        bridgeFile += "    default: strPrevStateKind = nil\n";
        bridgeFile += "    }\n";
        bridgeFile += "    var strNextStateKind : String? = nil\n";
        bridgeFile += "    switch (nextStateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "    case 0: strNextStateKind = \"NormalState\"\n";
        bridgeFile += "    case 1: strNextStateKind = \"HoveringState\"\n";
        bridgeFile += "    case 2: strNextStateKind = \"SelectedState\"\n";
        bridgeFile += "    case 3: strNextStateKind = \"PressedState\"\n";
        bridgeFile += "    case 4: strNextStateKind = \"DraggingState\"\n";
        bridgeFile += "    default: strNextStateKind = nil\n";
        bridgeFile += "    }\n";
        bridgeFile += "    if let strPrevStateKindValue = strPrevStateKind ,let strNextStateKindValue = strNextStateKind {\n";
        bridgeFile += "        let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "        return gagent.allowStateChange(strPrevStateKindValue, nextStateKind: strNextStateKindValue)\n";
        bridgeFile += "    } else {\n";
        bridgeFile += "        return false\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "public func OnGUIWidgetMove(agent : UnsafeRawPointer, dispX : Float, dispY : Float) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    gagent.onMove(dispX, dispY: dispY)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func OnGUIWidgetPress(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    gagent.onPress()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func OnGUIWidgetLeave(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    gagent.onLeave()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func OnGUIWidgetHover(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    let gagent = bridgeToObject(ptr: agent) as GUIAgent\n";
        bridgeFile += "    gagent.onHover()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func CreateActorAgent(type : UnsafePointer<Int8>, renderSys : UnsafeRawPointer, actor : UnsafeRawPointer)\n";
        bridgeFile += "    -> UnsafeRawPointer?\n";
        bridgeFile += "{\n";
        bridgeFile += "    let strType = String(cString: type)\n";
        bridgeFile += "    guard let proc = s_createActorAgentProcDic[strType] else { return nil }\n";
        bridgeFile += "    let agent = proc.createAgentProc(renderSys, actor)\n";
        bridgeFile += "    s_agentSetLock.lock()\n";
        bridgeFile += "    _ = s_actorAgentSet.add(object : agent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "    return bridgeToPtr(obj: agent)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func RemoveActorAgent(agent : UnsafeRawPointer) {\n";
        bridgeFile += "    s_agentSetLock.lock();\n";
        bridgeFile += "    let aagent = bridgeToObject(ptr: agent) as ActorAgent\n";
        bridgeFile += "    s_actorAgentSet.remove(aagent)\n";
        bridgeFile += "    s_agentSetLock.unlock()\n";
        bridgeFile += "}\n";
        bridgeFile += "public func UpdateActorAgent(agent : UnsafeRawPointer, elapsedTime : Double) {\n";
        bridgeFile += "    let aagent = bridgeToObject(ptr: agent) as ActorAgent\n";
        bridgeFile += "    aagent.update(elapsedTime)\n";
        bridgeFile += "}\n";
        bridgeFile += "public func GetNumSceneNodeAgentProcs() -> Int {\n";
        bridgeFile += "    return s_createSceneNodeAgentProcDic.count\n";
        bridgeFile += "}\n";
        bridgeFile += "public func GetNumActorAgentProcs() -> Int {\n";
        bridgeFile += "    return s_createActorAgentProcDic.count\n";
        bridgeFile += "}\n";
        bridgeFile += "public func GetSceneNodeAgentProcTypes() -> UnsafeRawPointer? {\n";
        bridgeFile += "    return nil\n";
        bridgeFile += "}\n";
        bridgeFile += "public func GetActorAgentProcTypes() ->UnsafeRawPointer? {\n";
        bridgeFile += "    return nil\n";
        bridgeFile += "}\n";
        bridgeFile += "public func SetPrintLogProc(proc : UnsafeRawPointer) {\n";
        bridgeFile += "}\n";
        bridgeFile += "public func PrintLog(type : Int, log : UnsafePointer<Int8>) {\n";
        bridgeFile += "}\n";
        END
        TEST_AS_OBJC
        snprintf(mbuf, 1024,
                 "NSString* swiftClassString(NSString *nameSpace, NSString *className) {\n"
                 "    NSString *appName = @%cVEngineLogic%c;\n"
                 "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%cld%c@%cld%c@%c, \n"
                 "    appName.length, appName, nameSpace.length, nameSpace, className.length, className];\n"
                 "    return classStringName;\n"
                 "}\n",
                 '"', '"',
                 '"', '%', '%', '%', '%', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1024,
        "Class swiftClassFromString(NSString *nameSpace, NSString *className) {\n"
        "    NSString *appName = @%cVEngineLogic%c;\n"
        "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%cld%c@%cld%c@%c, \n"
        "    appName.length, appName, nameSpace.length, nameSpace, className.length, className];\n"
        "    return NSClassFromString(classStringName);\n"
        "}\n",
        '"', '"',
        '"', '%', '%', '%', '%', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1024,
                 "NSString* swiftClassStringFromPath(NSString *path) {\n"
                 "    NSString *appName = @%cVEngineLogic%c;\n"
                 "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%c, appName.length, appName];\n"
                 "    NSArray *subpaths = [path componentsSeparatedByString:@%c.%c];\n"
                 "    for (NSString* subpath in subpaths) {\n"
                 "        classStringName = [classStringName stringByAppendingString:[NSString stringWithFormat:@%c%cld%c@%c, subpath.length, subpath]];\n"
                 "    }\n"
                 "    return classStringName;\n"
                 "}\n",
                 '"', '"',
                 '"', '%', '%', '"',
                 '"', '"',
                 '"', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1024,
        "Class swiftClassFromPath(NSString *path) {\n"
        "    NSString *appName = @%cVEngineLogic%c;\n"
        "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%c, appName.length, appName];\n"
        "    NSArray *subpaths = [path componentsSeparatedByString:@%c.%c];\n"
        "    for (NSString* subpath in subpaths) {\n"
        "        classStringName = [classStringName stringByAppendingString:[NSString stringWithFormat:@%c%cld%c@%c, subpath.length, subpath]];\n"
        "    }\n"
        "    return NSClassFromString(classStringName);\n"
        "}\n",
        '"', '"',
        '"', '%', '%', '"',
        '"', '"',
        '"', '%', '%', '"');
        bridgeFile += mbuf;
        END
        TEST_AS_OBJC
        bridgeFile += "void InitProcDic() {\n";
        bridgeFile += "    s_sceneNodeAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createSceneNodeAgentProcDic = [NSMutableDictionary new];\n";
        bridgeFile += "    s_guiAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createGUIAgentProcDic = [NSMutableDictionary new];\n";
        bridgeFile += "    s_actorAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createActorAgentProcDic = [NSMutableDictionary new];\n";
        ELSE
        bridgeFile += "public func InitProcDic() {\n";
        END
        /// 继承路径，用来判断一个类是否继承自另一个类或接口的依据
        /// 例：RegularSceneNodeAgent 继承自 SceneNodeAgent，SceneNodeAgent 继承自 NSObject
        /// 则 inheritPath[2] = NSObject， inheritPath[1] = SceneNodeAgent，inheritPath[0] = RegularSceneNodeAgent
        xhn::vector<xhn::static_string> inheritPath;
        
        /// 通过这个i可以知道加入的state有多少个
        auto addStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile, language](int& i) {
            xhn::vector<xhn::static_string> stateInheritPath;
            char mbuf[512];
            xhn::string firstState;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 512,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;
            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 512,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            /// 判断是否是派生自State和StateInterface
                            bool isInheritFromState = false;
                            bool isInheritFromStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            stateInheritPath.clear();
                            isInheritFromState = isInheritFromClassProc(strFullClassName, StrSceneNodeState, stateInheritPath);
                            stateInheritPath.clear();
                            isInheritFromStateInterface = isInheritFromClassProc(strFullClassName, StrSceneNodeStateInterface, stateInheritPath);
                            
                            if (isInheritFromState && isInheritFromStateInterface) {
                                /// 真正开始添加state了
                                TEST_AS_OBJC
                                snprintf(mbuf, 512,
                                         "        NSString* state%dName = swiftClassStringFromPath(@%c%s%c);\n"
                                         ///"        id state%d = [[swiftClassFromPath(@%c%s%c) alloc] init];\n"
                                         "        id state%d = [SwiftStates _TtCC12VEngineLogic%s];\n"
                                         "        [ret setState:state%dName state:state%d];\n",
                                         i, '"', fullClassName.c_str(), '"',
                                         i, stateFuncName.c_str(),
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, i);
                                ELSE
                                snprintf(mbuf, 512,
                                         "        let state%dName = swiftClassStringFromPath(%c%s%c)\n"
                                         ///"        id state%d = [[swiftClassFromPath(@%c%s%c) alloc] init];\n"
                                         "        let state%d = SwiftStates._TtCC12VEngineLogic%s()\n"
                                         "        ret.setState(state%dName, state:state%d)\n",
                                         i, '"', fullClassName.c_str(), '"',
                                         i, stateFuncName.c_str(),
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, i);
                                END
                                bridgeFile += mbuf;
                                
                                if (!firstState.size()) {
                                    snprintf(mbuf, 512, "state%d", i);
                                    firstState = mbuf;
                                }
                                
                                i++;
                            }
                        }
                    }
                }
                /// 将第一个state设为默认的state
                if (firstState.size()) {
                    TEST_AS_OBJC
                    snprintf(mbuf, 512,
                             "        [ret setCurrentState:%s];\n",
                             firstState.c_str());
                    ELSE
                    snprintf(mbuf, 512,
                             "        ret.setCurrentState(%s)\n",
                             firstState.c_str());
                    END
                    bridgeFile += mbuf;
                }
            }
        };
        
        /// 通过这个i可以知道加入的action有多少个
        auto addActionsProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile, language](int& i) {
            xhn::vector<xhn::static_string> actionInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 512,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;
            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 512,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromAction = false;
                            bool isInheritFromActionInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string actionFuncName = mbuf;
                            actionFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            actionFuncName += mbuf;
                            actionFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            actionInheritPath.clear();
                            isInheritFromAction = isInheritFromClassProc(strFullClassName, StrAction, actionInheritPath);
                            actionInheritPath.clear();
                            isInheritFromActionInterface = isInheritFromClassProc(strFullClassName, StrActionInterface, actionInheritPath);
                            
                            if (isInheritFromAction && isInheritFromActionInterface) {
                                TEST_AS_OBJC
                                snprintf(mbuf, 512,
                                         ///"        NSString* action%dName = swiftClassStringFromPath(@%c%s%c);\n"
                                         "        id action%d = [SwiftActions _TtCC12VEngineLogic%s];\n"
                                         ///"        id action%d = [[swiftClassFromPath(@%c%s%c) alloc] init];\n"
                                         "#pragma clang diagnostic push\n"
                                         "#pragma clang diagnostic ignored \"-Wundeclared-selector\"\n"
                                         "        if ([action%d respondsToSelector:@selector(resourceName)]) {\n"
                                         "            NSString* action%dResName = [action%d performSelector:@selector(resourceName)];\n"
                                         "            [ret setAction:action%dResName action:action%d];\n"
                                         "        }\n"
                                         "#pragma clang diagnostic pop\n",
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, actionFuncName.c_str(),
                                         i,
                                         i, i,
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, i);
                                ELSE
                                snprintf(mbuf, 512,
                                         ///"        NSString* action%dName = swiftClassStringFromPath(@%c%s%c);\n"
                                         "        let objectGroup%d = SwiftActions._TtCC12VEngineLogic%s()\n"
                                         "        let action%dResName = objectGroup%d.1.resourceName\n"
                                         "        ret.setAction(action%dResName, action:objectGroup%d.0 as AnyObject)\n",
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, actionFuncName.c_str(),
                                         i,
                                         i, i,
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i);
                                END
                                bridgeFile += mbuf;
                                TEST_AS_OBJC
                                if (!firstAction.size()) {
                                    snprintf(mbuf, 512, "action%d", i);
                                    firstAction = mbuf;
                                }
                                ELSE
                                if (!firstAction.size()) {
                                    snprintf(mbuf, 512, "objectGroup%d", i);
                                    firstAction = mbuf;
                                }
                                END
                                
                                i++;
                            }
                        }
                    }
                }
                if (firstAction.size()) {
                    TEST_AS_OBJC
                    snprintf(mbuf, 512,
                             "        [ret setCurrentAction:%s];\n",
                             firstAction.c_str());
                    ELSE
                    snprintf(mbuf, 512,
                             "        ret.setCurrentAction(%s.0 as AnyObject)\n",
                             firstAction.c_str());
                    END
                    bridgeFile += mbuf;
                }
            }
        };
        
        auto addGUIStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile](xhn::static_string& normalState,
                                                                                                                   xhn::static_string& hoveringState,
                                                                                                                   xhn::static_string& selectedState,
                                                                                                                   xhn::static_string& pressedState,
                                                                                                                   xhn::static_string& draggingState) {
            xhn::vector<xhn::static_string> guiStateInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            GUILog("Agent:%s\n", agentClassName.c_str());
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 512,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;

            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
#if USING_AST_LOG
            if (childClassIter != childrenClassMap.end()) {
                GUILog("  Has Child Classes:\n");
            }
            else {
                GUILog("  Has Not Child Classes:\n");
            }
#endif
            if (childClassIter != childrenClassMap.end()) {
#if USING_AST_LOG
                for (auto& c : childClassIter->second) {
                    GUILog("  %s\n", c.c_str());
                }
#endif
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 512,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
#if USING_AST_LOG
                    GUILog("Child Class:%s\n", childClassName.c_str());
                    if (classMap.find(childClassName) != classMap.end()) {
                        GUILog("  Found Child Class Node:\n");
                    }
                    else {
                        GUILog("  Not Found Child Class Node:\n");
                    }
#endif
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
#if USING_AST_LOG
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            GUILog("  Is A Inherited Class And Accessible:\n");
                        }
                        else {
                            GUILog("  Is Not A Inherited Class Or Unaccessible:\n");
                        }
#endif
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            
                            /// 判断是否是派生自GUIState和GUIStateInterface
                            bool isInheritFromGUIState = false;
                            bool isInheritFromGUIStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            guiStateInheritPath.clear();
                            isInheritFromGUIState = isInheritFromClassProc(strFullClassName, StrGUIState, guiStateInheritPath);
                            guiStateInheritPath.clear();
                            isInheritFromGUIStateInterface = isInheritFromClassProc(strFullClassName, StrGUIStateInterface, guiStateInheritPath);
                            
#if USING_AST_LOG
                            if (isInheritFromGUIState && isInheritFromGUIStateInterface) {
                                GUILog("  Is Inherited From GUIState And Is Inherited From GUIState Interface:\n");
                            }
                            else {
                                GUILog("  Is Not Inherited From GUIState Or Is Not Inherited From GUIState Interface:\n");
                            }
#endif
                            if (isInheritFromGUIState && isInheritFromGUIStateInterface) {
                                /// 这里要判断状态的类型
                                /// NormalState、HoveringState、SelectedState、PressedState、DraggingState
                                xhn::string tmpFullClassName = strFullClassName.c_str();
                                euint pos = tmpFullClassName.rfind(".");
#if USING_AST_LOG
                                if (xhn::string::npos != pos) {
                                    GUILog("  Has A Dot:\n");
                                }
                                else {
                                    GUILog("  Has Not A Dot:\n");
                                }
#endif
                                if (xhn::string::npos != pos) {
                                    xhn::string stateName = tmpFullClassName.substr(pos + 1, tmpFullClassName.size() - pos - 1);
                                    ///
                                    snprintf(mbuf, 512,
                                             "        /// stateName = %s\n", stateName.c_str());
                                    bridgeFile += mbuf;
                                    ///
                                    if (stateName == StrNormalState.c_str()) {
                                        normalState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrHoveringState.c_str()) {
                                        hoveringState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrSelectedState.c_str()) {
                                        selectedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrPressedState.c_str()) {
                                        pressedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrDraggingState.c_str()) {
                                        draggingState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrNormalStateBase.c_str()) {
                                        normalState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrHoveringStateBase.c_str()) {
                                        hoveringState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrSelectedStateBase.c_str()) {
                                        selectedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrPressedStateBase.c_str()) {
                                        pressedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrDraggingStateBase.c_str()) {
                                        draggingState = stateFuncName.c_str();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };
        
        m_sceneNodeAgentNameVector.clear();
        m_guiAgentNameVector.clear();
        m_actorAgentNameVector.clear();
        
#if USING_AST_LOG
        {
            for (auto n : m_nodes) {
                ASTNodeLog("#T:%s N:%s\n", n->nodetype.c_str(), n->name.c_str());
                ASTNode* parentNode = n->parent;
                while (parentNode)
                {
                    ASTNodeLog("  #T:%s N:%s\n", parentNode->nodetype.c_str(), parentNode->name.c_str());
                    parentNode = parentNode->parent;
                }
            }
            xhn::Lambda<void (vector<ASTNode*>&, xhn::string)> printNodes;
            printNodes = [&printNodes](vector<ASTNode*>& children, xhn::string indentation){
                for (auto c : children) {
                    ASTNodeLog("%sT:%s, N:%s\n", indentation.c_str(), c->nodetype.c_str(), c->name.c_str());
                    if (c->children) {
                        printNodes(*c->children, indentation + "  ");
                    }
                }
            };
            printNodes(m_roots, "");
            for (auto r : m_roots) {
                ASTNodeLog("###T:%s, N:%s\n", r->nodetype.c_str(), r->name.c_str());
            }
        }
#endif
        
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                if (StrClassDecl == node->nodetype &&
                    (StrPublic == node->access || StrOpen == node->access)) {
                    if (isInheritFromClassProc(node->name, StrSceneNodeAgent, inheritPath)) {
                        /// 这里将创建节点代理的回调放进s_createSceneNodeAgentProcDic里
                        inheritPath.insert(inheritPath.begin(), node->name);
                        char mbuf[1024];
                        TEST_AS_OBJC
                        snprintf(mbuf, 1024,
                                 "    s_createSceneNodeAgentProcDic[@%c%s%c] = [[CreateSceneNodeAgentProc alloc] initWithProc:^(void* sceneNode)\n"
                                 "    {\n"
                                 "        %s* ret = nil;\n"
                                 "        VSceneNode* sn = (__bridge VSceneNode*)((VEngine::VSceneNode*)sceneNode)->m_slot;\n"
                                 "        if (sn) {\n"
                                 "            ret = [[%s alloc] initWithSceneNode:sn];\n"
                                 "        }\n"
                                 "        else {\n"
                                 "            ret = [[%s alloc] initWithSceneNode:[[VSceneNode alloc] initWithVSceneNode:sceneNode]];\n"
                                 "        }\n",
                                 '"', node->name.c_str(), '"',
                                 node->name.c_str(),
                                 node->name.c_str(),
                                 node->name.c_str());
                        ELSE
                        snprintf(mbuf, 1024,
                                 "    s_createSceneNodeAgentProcDic[%c%s%c] = CreateSceneNodeAgentProc(proc:{ \n"
                                 "        ( _ sceneNode : UnsafeRawPointer ) in\n"
                                 "        var swiftSceneNode : VSceneNode? = nil\n"
                                 "        if let sceneNodePtr = VEngine_GetSlotPtr(UnsafeMutableRawPointer(mutating:sceneNode)) {\n"
                                 "            swiftSceneNode = bridgeToObject(ptr : sceneNodePtr) as VSceneNode\n"
                                 "        } else {\n"
                                 "            swiftSceneNode = VSceneNode(vSceneNode : UnsafeMutableRawPointer(mutating:sceneNode))\n"
                                 "            VEngine_SetSlotPtr(UnsafeMutableRawPointer(mutating:sceneNode), \n"
                                 "                               UnsafeMutableRawPointer(mutating:bridgeRetained(obj : swiftSceneNode!)))\n"
                                 "        }\n"
                                 "        let ret = %s(sceneNode : swiftSceneNode!)\n",
                                 '"', node->name.c_str(), '"',
                                 node->name.c_str());
                        END
                        m_sceneNodeAgentNameVector.push_back(node->name);
                        
                        bridgeFile += mbuf;
                        int i = 0;
                        while (inheritPath.size()) {
                            addStatesProc(i);
                            inheritPath.pop_back();
                        }
                        TEST_AS_OBJC
                        bridgeFile +=
                        "        [ret start];\n"
                        "        return ret;\n"
                        "    }];\n";
                        ELSE
                        bridgeFile +=
                        "        ret.start()\n"
                        "        return ret\n"
                        "    })\n";
                        END
                    }
                    else {
                        inheritPath.clear();
                        if (isInheritFromClassProc(node->name, StrActorAgent, inheritPath)) {
                            /// 这里将创建actor代理的回调放在s_createActorAgentProcDic里
                            inheritPath.insert(inheritPath.begin(), node->name);
                            char mbuf[1024];
                            TEST_AS_OBJC
                            snprintf(mbuf, 1024,
                                     "    s_createActorAgentProcDic[@%c%s%c] = [[CreateActorAgentProc alloc] initWithProc:^(void* renderSys, void* actor)\n"
                                     "    {\n"
                                     "        %s* ret = nil;\n"
                                     "        VActor* act = (__bridge VActor*)((VEngine::VActor*)actor)->m_slot;\n"
                                     "        if (act) {\n"
                                     "            ret = [[%s alloc] initWithActor:act];"
                                     "        }\n"
                                     "        else {\n"
                                     "            ret = [[%s alloc] initWithActor:[[VActor alloc] initWithVActor:actor]];\n"
                                     "        }\n",
                                     '"', node->name.c_str(), '"',
                                     node->name.c_str(),
                                     node->name.c_str(),
                                     node->name.c_str());
                            ELSE
                            snprintf(mbuf, 1024,
                                     "    s_createActorAgentProcDic[%c%s%c] = CreateActorAgentProc(proc:{ \n"
                                     "        ( _ renderSys : UnsafeRawPointer, _ actor : UnsafeRawPointer ) in\n"
                                     "        var swiftActor : VActor? = nil\n"
                                     "        if let actorPtr = VEngine_GetSlotPtr(UnsafeMutableRawPointer(mutating:actor)) {\n"
                                     "            swiftActor = bridgeToObject(ptr : actorPtr) as VActor\n"
                                     "        } else {\n"
                                     "            swiftActor = VActor(vActor : UnsafeMutableRawPointer(mutating:actor))\n"
                                     "            VEngine_SetSlotPtr(UnsafeMutableRawPointer(mutating:actor), \n"
                                     "                               UnsafeMutableRawPointer(mutating:bridgeRetained(obj : swiftActor!)))\n"
                                     "        }\n"
                                     "        let ret = %s(actor : swiftActor!)\n",
                                     '"', node->name.c_str(), '"',
                                     node->name.c_str());
                            
                            END
                            m_actorAgentNameVector.push_back(node->name);
                            
                            bridgeFile += mbuf;
                            int i = 0;
                            while (inheritPath.size()) {
                                addActionsProc(i);
                                inheritPath.pop_back();
                            }
                            TEST_AS_OBJC
                            bridgeFile +=
                            "        [ret start];\n"
                            "        return ret;\n"
                            "    }];\n";
                            ELSE
                            bridgeFile +=
                            "        ret.start()\n"
                            "        return ret\n"
                            "    })\n";
                            END
                        }
                        else {
                            inheritPath.clear();
                            if (isInheritFromClassProc(node->name, StrGUIAgent, inheritPath)) {
                                inheritPath.insert(inheritPath.begin(), node->name);
                                
                                for (auto i : inheritPath) {
                                    GUILog("--%s\n", i.c_str());
                                }
                                
                                char mbuf[1024];
                                TEST_AS_OBJC
                                snprintf(mbuf, 1024,
                                         "    s_createGUIAgentProcDic[@%c%s%c] = [[CreateGUIAgentProc alloc] initWithProc:^(void* renderSys, void* widget)\n"
                                         "    {\n"
                                         "        %s* ret = nil;\n"
                                         "        VWidget* wg = (__bridge VWidget*)((VEngine::GUIWidget*)widget)->m_slot;\n"
                                         "        if (wg) {\n"
                                         "            ret = [[%s alloc] initWithWidget:wg];\n"
                                         "        }\n"
                                         "        else {\n"
                                         "            ret = [[%s alloc] initWithWidget:[[VWidget alloc] initWithVWidget:widget]];\n"
                                         "        }\n",
                                         '"', node->name.c_str(), '"',
                                         node->name.c_str(),
                                         node->name.c_str(),
                                         node->name.c_str());
                                ELSE
                                snprintf(mbuf, 1024,
                                         "    s_createGUIAgentProcDic[%c%s%c] = CreateGUIAgentProc(proc:{ \n"
                                         "        ( _ renderSys : UnsafeRawPointer, _ widget : UnsafeRawPointer ) in\n"
                                         "        var swiftWidget : VWidget? = nil\n"
                                         "        if let widgetPtr = VEngine_GetSlotPtr(UnsafeMutableRawPointer(mutating:widget)) {\n"
                                         "            swiftWidget = bridgeToObject(ptr : widgetPtr) as VWidget\n"
                                         "        } else {\n"
                                         "            swiftWidget = VWidget(vWidget : UnsafeMutableRawPointer(mutating:widget))\n"
                                         "            VEngine_SetSlotPtr(UnsafeMutableRawPointer(mutating:widget), \n"
                                         "                               UnsafeMutableRawPointer(mutating:bridgeRetained(obj : swiftWidget!)))\n"
                                         "        }\n"
                                         "        let ret = %s(widget : swiftWidget!)\n",
                                         '"', node->name.c_str(), '"',
                                         node->name.c_str());
                                END
                                m_guiAgentNameVector.push_back(node->name);
                                
                                bridgeFile += mbuf;
                                ///
                                /// 这里配置
                                xhn::static_string normalState = xhn::static_string::empty_string;
                                xhn::static_string hoveringState = xhn::static_string::empty_string;
                                xhn::static_string selectedState = xhn::static_string::empty_string;
                                xhn::static_string pressedState = xhn::static_string::empty_string;
                                xhn::static_string draggingState = xhn::static_string::empty_string;
                                
                                while (inheritPath.size()) {
                                    addGUIStatesProc(normalState, hoveringState, selectedState, pressedState, draggingState);
                                    inheritPath.pop_back();
                                }
                                
                                EAssert(xhn::static_string::empty_string != normalState &&
                                        xhn::static_string::empty_string != hoveringState &&
                                        xhn::static_string::empty_string != selectedState &&
                                        xhn::static_string::empty_string != pressedState &&
                                        xhn::static_string::empty_string != draggingState, "gui state member are incomplete!, %s, %s, %s, %s, %s",
                                        normalState.c_str(), hoveringState.c_str(), selectedState.c_str(), pressedState.c_str(), draggingState.c_str());
                                ///
                                TEST_AS_OBJC
                                snprintf(mbuf, 512, "        id normalState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", normalState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        id hoveringState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", hoveringState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        id selectedState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", selectedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        id pressedState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", pressedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        id draggingState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", draggingState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        [ret setState:@\"NormalState\" state:normalState];\n"
                                                    "        [ret setState:@\"HoveringState\" state:hoveringState];\n"
                                                    "        [ret setState:@\"SelectedState\" state:selectedState];\n"
                                                    "        [ret setState:@\"PressedState\" state:pressedState];\n"
                                                    "        [ret setState:@\"DraggingState\" state:draggingState];\n"
                                                    "        [ret setCurrentState:normalState];\n");
                                ELSE
                                snprintf(mbuf, 512, "        let normalState = SwiftGUIStates._TtCC12VEngineLogic%s()\n", normalState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        let hoveringState = SwiftGUIStates._TtCC12VEngineLogic%s()\n", hoveringState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        let selectedState = SwiftGUIStates._TtCC12VEngineLogic%s()\n", selectedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        let pressedState = SwiftGUIStates._TtCC12VEngineLogic%s()\n", pressedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512, "        let draggingState = SwiftGUIStates._TtCC12VEngineLogic%s()\n", draggingState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 512,
                                         "        ret.setState(\"NormalState\", state:normalState)\n"
                                         "        ret.setState(\"HoveringState\", state:hoveringState)\n"
                                         "        ret.setState(\"SelectedState\", state:selectedState)\n"
                                         "        ret.setState(\"PressedState\", state:pressedState)\n"
                                         "        ret.setState(\"DraggingState\", state:draggingState)\n"
                                         "        ret.setCurrentState(normalState)\n");
                                END
                                bridgeFile += mbuf;
                                ///
                                TEST_AS_OBJC
                                bridgeFile +=
                                "        [ret start];\n"
                                "        return ret;\n"
                                "    }];\n";
                                ELSE
                                bridgeFile +=
                                "        ret.start()\n"
                                "        return ret\n"
                                "    })\n";
                                END
                            }
                        }
                    }
                }
            }
        }
        
        bridgeFile += "}\n";
        TEST_AS_OBJC
        bridgeFile += "void* CreateSceneNodeAgent(const char* type, void* sceneNode) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateSceneNodeAgentProc* proc = s_createSceneNodeAgentProcDic[strType];\n";
        bridgeFile += "    SceneNodeAgent* agent = proc.createAgentProc(sceneNode);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_sceneNodeAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveSceneNodeAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_sceneNodeAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateSceneNodeAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "void SceneNodeAgentGotoState(void* agent, const char* nextStateName) {\n";
        bridgeFile += "    SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    NSString* _Nonnull strNextStateName = [NSString stringWithUTF8String:nextStateName];\n";
        bridgeFile += "    [agentID gotoState:strNextStateName];\n";
        bridgeFile += "}\n";
        bridgeFile += "void* CreateGUIAgent(const char* type, void* renderSys, void* widget) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateGUIAgentProc* proc = s_createGUIAgentProcDic[strType];\n";
        bridgeFile += "    GUIAgent* agent = proc.createAgentProc(renderSys, widget);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_guiAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveGUIAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_guiAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateGUIAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "void GotoGUIAgentState(void* agent, int stateKind) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    NSString* strStateKind = nil;\n";
        bridgeFile += "    switch (stateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "        case 0: strStateKind = @\"NormalState\"; break;\n";
        bridgeFile += "        case 1: strStateKind = @\"HoveringState\"; break;\n";
        bridgeFile += "        case 2: strStateKind = @\"SelectedState\"; break;\n";
        bridgeFile += "        case 3: strStateKind = @\"PressedState\"; break;\n";
        bridgeFile += "        case 4: strStateKind = @\"DraggingState\"; break;\n";
        bridgeFile += "        default: break;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    if (strStateKind) {\n";
        bridgeFile += "        [agentID gotoState:strStateKind];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "bool AllowGUIStateChange(void* agent, int prevStateKind, int nextStateKind) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    NSString* strPrevStateKind = nil;\n";
        bridgeFile += "    switch (prevStateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "        case 0: strPrevStateKind = @\"NormalState\"; break;\n";
        bridgeFile += "        case 1: strPrevStateKind = @\"HoveringState\"; break;\n";
        bridgeFile += "        case 2: strPrevStateKind = @\"SelectedState\"; break;\n";
        bridgeFile += "        case 3: strPrevStateKind = @\"PressedState\"; break;\n";
        bridgeFile += "        case 4: strPrevStateKind = @\"DraggingState\"; break;\n";
        bridgeFile += "        default: break;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    NSString* strNextStateKind = nil;\n";
        bridgeFile += "    switch (nextStateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "        case 0: strNextStateKind = @\"NormalState\"; break;\n";
        bridgeFile += "        case 1: strNextStateKind = @\"HoveringState\"; break;\n";
        bridgeFile += "        case 2: strNextStateKind = @\"SelectedState\"; break;\n";
        bridgeFile += "        case 3: strNextStateKind = @\"PressedState\"; break;\n";
        bridgeFile += "        case 4: strNextStateKind = @\"DraggingState\"; break;\n";
        bridgeFile += "        default: break;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    if (strPrevStateKind && strNextStateKind) {\n";
        bridgeFile += "        return [agentID allowStateChange:strPrevStateKind nextStateKind:strNextStateKind];\n";
        bridgeFile += "    }\n";
        bridgeFile += "    else {\n";
        bridgeFile += "        return false;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void OnGUIWidgetMove(void* agent, float dispX, float dispY) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID onMove:dispX dispY:dispY];\n";
        bridgeFile += "}\n";
        bridgeFile += "void OnGUIWidgetPress(void* agent) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID onPress];\n";
        bridgeFile += "}\n";
        bridgeFile += "void OnGUIWidgetLeave(void* agent) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID onLeave];\n";
        bridgeFile += "}\n";
        bridgeFile += "void OnGUIWidgetHover(void* agent) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID onHover];\n";
        bridgeFile += "}\n";
        bridgeFile += "void* CreateActorAgent(const char* type, void* renderSys, void* actor) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateActorAgentProc* proc = s_createActorAgentProcDic[strType];\n";
        bridgeFile += "    ActorAgent* agent = proc.createAgentProc(renderSys, actor);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_actorAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveActorAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        ActorAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_actorAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateActorAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    ActorAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "int GetNumSceneNodeAgentProcs() {\n";
        bridgeFile += "    return (int)[s_createSceneNodeAgentProcDic count];\n";
        bridgeFile += "}\n";
        bridgeFile += "int GetNumActorAgentProcs() {\n";
        bridgeFile += "    return (int)[s_createActorAgentProcDic count];\n";
        bridgeFile += "}\n";
        bridgeFile += "char** GetSceneNodeAgentProcTypes() {\n";
        bridgeFile += "    char** ret = (char**)malloc(sizeof(char*) * [s_createSceneNodeAgentProcDic count]);\n";
        bridgeFile += "    NSEnumerator *enumeratorKey = [s_createSceneNodeAgentProcDic keyEnumerator];\n";
        bridgeFile += "    int i = 0;\n";
        bridgeFile += "    for (NSString* key in enumeratorKey) {\n";
        bridgeFile += "        int len = (int)strlen([key UTF8String]);\n";
        bridgeFile += "        ret[i] = (char*)malloc(len + 1);\n";
        bridgeFile += "        memcpy(ret[i], [key UTF8String], len + 1);\n";
        bridgeFile += "        i++;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    return ret;\n";
        bridgeFile += "}\n";
        bridgeFile += "char** GetActorAgentProcTypes() {\n";
        bridgeFile += "    char** ret = (char**)malloc(sizeof(char*) * [s_createActorAgentProcDic count]);\n";
        bridgeFile += "    NSEnumerator *enumeratorKey = [s_createActorAgentProcDic keyEnumerator];\n";
        bridgeFile += "    int i = 0;\n";
        bridgeFile += "    for (NSString* key in enumeratorKey) {\n";
        bridgeFile += "        int len = (int)strlen([key UTF8String]);\n";
        bridgeFile += "        ret[i] = (char*)malloc(len + 1);\n";
        bridgeFile += "        memcpy(ret[i], [key UTF8String], len + 1);\n";
        bridgeFile += "        i++;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    return ret;\n";
        bridgeFile += "}\n";
        bridgeFile += "void SetPrintLogProc(void (*printLogProc)(int, const char*)) {\n";
        bridgeFile += "    s_printLogProc = printLogProc;\n";
        bridgeFile += "}\n";
        bridgeFile += "void PrintLog(int type, const char* log) {\n";
        bridgeFile += "    if (s_printLogProc) {\n";
        bridgeFile += "        s_printLogProc(type, log);\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
//        bridgeFile += "void LoadScene(const char* sceneName, const char* sceneFilePath, void* renderSys, void* removedSceneNodes, void* sceneNodeFilter, void* callback) {\n";
//        bridgeFile += "    NSArray<NSString*>* removedSceneNodeNames = (__bridge id)removedSceneNodes;\n";
//        bridgeFile += "    VEngine::SceneNodeNameVectorPtr sceneNodeVector = VNEW VEngine::SceneNodeNameVector();\n";
//        bridgeFile += "    for (NSString* name in removedSceneNodeNames) {\n";
//        bridgeFile += "        sceneNodeVector->push_back([name UTF8String]);\n";
//        bridgeFile += "    }\n";
//        bridgeFile += "    xhn::SmartPtr<SwiftCallback> swiftCallback = VNEW SwiftCallback();\n";
//        bridgeFile += "    swiftCallback->m_callback = (__bridge void (^) (void))callback;\n";
//        bridgeFile += "    auto channel = VEngine::VRobotManager::Get((VEngine::VRenderSystem*)renderSys)->GetChannel(VEngine::VScriptRobot::StrScriptRobot,\n";
//        bridgeFile += "                                                                                               VEngine::VSceneRobot::StrSceneRobot);\n";
//        bridgeFile += "    xhn::string strSceneName(sceneName);\n";
//        bridgeFile += "    xhn::string strSceneFilePath(sceneFilePath);\n";
//        bridgeFile += "    SwiftSceneNodeFilter* filter = VNEW SwiftSceneNodeFilter();\n";
//        bridgeFile += "    filter->m_swiftSceneNodeFilter = (__bridge id)sceneNodeFilter;\n";
//        bridgeFile += "    VEngine::VSceneNodeFilterPtr swiftNodeFilter = filter;\n";
//        bridgeFile += "    xhn::Lambda<void (VEngine::VRobotCommandReceipt*)>\n";
//        bridgeFile += "    deleteSceneNodesCallback([strSceneName, strSceneFilePath, renderSys, swiftNodeFilter, swiftCallback](VEngine::VRobotCommandReceipt*){\n";
//        bridgeFile += "        auto channel = VEngine::VRobotManager::Get((VEngine::VRenderSystem*)renderSys)->GetChannel(VEngine::VScriptRobot::StrScriptRobot,\n";
//        bridgeFile += "                                                                                                   VEngine::VSceneRobot::StrSceneRobot);\n";
//        bridgeFile += "        xhn::Lambda<void (VEngine::VRobotCommandReceipt*)> loadSceneCallback([renderSys, swiftCallback](VEngine::VRobotCommandReceipt* rec){\n";
//        bridgeFile += "            VEngine::VLoadSceneCommandReceipt* lscr = rec->DynamicCast<VEngine::VLoadSceneCommandReceipt>();\n";
//        bridgeFile += "            auto channel = VEngine::VRobotManager::Get((VEngine::VRenderSystem*)renderSys)->GetChannel(VEngine::VScriptRobot::StrScriptRobot,\n";
//        bridgeFile += "                                                                                                       VEngine::VSceneRobot::StrSceneRobot);\n";
//        bridgeFile += "            xhn::Lambda<void (VEngine::VRobotCommandReceipt*)> loadAgentsCallback([swiftCallback](VEngine::VRobotCommandReceipt*){\n";
//        bridgeFile += "                swiftCallback->m_callback();\n";
//        bridgeFile += "            });\n";
//        bridgeFile += "            VEngine::VLoadAgentsCommand lac((VEngine::VRenderSystem*)renderSys,\n";
//        bridgeFile += "                                            lscr->m_sceneNodeVector.get(),\n";
//        bridgeFile += "                                            loadAgentsCallback);\n";
//        bridgeFile += "            channel->Write(lac);\n";
//        bridgeFile += "        });\n";
//        bridgeFile += "        VEngine::VLoadSceneCommand lsc((VEngine::VRenderSystem*)renderSys,\n";
//        bridgeFile += "                                       strSceneName.c_str(), strSceneFilePath.c_str(),\n";
//        bridgeFile += "                                       swiftNodeFilter, nullptr, loadSceneCallback, false);\n";
//        bridgeFile += "        channel->Write(lsc);\n";
//        bridgeFile += "    });\n";
//        bridgeFile += "    VEngine::VDeleteSceneNodesCommand dsnc((VEngine::VRenderSystem*)renderSys, sceneNodeVector, deleteSceneNodesCallback);\n";
//        bridgeFile += "    channel->Write(dsnc);\n";
//        bridgeFile += "}\n";
        END
        ///printf("%s\n", bridgeFile.c_str());
        return bridgeFile;
    }
    string SwiftParser::CreateStateActionFile(const map<static_string, vector<static_string>>& inheritMap,
                                              const map<static_string, vector<static_string>>& childrenClassMap,
                                              const map<static_string, ASTNode*>& classMap,
                                              Lambda<bool (static_string, static_string,
                                                           vector<static_string>&)>& isInheritFromClassProc)
    {
        string actionFile = "import Foundation\n";
        
        /// func class
        map<string, string> stateFuncClassMap;
        /// func class
        map<string, string> guiStateFuncClassMap;
        /// func class
        map<string, string> actionFuncClassMap;
        
        /// 继承路径，用来判断一个类是否继承自另一个类或接口的依据
        vector<static_string> inheritPath;
        
        auto addStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &stateFuncClassMap]() {
            xhn::vector<xhn::static_string> stateInheritPath;
            char mbuf[512];
            xhn::string firstState;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromState = false;
                            bool isInheritFromStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            stateInheritPath.clear();
                            isInheritFromState = isInheritFromClassProc(strFullClassName, StrSceneNodeState, stateInheritPath);
                            stateInheritPath.clear();
                            isInheritFromStateInterface = isInheritFromClassProc(strFullClassName, StrSceneNodeStateInterface, stateInheritPath);
                            
                            if (isInheritFromState && isInheritFromStateInterface) {
                                stateFuncClassMap.insert(make_pair(stateFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        auto addGUIStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &guiStateFuncClassMap]() {
            xhn::vector<xhn::static_string> guiStateInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            
                            /// 判断是否是派生自GUIState和GUIStateInterface
                            bool isInheritFromGUIState = false;
                            bool isInheritFromGUIStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            guiStateInheritPath.clear();
                            isInheritFromGUIState = isInheritFromClassProc(strFullClassName, StrGUIState, guiStateInheritPath);
                            guiStateInheritPath.clear();
                            isInheritFromGUIStateInterface = isInheritFromClassProc(strFullClassName, StrGUIStateInterface, guiStateInheritPath);
                            
                            if (isInheritFromGUIState && isInheritFromGUIStateInterface) {
                                guiStateFuncClassMap.insert(make_pair(stateFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        auto addActionsProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &actionFuncClassMap]() {
            vector<static_string> actionInheritPath;
            char mbuf[512];
            string firstAction;
            static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromAction = false;
                            bool isInheritFromActionInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 512, "%lld", node->name.size());
                            xhn::string actionFuncName = mbuf;
                            actionFuncName += node->name.c_str();
                            snprintf(mbuf, 512, "%lld", child->name.size());
                            actionFuncName += mbuf;
                            actionFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            
                            actionInheritPath.clear();
                            isInheritFromAction = isInheritFromClassProc(strFullClassName, StrAction, actionInheritPath);
                            actionInheritPath.clear();
                            isInheritFromActionInterface = isInheritFromClassProc(strFullClassName, StrActionInterface, actionInheritPath);
                            
                            if (isInheritFromAction && isInheritFromActionInterface) {
                                actionFuncClassMap.insert(make_pair(actionFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        m_sceneNodeAgentNameVector.clear();
        m_guiAgentNameVector.clear();
        m_actorAgentNameVector.clear();
        
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                if (StrClassDecl == node->nodetype &&
                    (StrPublic == node->access || StrOpen == node->access)) {
                    if (isInheritFromClassProc(node->name, StrSceneNodeAgent, inheritPath)) {
                        /// 这里将创建节点代理的回调放进s_createSceneNodeAgentProcDic里
                        inheritPath.insert(inheritPath.begin(), node->name);
                        
                        m_sceneNodeAgentNameVector.push_back(node->name);
                        
                        while (inheritPath.size()) {
                            addStatesProc();
                            inheritPath.pop_back();
                        }
                        
                    }
                    else {
                        inheritPath.clear();
                        if (isInheritFromClassProc(node->name, StrActorAgent, inheritPath)) {
                            /// 这里将创建actor代理的回调放在s_createActorAgentProcDic里
                            inheritPath.insert(inheritPath.begin(), node->name);
                            
                            m_actorAgentNameVector.push_back(node->name);
                            
                            while (inheritPath.size()) {
                                addActionsProc();
                                inheritPath.pop_back();
                            }
                            
                        }
                        else {
                            inheritPath.clear();
                            if (isInheritFromClassProc(node->name, StrGUIAgent, inheritPath)) {
                                /// 这里将创建actor代理的回调放在s_createGUIAgentProcDic里
                                inheritPath.insert(inheritPath.begin(), node->name);
                                
                                m_guiAgentNameVector.push_back(node->name);
                                
                                while (inheritPath.size()) {
                                    addGUIStatesProc();
                                    inheritPath.pop_back();
                                }
                            }
                        }
                    }
                }
            }
        }
        
        printf("Beginning make action file...\n");
        
        actionFile += "open class SwiftStates : NSObject {\n";
        
        for (auto& p : stateFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 512,
                     "    static open func _TtCC12VEngineLogic%s() -> AnyObject {\n"
                     "        return %s();\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            
            printf("  _TtCC12VEngineLogic%s\n", p.first.c_str());
            
            actionFile += mbuf;
        }
        
        actionFile += "}\n";
        
        actionFile += "open class SwiftGUIStates : NSObject {\n";
        for (auto& p : guiStateFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 512,
                     "    static open func _TtCC12VEngineLogic%s() -> AnyObject {\n"
                     "        return %s();\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            
            printf("  _TtCC12VEngineLogic%s\n", p.first.c_str());
            
            actionFile += mbuf;
        }
        actionFile += "}\n";
        
        actionFile += "open class SwiftActions : NSObject {\n";
        
        for (auto& p : actionFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 512,
                     "    static open func _TtCC12VEngineLogic%s() -> (Action, AnimationInterface, ActionInterface) {\n"
                     "        let ret = %s();\n"
                     "        return (ret as Action, ret as AnimationInterface, ret as ActionInterface);\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            
            printf("  _TtCC12VEngineLogic%s\n", p.first.c_str());
            
            actionFile += mbuf;
        }
        
        actionFile += "}\n";
        
        printf("Completed...\n");
        
        return actionFile;
    }
    void SwiftParser::Parse(const char* strBuffer, euint length)
    {
        xhn::string parsedString;
        const char* parsedBuffer = strBuffer;
        euint parsedLength = length;
        if (m_reformatter) {
            parsedString = m_reformatter->Reformat(strBuffer, length);
            parsedBuffer = parsedString.c_str();
            parsedLength = parsedString.size();
        }
        
        euint count = 0;
        
        auto reduce = [this]()
        {
            if (m_isNodeType) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (StrSourceFile == symbol) {
                    m_roots.push_back(m_nodeStack.back());
                }
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->nodetype = symbol;
                    if (StrTypealias == symbol) {
                        ASTLog("TYPEALIAS\n");
                    }
                }
                m_isNodeType = false;
                m_isName = true;
            }
            else if (m_isAccess) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->access = symbol;
                }
                m_isAccess = false;
            }
            else if (m_isDecl) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->decl = symbol;
                }
                m_isDecl = false;
            }
            else if (m_isType) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    if (!m_isInterface) {
                        m_nodeStack.back()->type = symbol;
                    }
                    else {
                        m_nodeStack.back()->interfacetype = symbol;
                        m_isInterface = false;
                    }
                }
                m_isType = false;
            }
            else {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                if (StrInherits == symbol) {
                    m_isInherits = true;
                }
                else if (StrAccess == symbol) {
                    m_isAccess = true;
                }
                else if (StrDecl == symbol) {
                    m_isDecl = true;
                }
                else if (StrType == symbol) {
                    m_isType = true;
                }
                else if (StrInterface == symbol) {
                    m_isInterface = true;
                }
                else {
                    ASTLog("SYMBOL=%s\n", symbol.c_str());
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
        while (count < parsedLength) {
            char c = parsedBuffer[count];
//            ASTLog("%c, m_isNodeType %d, m_isName %d, m_isInterface %d, m_isApostropheBlock %d, m_isQuotationBlock %d\n",
//                   c,   m_isNodeType,    m_isName,    m_isInterface,    m_isApostropheBlock,    m_isQuotationBlock);
            switch (c)
            {
                case '\0':
                    break;
                case '(':
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        m_symbolBuffer.bufferTop = 0;
                        m_isName = false;
                        ASTNode* currentNode = VNEW ASTNode();
                        if (m_nodeStack.size()) {
                            ASTNode* parentNode = m_nodeStack.back();
                            if (!parentNode->children) {
                                parentNode->children = VNEW xhn::vector<ASTNode*>();
                            }
                            parentNode->children->push_back(currentNode);
                            currentNode->parent = parentNode;
                        }
                        m_nodeStack.push_back(currentNode);
                        m_nodes.push_back(currentNode);
                        m_isNodeType = true;
                        ASTLog("NEW NODE\n");
                    }
                    else {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                case ')':
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        m_isName = false;
                        reduce();
                        m_symbolBuffer.bufferTop = 0;
#if USING_AST_LOG
                        ASTNode* currentNode = m_nodeStack.back();
                        ASTNode* parentNode = currentNode->parent;
                        ASTLog("NODETYPE=%s, NAME=%s, ACCESS=%s, DECL=%s, TYPE=%s, INTERFACETYPE=%s\n",
                               currentNode->nodetype.c_str(),
                               currentNode->name.c_str(),
                               currentNode->access.c_str(),
                               currentNode->decl.c_str(),
                               currentNode->type.c_str(),
                               currentNode->interfacetype.c_str());
                        if (parentNode) {
                            ASTLog("%s <- %s\n", parentNode->nodetype.c_str(), currentNode->nodetype.c_str());
                        }
                        ASTLog("PUSH TO PARENT NODE\n");
#endif
                        m_nodeStack.pop_back();
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
                                ASTLog("NODETYPE=%s NAME=%s\n",
                                       m_nodeStack.back()->nodetype.c_str(), m_nodeStack.back()->name.c_str());
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
                    if (m_isApostropheBlock ||
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduce();
                        m_symbolBuffer.bufferTop = 0;
                    }
                    break;
                case '\n':
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    if (m_isApostropheBlock ||
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduce();
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
    void SwiftParser::ParseSwifts(const string& logDir,
                                  ASTReformatterPtr reformatter,
                                  const string& importPaths,
                                  const string& paths, xhn::Lambda<void (const xhn::string& objcBridgeFile,
                                                                         const xhn::string& swiftBridgeFile,
                                                                         const xhn::string& stateActionFile,
                                                                         const xhn::vector<xhn::static_string>&,
                                                                         const xhn::vector<xhn::static_string>&,
                                                                         const xhn::vector<xhn::static_string>&)>& callback)
    {
        SwiftCommandLineUtil* sclu = [SwiftCommandLineUtil new];
        NSString* command = [[NSString alloc] initWithFormat:@"%@ -swift-version %@ -sdk %@ -dump-ast %@ %@",
                             swiftc, usedVersion, sdk,
                             [[NSString alloc] initWithUTF8String:importPaths.c_str()],
                             [[NSString alloc] initWithUTF8String:paths.c_str()]];
        __block xhn::Lambda<void (const xhn::string& objcBridgeFile,
                                  const xhn::string& swiftBridgeFile,
                                  const xhn::string& stateActionFile,
                                  const xhn::vector<xhn::static_string>&,
                                  const xhn::vector<xhn::static_string>&,
                                  const xhn::vector<xhn::static_string>&)> tmpCallback = callback;
        void (^objcCallback)(const xhn::string& objcBridgeFile,
                             const xhn::string& swiftBridgeFile,
                             const xhn::string& stateActionFile,
                             const xhn::vector<xhn::static_string>&,
                             const xhn::vector<xhn::static_string>&,
                             const xhn::vector<xhn::static_string>&)  = ^(const xhn::string& objcBridgeFile,
                                                                          const xhn::string& swiftBridgeFile,
                                                                          const xhn::string& stateActionFile,
                                                                          const xhn::vector<xhn::static_string>& sceneNodeAgentNames,
                                                                          const xhn::vector<xhn::static_string>& guiAgentNames,
                                                                          const xhn::vector<xhn::static_string>& actorAgentNames) {
            tmpCallback(objcBridgeFile, swiftBridgeFile, stateActionFile,
                        sceneNodeAgentNames, guiAgentNames, actorAgentNames);
        };
        [sclu parseSwiftSourceFiles:command
                             logDir:logDir
                        reformatter:reformatter
                           callback:objcCallback];
    }
    SwiftParser::SwiftParser(const string& logDir,
                             ASTReformatterPtr reformatter)
    : m_isApostropheBlock(false)
    , m_isQuotationBlock(false)
    , m_isNodeType(false)
    , m_isName(false)
    , m_isInterface(false)
    , m_isInherits(false)
    , m_isAccess(false)
    , m_isDecl(false)
    , m_isType(false)
    , m_reformatter(reformatter)
    {
#if USING_AST_LOG
        s_ASTLogFile = fopen((logDir + "/swiftParseLog.txt").c_str(), "wb");
        s_ASTNodeLogFile = fopen((logDir + "/swiftParseNodeLog.txt").c_str(), "wb");
        s_ClassHierarchyLogFile = fopen((logDir + "/swiftParserClassHierarchyLog.txt").c_str(), "wb");
        s_GUILogFile = fopen((logDir + "/swiftParseGUILog.txt").c_str(), "wb");
        s_ASTFile = fopen((logDir + "/swiftParseAst.txt").c_str(), "wb");
#endif
    }
    SwiftParser::~SwiftParser()
    {
#if USING_AST_LOG
        fclose(s_ASTLogFile);
        fclose(s_ASTNodeLogFile);
        fclose(s_ClassHierarchyLogFile);
        fclose(s_GUILogFile);
        fclose(s_ASTFile);
#endif
    }
}

@implementation SwiftCommandLineUtil
{
    void(^mGetSwiftVersionCallback)(const xhn::string&);
    void(^mSwiftParserCallback)(const xhn::string& objcBridgeFile,
                                const xhn::string& swiftBridgeFile,
                                const xhn::string& stateActionFile,
                                const xhn::vector<xhn::static_string>&,
                                const xhn::vector<xhn::static_string>&,
                                const xhn::vector<xhn::static_string>&);
    NSTask *mTask;
    NSPipe *mPipe;
    NSFileHandle *mFile;
}
- (id) init
{
    self = [super init];
    if (self) {
        self.parseMask = PARSE_MASK;
    }
    return self;
}
- (void) dealloc
{
}

- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];

        self.parser->Parse((const char*)[data bytes], [data length]);
#if USING_AST_LOG
        if (s_ASTFile) {
            fwrite([data bytes], 1, [data length], s_ASTFile);
        }
#endif
    }
    else {
        if (mGetSwiftVersionCallback) {
            xhn::string versionInfo;
            xhn::SwiftVerisonInfoParser* versionParser = self.parser->DynamicCast<xhn::SwiftVerisonInfoParser>();
            EDebugAssert(versionParser, "current parser must be a kind of SwiftVerisonInfoParser");
            versionParser->EndParser(versionInfo);
            delete versionParser;
            self.parser = nil;
            [[NSNotificationCenter defaultCenter] removeObserver:self];
            {
                auto inst = s_SwiftCommandLineUtilsLock.Lock();
                if (s_SwiftCommandLineUtils) {
                    [s_SwiftCommandLineUtils removeObject:self];
                }
            }
            mGetSwiftVersionCallback(versionInfo);
            mGetSwiftVersionCallback = nil;
        }
        else if (mSwiftParserCallback) {
            xhn::string objcBridgeFile;
            xhn::string swiftBridgeFile;
            xhn::string stateActionFile;
            xhn::string tmp;
            xhn::SwiftParser* swiftParser = self.parser->DynamicCast<xhn::SwiftParser>();
            EDebugAssert(swiftParser, "current parser must be a kind of SwiftParser");
            if ( self.parseMask & OBJC_MASK ) {
                swiftParser->EndParse(objcBridgeFile, stateActionFile, xhn::SwiftParser::ObjC);
            }
            if ( self.parseMask & SWIFT_MASK ) {
                swiftParser->EndParse(swiftBridgeFile, stateActionFile, xhn::SwiftParser::Swift);
            }
            xhn::vector<xhn::static_string> sceneNodeAgentNameVector = swiftParser->GetSceneNodeAgentNameVector();
            xhn::vector<xhn::static_string> guiAgentNameVector = swiftParser->GetGUIAgentNameVector();
            xhn::vector<xhn::static_string> actorAgentNameVector = swiftParser->GetActorAgentNameVector();
            ASTLog("%s\n", bridgeFile.c_str());
            ASTLog("%s\n", stateActionFile.c_str());
            delete swiftParser;
            self.parser = nil;
            [[NSNotificationCenter defaultCenter] removeObserver:self];
            {
                auto inst = s_SwiftCommandLineUtilsLock.Lock();
                if (s_SwiftCommandLineUtils) {
                    [s_SwiftCommandLineUtils removeObject:self];
                }
            }
            mSwiftParserCallback(objcBridgeFile, swiftBridgeFile, stateActionFile,
                                 sceneNodeAgentNameVector, guiAgentNameVector, actorAgentNameVector);
            mSwiftParserCallback = nil;
        }
    }
}
- (void) getSwiftVersion:(NSString*)commandToRun
                  logDir:(const xhn::string&)logDir
                callback:(void(^)(const xhn::string&))proc
{
    mGetSwiftVersionCallback = proc;
    mTask = [[NSTask alloc] init];
    [mTask setLaunchPath: @"/bin/sh"];
    
    NSArray *arguments = [NSArray arrayWithObjects:
                          @"-c" ,
                          [NSString stringWithFormat:@"%@", commandToRun],
                          nil];
#if USING_AST_LOG
    s_COMMANDFile = fopen((logDir + "/swiftParserCommand.txt").c_str(), "wb");
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
    self.parser = VNEW xhn::SwiftVerisonInfoParser();
    self.parser->BeginParse();
    [mTask launch];
}

- (void) parseSwiftSourceFiles:(NSString*)commandToRun
                        logDir:(const xhn::string&)logDir
                   reformatter:(xhn::ASTReformatterPtr)reformatter
                      callback:(void(^)(const xhn::string& objcBridgeFile,
                                        const xhn::string& swiftBridgeFile,
                                        const xhn::string& stateActionFile,
                                        const xhn::vector<xhn::static_string>&,
                                        const xhn::vector<xhn::static_string>&,
                                        const xhn::vector<xhn::static_string>&))proc
{
    mSwiftParserCallback = proc;
    mTask = [[NSTask alloc] init];
    [mTask setLaunchPath: @"/bin/sh"];
    
    printf("Run command:\n");
    printf("%s\n", [commandToRun UTF8String]);
    
    NSArray *arguments = [NSArray arrayWithObjects:
                          @"-c" ,
                          [NSString stringWithFormat:@"%@", commandToRun],
                          nil];
#if USING_AST_LOG
    s_COMMANDFile = fopen((logDir + "/swiftParserCommand.txt").c_str(), "wb");
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
    self.parser = VNEW xhn::SwiftParser(logDir, reformatter);
    self.parser->BeginParse();
    [mTask launch];
}
@end
